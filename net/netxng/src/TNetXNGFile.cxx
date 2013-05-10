//------------------------------------------------------------------------------
// Copyright (c) 2013 by European Organization for Nuclear Research (CERN)
// Author: Lukasz Janyst <ljanyst@cern.ch>
// Author: Justin Salmon <jsalmon@cern.ch>
//------------------------------------------------------------------------------

#include "TNetXNGFile.h"
#include <XrdCl/XrdClURL.hh>
#include <XrdCl/XrdClFile.hh>
#include <XrdCl/XrdClXRootDResponses.hh>
#include <iostream>

ClassImp(TNetXNGFile);

//------------------------------------------------------------------------------
//! Constructor
//------------------------------------------------------------------------------
TNetXNGFile::TNetXNGFile( const char  *url,
                          Option_t    *mode,
                          const char  *title,
                          Int_t        compress,
                          Int_t        /*netopt*/,
                          Bool_t       /*parallelopen*/ ):
  TFile( url, "NET", title, compress )
{
  using namespace XrdCl;
  Info( "TNetXNGFile", "Creating TNetXNGFile" );

  fFile = new File();
  fUrl  = new URL( std::string( url ) );
  fUrl->SetProtocol( std::string( "root" ) );

  fMode = ParseOpenMode( mode );

  fFile->Open( fUrl->GetURL(), fMode );
  TFile::Init( false );
}

//------------------------------------------------------------------------------
//! Destructor
//------------------------------------------------------------------------------
TNetXNGFile::~TNetXNGFile()
{
  if( IsOpen() )
    Close();
  delete fFile;
  delete fUrl;
}

//------------------------------------------------------------------------------
//! Parse an file open mode given as a string into an integer that the
//! client can use
//------------------------------------------------------------------------------
XrdCl::OpenFlags::Flags TNetXNGFile::ParseOpenMode( Option_t *modestr )
{
  Info( "TNetXNGFile", "ParseOpenMode" );
  using namespace XrdCl;
  OpenFlags::Flags mode = OpenFlags::None;
  TString mod = ToUpper( TString( modestr ) );

  if( mod == "NEW" || mod == "CREATE" ) mode = OpenFlags::New;
  else if ( mod == "RECREATE" )         mode = OpenFlags::Delete;
  else if ( mod == "UPDATE" )           mode = OpenFlags::Update;
  else if ( mod == "READ" )             mode = OpenFlags::Read;

  return mode;
}

//------------------------------------------------------------------------------
//! Get the file size
//------------------------------------------------------------------------------
Long64_t TNetXNGFile::GetSize() const
{
  using namespace XrdCl;
  StatInfo *info = 0;

  fFile->Stat( false, info );
  Long64_t size = info->GetSize();
  delete info;
  return size;
}

//------------------------------------------------------------------------------
//! Is the file open?
//------------------------------------------------------------------------------
Bool_t TNetXNGFile::IsOpen() const
{
  return fFile->IsOpen();
}

//------------------------------------------------------------------------------
//! Close the file
//------------------------------------------------------------------------------
void TNetXNGFile::Close( const Option_t */*option*/ )
{
  fFile->Close();
}

//------------------------------------------------------------------------------
//! Reopen the file with the new access mode
//------------------------------------------------------------------------------
Int_t TNetXNGFile::ReOpen( Option_t *modestr )
{
  Info( "TNetXNGFile", "ReOpen" );
  using namespace XrdCl;
  OpenFlags::Flags mode = ParseOpenMode( modestr );

  //----------------------------------------------------------------------------
  // Only Read and Update are valid modes
  //----------------------------------------------------------------------------
  if( mode != OpenFlags::Read && mode != OpenFlags::Update )
  {
    Error("ReOpen", "mode must be either READ or UPDATE, not %s", modestr);
    return 1;
  }

  //----------------------------------------------------------------------------
  // The mode is not really changing
  //----------------------------------------------------------------------------
  if( mode == fMode
      || ( mode == OpenFlags::Update && fMode == OpenFlags::New ) )
  {
    return 1;
  }

  fFile->Close();
  fMode = mode;

  XRootDStatus st = fFile->Open( fUrl->GetURL(), fMode );
  if( !st.IsOK() )
  {
    Error( "ReOpen", "%s", st.GetErrorMessage().c_str() );
    return 1;
  }

  return 0;
}

//------------------------------------------------------------------------------
//! Read a data chunk of the given size
//------------------------------------------------------------------------------
Bool_t TNetXNGFile::ReadBuffer( char *buffer, Int_t length )
{
  // TODO: should we read from fOffset of from 0?
  return ReadBuffer( buffer, fOffset, length );
}

//------------------------------------------------------------------------------
//! Read a data chunk of the given size, starting from the given offset
//------------------------------------------------------------------------------
Bool_t TNetXNGFile::ReadBuffer( char *buffer, Long64_t position, Int_t length )
{
  using namespace XrdCl;
  Info( "ReadBuffer", "offset: %lld length: %d", position, length );

  //----------------------------------------------------------------------------
  // Check the file isn't a zombie
  //----------------------------------------------------------------------------
  if( IsZombie() )
  {
    Error( "ReadBuffer", "ReadBuffer is not possible because object"
                         " is in 'zombie' state" );
    return kTRUE;
  }

  //----------------------------------------------------------------------------
  // Check the file is actually open
  //----------------------------------------------------------------------------
  if( !IsOpen() )
  {
    Error( "ReadBuffer", "The remote file is not open" );
    return kTRUE;
  }

  //----------------------------------------------------------------------------
  // Read the data
  //----------------------------------------------------------------------------
  uint32_t bytesRead = 0;
  XRootDStatus st = fFile->Read( position, length, buffer, bytesRead );
  Info( "ReadBuffer", "%s bytes read: %d", st.ToStr().c_str(), bytesRead );

  if( !st.IsOK() )
  {
    Error( "ReadBuffer", "%s", st.GetErrorMessage().c_str() );
    return kTRUE;
  }

  //----------------------------------------------------------------------------
  // Bump the globals
  //----------------------------------------------------------------------------
  fOffset     += length;
  fBytesRead  += bytesRead;
  fgBytesRead += bytesRead;
  fReadCalls  ++;
  fgReadCalls ++;

  return kFALSE;
}

//------------------------------------------------------------------------------
//! Read scattered data chunks in one operation
//------------------------------------------------------------------------------
Bool_t TNetXNGFile::ReadBuffers( char     *buffer,
                                 Long64_t *position,
                                 Int_t    *length,
                                 Int_t     nbuffs )
{
  using namespace XrdCl;
  Info( "ReadBuffers", "nbuffs: %d", nbuffs );

  //----------------------------------------------------------------------------
  // Check the file isn't a zombie
  //----------------------------------------------------------------------------
  if( IsZombie() )
  {
    Error( "ReadBuffers", "ReadBuffers is not possible because object"
                          " is in 'zombie' state" );
    return kTRUE;
  }

  //----------------------------------------------------------------------------
  // Check the file is actually open
  //----------------------------------------------------------------------------
  if( !IsOpen() )
  {
    Error( "ReadBuffers", "The remote file is not open" );
    return kTRUE;
  }

  //----------------------------------------------------------------------------
  // Build a list of chunks
  //----------------------------------------------------------------------------
  ChunkList chunks;
  for( int i = 0; i < nbuffs; ++i )
  {
    chunks.push_back( ChunkInfo( position[i], length[i] ) );
  }

  //----------------------------------------------------------------------------
  // Read the data
  //----------------------------------------------------------------------------
  VectorReadInfo *info = 0;
  XRootDStatus st      = fFile->VectorRead( chunks, (void *) buffer, info );
  uint32_t size        = info->GetSize();
  delete info;

  if( !st.IsOK() )
  {
    Error( "ReadBuffers", "lol %s", st.GetErrorMessage().c_str() );
    return kTRUE;
  }

  //----------------------------------------------------------------------------
  // Bump the globals
  //----------------------------------------------------------------------------
  fBytesRead  += size;
  fgBytesRead += size;
  fReadCalls  ++;
  fgReadCalls ++;

  return kFALSE;
}

//------------------------------------------------------------------------------
//! Write a data chunk
//------------------------------------------------------------------------------
Bool_t TNetXNGFile::WriteBuffer( const char *buffer, Int_t length )
{
  using namespace XrdCl;

  //----------------------------------------------------------------------------
  // Check the file isn't a zombie
  //----------------------------------------------------------------------------
  if( IsZombie() )
  {
    Error( "WriteBuffer", "WriteBuffer is not possible because object"
                          " is in 'zombie' state" );
    return kTRUE;
  }

  //----------------------------------------------------------------------------
  // Check the file is writable
  //----------------------------------------------------------------------------
  if( !fWritable )
  {
    if( gDebug > 1 )
      Info( "WriteBuffer", "file not writable" );
    return kTRUE;
  }

  //----------------------------------------------------------------------------
  // Check the file is actually open
  //----------------------------------------------------------------------------
  if( !IsOpen() )
  {
    Error( "WriteBuffer", "The remote file is not open" );
    return kTRUE;
  }

  //----------------------------------------------------------------------------
  // Write the data
  //
  // TODO: The old client writes to some ROOT cache here also. Do we need to?
  //----------------------------------------------------------------------------
  XRootDStatus st = fFile->Write( 0, length, buffer );
  if( !st.IsOK() )
  {
    Error( "WriteBuffer", "%s", st.GetErrorMessage().c_str() );
    return kTRUE;
  }

  //----------------------------------------------------------------------------
  // Bump the globals
  //----------------------------------------------------------------------------
  fOffset      += length;
  fBytesWrite  += length;
  fgBytesWrite += length;

  return kFALSE;
}

//------------------------------------------------------------------------------
//! Set the position within the file
//------------------------------------------------------------------------------
void TNetXNGFile::Seek( Long64_t offset, ERelativeTo position )
{
  SetOffset( offset, position );
}


