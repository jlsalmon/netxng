//------------------------------------------------------------------------------
// Copyright (c) 2013 by European Organization for Nuclear Research (CERN)
// Author: Justin Salmon <jsalmon@cern.ch>
//------------------------------------------------------------------------------

void FileTest()
{
  std::cout << "Beginning FileTest" << std::endl;
  gDebug = 3;

  // Open()
  TFile *f = TFile::Open( "root://localhost//tmp/Event.root" );

  // IsOpen()
  std::cout << "IsOpen: " << ( f->IsOpen() ? "true" : "false" ) << std::endl;

  // GetSize()
  Long64_t size = f->GetSize();
  std::cout << "File size: " << size << std::endl;

  // ReadBuffer(3)
  char *buf = new char[1024];
  std::cout << "ReadBuffer(3)" << std::endl;
  if( f->ReadBuffer( buf, 0, 1024 ) )
  {
    std::cout << "Error reading" << std::endl;
  }
  delete buf;

  // Seek()
  std::cout << "Seek()" << std::endl;
  f->Seek( 1024, TFile::ERelativeTo::kBeg );

  // ReadBuffer(2)
  buf = new char[1024];
  std::cout << "ReadBuffer(2)" << std::endl;
  if( f->ReadBuffer( buf, 1024 ) )
  {
    std::cout << "Error reading" << std::endl;
  }
  delete buf;

  // ReadBuffers()
  std::cout << "ReadBuffers()" << std::endl;

  buf = new char[4096];
  Long64_t pos[4] = {0, 1024, 2048, 3072};
  Int_t    len[4] = {1024, 1024, 1024, 1024};
  Int_t    nbuf   = 4;

  if( f->ReadBuffers(buf, pos, len, nbuf) )
  {
    std::cout << "Error reading" << std::endl;
  }
  delete buf;

  // ReOpen()
  if( f->ReOpen( "update" ) >= 0 )
  {
    std::cout << "File reopened in write mode" << std::endl;
  }

  // WriteBuffer()
  buf = "write me";
  f->Seek( 0, TFile::ERelativeTo::kEnd );

  if( f->WriteBuffer( buf, 8 ) )
  {
    std::cout << "Error writing" << std::endl;
  }

  f->Close();
}
