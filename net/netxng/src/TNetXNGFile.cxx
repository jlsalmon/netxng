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
// Constructor
//------------------------------------------------------------------------------
TNetXNGFile::TNetXNGFile( const char  *url,
                          Option_t    */*option*/,
                          const char  *title,
                          Int_t        compress,
                          Int_t        /*netopt*/ ):
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
// Desctructor
//------------------------------------------------------------------------------
TNetXNGFile::~TNetXNGFile()
{
  if( IsOpen() )
    Close();
  delete fFile;
}

Long64_t TNetXNGFile::GetSize() const
{
  using namespace XrdCl;
  XrdCl::StatInfo *info = 0;
  fFile->Stat( false, info );
  Long64_t size = info->GetSize();
  delete info;
//  std::cout << "size: " << size << std::endl;
  return size;
}

Bool_t TNetXNGFile::IsOpen() const
{
  return fFile->IsOpen();
}

Bool_t TNetXNGFile::ReadBuffer( char *buf, Int_t len )
{
  using namespace XrdCl;
  uint32_t bytesRead = 0;
//  std::cout << "rb 1 o " << fOffset << " l " << len << std::endl;
  XRootDStatus st = fFile->Read( fOffset, len, buf, bytesRead );
//  std::cout << "[x]" << st.ToStr() << " " << bytesRead << std::endl;
  fOffset += bytesRead;
  return false;
}

Bool_t TNetXNGFile::ReadBuffer( char *buf, Long64_t pos, Int_t len )
{
  using namespace XrdCl;
//  std::cout << "rb 2" << std::endl;
  uint32_t bytesRead = 0;
  XRootDStatus st = fFile->Read( pos, len, buf, bytesRead );
  return true;
}

Bool_t TNetXNGFile::ReadBuffers(char *buf,  Long64_t *pos, Int_t *len, Int_t nbuf)
{
//  std::cout << "rb3 " << std::endl;
  return true;
}

void TNetXNGFile::Seek(Long64_t offset, ERelativeTo pos)
{
  SetOffset(offset, pos);
}

void TNetXNGFile::Close(const Option_t */*opt*/)
{
  fFile->Close();
}
