//------------------------------------------------------------------------------
// Copyright (c) 2013 by European Organization for Nuclear Research (CERN)
// Author: Lukasz Janyst <ljanyst@cern.ch>
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

XrdCl::OpenFlags::Flags TNetXNGFile::ParseOpenMode( Option_t *modestr )
{
  using namespace XrdCl;
  OpenFlags::Flags mode = OpenFlags::None;
  std::string mod = std::string( modestr );

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
}

//------------------------------------------------------------------------------
//! Read a data chunk of the given size
//------------------------------------------------------------------------------
Bool_t TNetXNGFile::ReadBuffer( char *buffer, Int_t length )
{
  using namespace XrdCl;
  uint32_t bytesRead = 0;
  Info( "ReadBuffer", "offset: %lld length: %d", fOffset, length );
  XRootDStatus st = fFile->Read( fOffset, length, buffer, bytesRead );
  Info( "ReadBuffer", "%s bytes read: %d", st.ToStr().c_str(), bytesRead );
  fOffset += bytesRead;
  return st.IsError();
}

//------------------------------------------------------------------------------
//! Read a data chunk of the given size, starting from the given offset
//------------------------------------------------------------------------------
Bool_t TNetXNGFile::ReadBuffer( char *buffer, Long64_t position, Int_t length )
{
  using namespace XrdCl;
  Info( "ReadBuffer", "offset: %lld length: %d", position, length );
  uint32_t bytesRead = 0;
  XRootDStatus st = fFile->Read( position, length, buffer, bytesRead );
  Info( "ReadBuffer", "%s bytes read: %d", st.ToStr().c_str(), bytesRead );
  return st.IsError();
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
  ChunkList chunks;

  for( int i = 0; i < nbuffs; ++i )
  {
    chunks.push_back( ChunkInfo( position[i], length[i] ) );
  }

  VectorReadInfo *vi;
  XRootDStatus st = fFile->VectorRead( chunks, (void *) buffer, vi );
  Info( "ReadBuffer", "%s", st.ToStr().c_str() );

  delete vi;
  return true;
}

//------------------------------------------------------------------------------
//! Write a data chunk
//------------------------------------------------------------------------------
Bool_t TNetXNGFile::WriteBuffer( const char */*buffer*/, Int_t /*length*/ )
{
  return false;
}

//------------------------------------------------------------------------------
//! Set the position within the file
//------------------------------------------------------------------------------
void TNetXNGFile::Seek( Long64_t offset, ERelativeTo position )
{
  SetOffset( offset, position );
}


