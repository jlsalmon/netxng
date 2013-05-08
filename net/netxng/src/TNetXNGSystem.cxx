//------------------------------------------------------------------------------
// Copyright (c) 2013 by European Organization for Nuclear Research (CERN)
// Author: Lukasz Janyst <ljanyst@cern.ch>
//------------------------------------------------------------------------------

#include "TNetXNGSystem.h"
#include "Rtypes.h"
#include <XrdCl/XrdClFileSystem.hh>
#include <XrdCl/XrdClXRootDResponses.hh>

ClassImp( TNetXNGSystem );

//------------------------------------------------------------------------------
//! Constructor - Create system class without connecting to server
//------------------------------------------------------------------------------
TNetXNGSystem::TNetXNGSystem( Bool_t /*owner*/ ):
  TSystem( "-root", "Net file Helper System" ), fFileSystem( 0 )
{
  Info( "TNetXNGSystem", "Creating TNetXNGSystem" );
}

//------------------------------------------------------------------------------
//! Constructor - Create system class and connect to server
//------------------------------------------------------------------------------
TNetXNGSystem::TNetXNGSystem( const char *url, Bool_t /*owner*/ ):
  TSystem( "-root", "Net file Helper System" )
{
  using namespace XrdCl;
  Info( "TNetXNGSystem", "Creating TNetXNGSystem" );
  URL fileUrl( url );
  fFileSystem = new FileSystem( fileUrl );
}

//------------------------------------------------------------------------------
//! Open a directory
//------------------------------------------------------------------------------
void *TNetXNGSystem::OpenDirectory( const char */*dir*/ )
{
  Info( "TNetXNGSystem", "OpenDirectory()" );
  return (void *) 1;
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
Int_t TNetXNGSystem::GetPathInfo( const char *path, FileStat_t &buf )
{
  Info( "TNetXNGSystem", "GetPathInfo()" );
  using namespace XrdCl;
  StatInfo *info = 0;
  URL target( path );
  XRootDStatus st = fFileSystem->Stat( target.GetPath(), info );

  if( !st.IsOK() )
  {
    Error( "GetPathInfo", "%s", st.GetErrorMessage().c_str() );
    delete info;
    return 1;
  }
  else
  {
    //--------------------------------------------------------------------------
    // Flag offline files
    //--------------------------------------------------------------------------
    if ( info->GetFlags() & kXR_offline )
    {
      buf.fMode = kS_IFOFF;
    }
    else
    {
      std::stringstream sstr( info->GetId() );
      Long64_t id;
      sstr >> id;

      buf.fDev    = ( id >> 32 );
      buf.fIno    = ( id & 0x00000000FFFFFFFF );
      buf.fUid    = -1;  // not available
      buf.fGid    = -1;  // not available
      buf.fIsLink = 0;   // not available
      buf.fSize   = info->GetSize();
      buf.fMtime  = info->GetModTime();

      if ( info->GetFlags() & kXR_xset )
        buf.fMode = ( kS_IFREG | kS_IXUSR | kS_IXGRP | kS_IXOTH );
      if ( info->GetFlags() == 0 )           buf.fMode  = kS_IFREG;
      if ( info->GetFlags() & kXR_isDir )    buf.fMode  = kS_IFDIR;
      if ( info->GetFlags() & kXR_other )    buf.fMode  = kS_IFSOCK;
      if ( info->GetFlags() & kXR_readable ) buf.fMode |= kS_IRUSR;
      if ( info->GetFlags() & kXR_writable ) buf.fMode |= kS_IWUSR;
    }
  }

  delete info;
  return 0;
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
