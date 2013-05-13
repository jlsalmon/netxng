//------------------------------------------------------------------------------
// Copyright (c) 2013 by European Organization for Nuclear Research (CERN)
// Author: Justin Salmon <jsalmon@cern.ch>
//------------------------------------------------------------------------------

void SystemTest()
{
  std::cout << "Beginning SystemTest" << std::endl;

  // OpenDirectory()
  void *dir = gSystem->OpenDirectory( "root://localhost//tmp" );

  // GetPathInfo()
  FileStat_t st;
  gSystem->GetPathInfo( "root://localhost//tmp/Event.root", st );
  std::cout << st.fSize << std::endl;

  // MakeDirectory()
  if( !gSystem->MakeDirectory( "root://localhost//tmp/somedir" ) == 0 )
  {
    std::cout << "Error making directory" << std::endl;
  }

  // FreeDirectory()

  // GetDirEntry()
  const char *filename;
  // ! This should call plugin impl, but doesn't
  filename = gSystem->GetDirEntry( dir );

  while( ( filename = gSystem->GetDirEntry( dir ) ) != 0 )
  {
    std::cout << "Filename: " << filename << std::endl;
  }

  // CosistentWith()

  // Unlink()


  // IsPathLocal()

  // Locate()
}
