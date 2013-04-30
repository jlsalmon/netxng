//------------------------------------------------------------------------------
// Copyright (c) 2013 by European Organization for Nuclear Research (CERN)
// Author: Lukasz Janyst <ljanyst@cern.ch>
//------------------------------------------------------------------------------

#include "TNetXNGSystem.h"
#include "Rtypes.h"
#include <XrdCl/XrdClFileSystem.hh>

ClassImp( TNetXNGSystem );

//------------------------------------------------------------------------------
//! Constructor - Create system class without connecting to server
//------------------------------------------------------------------------------
TNetXNGSystem::TNetXNGSystem( Bool_t /*owner*/ ):
  TSystem( "-root", "Net file Helper System" ), fFileSystem( 0 )
{
  using namespace XrdCl;
}

//------------------------------------------------------------------------------
//! Constructor - Create system class and connect to server
//------------------------------------------------------------------------------
TNetXNGSystem::TNetXNGSystem( const char *url, Bool_t /*owner*/ ):
  TSystem( "-root", "Net file Helper System" )
{
  using namespace XrdCl;
  XrdCl::URL fileUrl( url );
  fFileSystem = new XrdCl::FileSystem( fileUrl );
}

//------------------------------------------------------------------------------
//! Open a directory
//------------------------------------------------------------------------------
void *TNetXNGSystem::OpenDirectory( const char */*dir*/ )
{
  return 0;
}

//------------------------------------------------------------------------------
//! Create a directory
//------------------------------------------------------------------------------
Int_t TNetXNGSystem::MakeDirectory( const char */*dir*/ )
{
  return -1;
}

//------------------------------------------------------------------------------
//! Free a directory
//------------------------------------------------------------------------------
void TNetXNGSystem::FreeDirectory( void */*dirp*/ )
{
}

//------------------------------------------------------------------------------
//! Get a directory entry.
//------------------------------------------------------------------------------
const char *TNetXNGSystem::GetDirEntry( void */*dirp*/ )
{
  return 0;
}

//------------------------------------------------------------------------------
//! Get info about a file (stat)
//------------------------------------------------------------------------------
Int_t TNetXNGSystem::GetPathInfo( const char */*path*/, FileStat_t &/*buf*/ )
{
  return -1;
}

//------------------------------------------------------------------------------
//! Check consistency of this helper with the one required by 'path' or 'dirptr'
//------------------------------------------------------------------------------
Bool_t TNetXNGSystem::ConsistentWith( const char */*path*/, void */*dirptr*/ )
{
  return kFALSE;
}

//------------------------------------------------------------------------------
//! Unlink a file on the remote server
//------------------------------------------------------------------------------
int TNetXNGSystem::Unlink( const char */*path*/ )
{
  return -1;
}

//------------------------------------------------------------------------------
//! Is this path a local path?
//------------------------------------------------------------------------------
Bool_t TNetXNGSystem::IsPathLocal( const char */*path*/ )
{
  return kFALSE;
}

//------------------------------------------------------------------------------
//! Get the endpoint URL of a file.
//------------------------------------------------------------------------------
Int_t TNetXNGSystem::Locate( const char */*path*/, TString &/*eurl*/ )
{
  return -1;
}
