//------------------------------------------------------------------------------
// Copyright (c) 2013 by European Organization for Nuclear Research (CERN)
// Author: Lukasz Janyst <ljanyst@cern.ch>
// Author: Justin Salmon <jsalmon@cern.ch>
//------------------------------------------------------------------------------

#include "TNetXNGFileStager.h"
#include "TNetXNGSystem.h"
#include "THashList.h"
#include "TFileInfo.h"
#include "TFileCollection.h"
#include <XrdCl/XrdClFileSystem.hh>

ClassImp( TNetXNGFileStager );

//------------------------------------------------------------------------------
//! Constructor
//------------------------------------------------------------------------------
TNetXNGFileStager::TNetXNGFileStager( const char *url ):
  TFileStager( "xrd" )
{
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
  return fSystem->Locate( path, url );
}

//------------------------------------------------------------------------------
//! Bulk locate request for a collection of files
//------------------------------------------------------------------------------
Int_t TNetXNGFileStager::LocateCollection( TFileCollection *fc,
                                           Bool_t addDummyUrl )
{
  if( !fc )
  {
    Error( "Locate", "No input collection given!" );
    return -1;
  }

  int        numFiles = 0;
  TFileInfo *info;
  TIter      it( fc->GetList() );
  TString    startUrl, endUrl;

  while( ( info = dynamic_cast<TFileInfo *>( it.Next() ) ) != NULL )
  {
    startUrl = info->GetCurrentUrl()->GetUrl();

    //--------------------------------------------------------------------------
    // File not staged
    //--------------------------------------------------------------------------
    if( fSystem->Locate( startUrl.Data(), endUrl ) )
    {
      info->ResetBit( TFileInfo::kStaged );

      if( addDummyUrl ) info->AddUrl( "noop://none", kTRUE );

      if( gDebug > 1 )
        Info( "LocateCollection", "Not found: %s", startUrl.Data() );
    }

    //--------------------------------------------------------------------------
    // File staged
    //--------------------------------------------------------------------------
    else
    {
      info->SetBit( TFileInfo::kStaged );
      if( startUrl != endUrl )
      {
        info->AddUrl( endUrl.Data(), kTRUE );
      }
      else if( addDummyUrl )
      {
        //----------------------------------------------------------------------
        // Returned URL identical to redirector URL
        //----------------------------------------------------------------------
        info->AddUrl( "noop://redir", kTRUE );
      }

      if( gDebug > 1 )
        Info( "LocateCollection", "Found: %s --> %s",
                                  startUrl.Data(), endUrl.Data() );
    }

    numFiles++;
  }

  return numFiles;
}

//------------------------------------------------------------------------------
//! Returns kTRUE if stager 's' is compatible with current stager
//------------------------------------------------------------------------------
Bool_t TNetXNGFileStager::Matches( const char *s )
{
  return ( ( s && ( fName == s ) ) ? kTRUE : kFALSE );
}

//------------------------------------------------------------------------------
//! Issue a stage request for a single file
//------------------------------------------------------------------------------
Bool_t TNetXNGFileStager::Stage( const char *path, Option_t *opt )
{
  Int_t priority = ParseStagePriority( opt );
  return fSystem->Stage( path, priority );
}

//------------------------------------------------------------------------------
//! Issue stage requests for multiple files
//------------------------------------------------------------------------------
Bool_t TNetXNGFileStager::Stage( TCollection *paths, Option_t *opt )
{
  Int_t priority = ParseStagePriority( opt );
  return fSystem->Stage( paths, priority );
}

//--------------------------------------------------------------------------
//! Get a staging priority value from an option string
//--------------------------------------------------------------------------
UChar_t TNetXNGFileStager::ParseStagePriority( Option_t *opt )
{
  UChar_t priority = 0;
  Ssiz_t from = 0;
  TString token;

  while( TString( opt ).Tokenize( token, from, "[ ,|]" ) )
  {
    if( token.Contains( "priority=" ) )
    {
      token.ReplaceAll( "priority=", "" );
      if( token.IsDigit() )
      {
        priority = token.Atoi();
      }
    }
  }

  return priority;
}
