//------------------------------------------------------------------------------
// Copyright (c) 2013 by European Organization for Nuclear Research (CERN)
// Author: Lukasz Janyst <ljanyst@cern.ch>
//------------------------------------------------------------------------------

#include "TNetXNGFileStager.h"
#include "TNetXNGSystem.h"
#include <XrdCl/XrdClFileSystem.hh>

ClassImp( TNetXNGFileStager );

//------------------------------------------------------------------------------
//! Constructor
//------------------------------------------------------------------------------
TNetXNGFileStager::TNetXNGFileStager( const char *url ):
  TFileStager( "xrd" )
{
  Info( "TNetXNGFileStager", "Creating TNetXNGFileStager" );
  fSystem = new TNetXNGSystem( url );
}

//------------------------------------------------------------------------------
//! Destructor
//------------------------------------------------------------------------------
TNetXNGFileStager::~TNetXNGFileStager()
{
  delete fSystem;
}

//------------------------------------------------------------------------------
//! Check if a file is ready to be used
//------------------------------------------------------------------------------
Bool_t TNetXNGFileStager::IsStaged( const char *path )
{
  FileStat_t st;
  if( fSystem->GetPathInfo( path, st ) != 0 )
  {
    if ( gDebug > 0 )
      Info( "IsStaged", "path %s cannot be stat'ed", path );
    return kFALSE;
  }

  if ( R_ISOFF( st.fMode ) )
  {
    if ( gDebug > 0 )
      Info( "IsStaged", "path '%s' is offline", path );
    return kFALSE;
  }

  return kTRUE;
}

//------------------------------------------------------------------------------
//! Get actual endpoint URL
//------------------------------------------------------------------------------
Int_t TNetXNGFileStager::Locate( const char *path, TString &url )
{
  Info( "TNetXNGFileStager", "Locate" );
  using namespace XrdCl;
  LocationInfo *info = 0;
  URL pathUrl( path );
  FileSystem fs( pathUrl );

  XRootDStatus st = fs.Locate( pathUrl.GetURL(), OpenFlags::None, info );

  if( !st.IsOK() )
  {
    Error( "Locate", "%s", st.GetErrorMessage().c_str() );
    return 1;
  }

  Info("Locate", "lol");
  LocationInfo::Iterator it;
  for( it = info->Begin(); it != info->End(); ++it )
  {
    Info( "Locate", "Location: %s", *it->GetAddress().c_str() );
  }

  return 0;
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
