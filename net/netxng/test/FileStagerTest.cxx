//------------------------------------------------------------------------------
// Copyright (c) 2013 by European Organization for Nuclear Research (CERN)
// Author: Justin Salmon <jsalmon@cern.ch>
//------------------------------------------------------------------------------

void FileStagerTest()
{
  std::cout << "Beginning FileStagerTest" << std::endl;

  TFileStager s( "root://localhost" );

  // IsStaged()
  Bool_t isStaged = s.IsStaged( "root://localhost//tmp/Event.root" );
  cout << (isStaged ? "true\n" : "false\n");

  // Locate()
  TString endpoint = 0;
  s.Locate( "root://localhost//tmp/Event.root", endpoint );

  // LocateCollection()

  // Matches()

  // Stage() single

  // Stage() multiple

}

