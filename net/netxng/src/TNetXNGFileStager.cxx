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

TNetXNGFileStager::TNetXNGFileStager( const char */*url*/ ): TFileStager( "xrd" )
{
}

TNetXNGFileStager::~TNetXNGFileStager()
{
}

Bool_t TNetXNGFileStager::IsStaged( const char */*path*/ )
{
  return kFALSE;
}

Bool_t TNetXNGFileStager::Stage( TCollection */*paths*/, Option_t */*opt*/ )
{
  return kFALSE;
}

Bool_t TNetXNGFileStager::Stage( const char */*path*/, Option_t */*opt*/ )
{
  return kFALSE;
}

void TNetXNGFileStager::Print( Option_t * ) const
{
}

Int_t TNetXNGFileStager::Locate( const char */*path*/, TString &/*eurl*/ )
{
   return -1;
}

Int_t TNetXNGFileStager::LocateCollection( TFileCollection */*fc*/,
                                           Bool_t           /*addDummyUrl*/ )
{
  return 0;
}

Bool_t TNetXNGFileStager::Matches( const char */*s*/ )
{
  return kFALSE;
}
