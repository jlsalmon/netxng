//------------------------------------------------------------------------------
// Copyright (c) 2013 by European Organization for Nuclear Research (CERN)
// Author: Lukasz Janyst <ljanyst@cern.ch>
//------------------------------------------------------------------------------

#include "TNetXNGFile.h"
#include <XrdCl/XrdClURL.hh>
#include <XrdCl/XrdClFile.hh>
#include <iostream>

ClassImp(TNetXNGFile);

//------------------------------------------------------------------------------
//! Constructor
//------------------------------------------------------------------------------
TNetXNGFile::TNetXNGFile( const char  *url,
                          Option_t    */*mode*/,
                          const char  *title,
                          Int_t        compress,
                          Int_t        /*netopt*/,
                          Bool_t       /*parallelopen*/):
  TFile( url, "NET", title, compress )
{
  using namespace XrdCl;
  std::cout << "Creating TNetXNGFile" << std::endl;
  fFile = new XrdCl::File();
  XrdCl::URL fileUrl( url );
  fileUrl.SetProtocol( "root" );
  fFile->Open( fileUrl.GetURL().c_str(), OpenFlags::Read );
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
}

//------------------------------------------------------------------------------
//! Get the file size
//------------------------------------------------------------------------------
Long64_t TNetXNGFile::GetSize() const
{
  using namespace XrdCl;
  XrdCl::StatInfo *info = 0;
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
void TNetXNGFile::Close( const Option_t */*option*/)
{
  fFile->Close();
}

//------------------------------------------------------------------------------
//! Reopen the file with the new access mode
//------------------------------------------------------------------------------
Int_t TNetXNGFile::ReOpen(Option_t */*mode*/)
{
  return 0;
}

//------------------------------------------------------------------------------
//! Read a data chunk of the given size
//------------------------------------------------------------------------------
Bool_t TNetXNGFile::ReadBuffer( char *buffer, Int_t length )
{
  using namespace XrdCl;
  uint32_t bytesRead = 0;
  std::cout << "rb 1 o " << fOffset << " l " << length << std::endl;
  XRootDStatus st = fFile->Read( fOffset, length, buffer, bytesRead );
  std::cout << "[x]" << st.ToStr() << " " << bytesRead << std::endl;
  fOffset += bytesRead;
  return false;
}

//------------------------------------------------------------------------------
//! Read a data chunk of the given size, starting from the given offset
//------------------------------------------------------------------------------
Bool_t TNetXNGFile::ReadBuffer( char *buffer, Long64_t position, Int_t length )
{
  using namespace XrdCl;
  std::cout << "rb 2" << std::endl;
  uint32_t bytesRead = 0;
  XRootDStatus st = fFile->Read( position, length, buffer, bytesRead );
  return true;
}

//------------------------------------------------------------------------------
//! Read scattered data chunks in one operation
//------------------------------------------------------------------------------
Bool_t TNetXNGFile::ReadBuffers( char     *buffer,
                                 Long64_t *position,
                                 Int_t    *length,
                                 Int_t     nbuffs )
{
  std::cout << "rb3 " << std::endl;
  return true;
}

//------------------------------------------------------------------------------
//! Write a data chunk
//------------------------------------------------------------------------------
Bool_t TNetXNGFile::WriteBuffer(const char *buffer, Int_t length)
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


