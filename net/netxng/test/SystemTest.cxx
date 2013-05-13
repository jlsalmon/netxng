//------------------------------------------------------------------------------
// Copyright (c) 2013 by European Organization for Nuclear Research (CERN)
// Author: Justin Salmon <jsalmon@cern.ch>
//------------------------------------------------------------------------------

void SystemTest()
{
  std::cout << "Beginning SystemTest" << std::endl;

  // OpenDirectory()
  TSystem *system = new TNetXNGSystem( "root://localhost" );
  void *dir = system->OpenDirectory( "root://localhost//tmp" );

  // GetPathInfo()
  FileStat_t st;
  system->GetPathInfo( "root://localhost//tmp/Event.root", st );
  std::cout << st.fSize << std::endl;

  // MakeDirectory()
  if( !system->MakeDirectory( "root://localhost//tmp/testdir" ) == 0 )
  {
    std::cout << "Error making directory" << std::endl;
  }

  // Unlink()
  if( !system->Unlink( "root://localhost//tmp/testdir" ) == 0 )
  {
    std::cout << "Error removing directory" << std::endl;
  }

  // GetDirEntry()
  const char *filename;
  while( ( filename = system->GetDirEntry( dir ) ) != 0 )
  {
    std::cout << "Filename: " << filename << std::endl;
  }

  // FreeDirectory()
  system->FreeDirectory( dir );

}
