//------------------------------------------------------------------------------
// Copyright (c) 2013 by European Organization for Nuclear Research (CERN)
// Author: Lukasz Janyst <ljanyst@cern.ch>
//------------------------------------------------------------------------------

//#include "TError.h"
//#include "TObjString.h"
//#include "TUrl.h"
#include "TNetXNGFileStager.h"
//#include "TXNetSystem.h"
//#include "TFileCollection.h"
//#include "TStopwatch.h"
//#include "TFileInfo.h"
#include <iostream>

ClassImp( TNetXNGFileStager );

//------------------------------------------------------------------------------
//! Constructor
//------------------------------------------------------------------------------
TNetXNGFileStager::TNetXNGFileStager( const char *url ):
  TFileStager( "xrd" ), fSystem( url )
{
  std::cout << "Creating TNetXNGFileStager" << std::endl;
}

//------------------------------------------------------------------------------
//! Destructor
//------------------------------------------------------------------------------
TNetXNGFileStager::~TNetXNGFileStager()
{
}

//------------------------------------------------------------------------------
//! Check if a file is ready to be used
//------------------------------------------------------------------------------
Bool_t TNetXNGFileStager::IsStaged( const char */*path*/ )
{
  return kFALSE;
}

//------------------------------------------------------------------------------
//! Get actual endpoint URL
//------------------------------------------------------------------------------
Int_t TNetXNGFileStager::Locate( const char */*path*/, TString &/*url*/ )
{
  return -1;
}

//------------------------------------------------------------------------------
//! Bulk locate request for a collection of files
//------------------------------------------------------------------------------
Int_t TNetXNGFileStager::LocateCollection( TFileCollection */*fc*/,
                                           Bool_t /*addDummyUrl*/ )
{
  return 0;
}

//------------------------------------------------------------------------------
//! Returns kTRUE if stager 's' is compatible with current stager
//------------------------------------------------------------------------------
Bool_t TNetXNGFileStager::Matches( const char */*s*/ )
{
  return kFALSE;
}

//------------------------------------------------------------------------------
//! Issue a stage request for a single file
//------------------------------------------------------------------------------
Bool_t TNetXNGFileStager::Stage( TCollection */*paths*/, Option_t */*opt*/ )
{
  return kFALSE;
}

//------------------------------------------------------------------------------
//! Issue stage requests for multiple files
//------------------------------------------------------------------------------
Bool_t TNetXNGFileStager::Stage( const char */*path*/, Option_t */*opt*/ )
{
  return kFALSE;
}
