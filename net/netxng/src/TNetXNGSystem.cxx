//------------------------------------------------------------------------------
// Copyright (c) 2013 by European Organization for Nuclear Research (CERN)
// Author: Lukasz Janyst <ljanyst@cern.ch>
// Author: Justin Salmon <jsalmon@cern.ch>
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
  TSystem( "-root", "Net file Helper System" ),
  fFileSystem( 0 ), fUrl( 0 ), fDirList( 0 ), fDirListIter( 0 )
{
  Info( "TNetXNGSystem", "Creating TNetXNGSystem" );
}

//------------------------------------------------------------------------------
//! Constructor - Create system class and connect to server
//------------------------------------------------------------------------------
TNetXNGSystem::TNetXNGSystem( const char *url, Bool_t /*owner*/ ):
  TSystem( "-root", "Net file Helper System" ),
  fUrl( 0 ), fDirList(0), fDirListIter( 0 )
{
  using namespace XrdCl;
  Info( "TNetXNGSystem", "Creating TNetXNGSystem" );
  fUrl        = new URL( std::string( url ) );
  fFileSystem = new FileSystem( fUrl->GetURL() );
}

//------------------------------------------------------------------------------
//! Destructor
//------------------------------------------------------------------------------
TNetXNGSystem::~TNetXNGSystem()
{
  delete fFileSystem;
  delete fUrl;
  delete fDirList;
}

//------------------------------------------------------------------------------
//! Open a directory
//------------------------------------------------------------------------------
void* TNetXNGSystem::OpenDirectory( const char *dir )
{
  using namespace XrdCl;
  Info( "TNetXNGSystem", "OpenDirectory()" );
  delete fFileSystem;
  delete fUrl;
  delete fDirList;

  fUrl        = new URL( std::string( dir ) );
  fFileSystem = new FileSystem( fUrl->GetURL() );
  return (void *) fUrl;
}

//------------------------------------------------------------------------------
//! Create a directory
//------------------------------------------------------------------------------
Int_t TNetXNGSystem::MakeDirectory( const char *dir )
{
  using namespace XrdCl;
  URL url( dir );
  XRootDStatus st = fFileSystem->MkDir( url.GetPath(),
                                        MkDirFlags::MakePath,
                                        Access::None );

  if( !st.IsOK() )
  {
    Error( "MakeDirectory", "%s", st.GetErrorMessage().c_str() );
    return -1;
  }

  return 0;
}

//------------------------------------------------------------------------------
//! Free a directory
//------------------------------------------------------------------------------
void TNetXNGSystem::FreeDirectory( void *dirp )
{
  using namespace XrdCl;

  if( (URL *) dirp != fUrl )
  {
    Error( "FreeDirectory", "invalid directory pointer" );
    return;
  }


}

//------------------------------------------------------------------------------
//! Get a directory entry.
//------------------------------------------------------------------------------
const char* TNetXNGSystem::GetDirEntry( void *dirp )
{
  using namespace XrdCl;
  Info( "TNetXNGSystem", "GetDirEntry()" );
  URL *url = (URL *) dirp;

  if ( url != fUrl ) {
     Error( "GetDirEntry","invalid directory pointer" );
     return 0;
  }

  if( !fDirList )
  {
    XRootDStatus st = fFileSystem->DirList( fUrl->GetPath(),
                                            DirListFlags::Locate,
                                            fDirList );
    if( !st.IsOK() )
    {
      Error( "GetDirEntry","%s", st.GetErrorMessage().c_str() );
      return 0;
    }

    fDirListIter = fDirList->Begin();
  }

  if( fDirListIter != fDirList->End() )
  {
    const char *filename = (*fDirListIter)->GetName().c_str();
    ++fDirListIter;
    return filename;
  }
  else return 0;
}

//------------------------------------------------------------------------------
//! Get info about a file (stat)
//------------------------------------------------------------------------------
Int_t TNetXNGSystem::GetPathInfo( const char *path, FileStat_t &buf )
{
  using namespace XrdCl;
  Info( "TNetXNGSystem", "GetPathInfo()" );
  StatInfo *info = 0;
  URL target( path );
  Info("GetPathInfo", "%s", target.GetHostId().c_str());
  XRootDStatus st = fFileSystem->Stat( target.GetPath(), info );

  if( !st.IsOK() )
  {
    Error( "GetPathInfo", "Error: %s", st.GetErrorMessage().c_str() );
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
int TNetXNGSystem::Unlink( const char *path )
{
  using namespace XrdCl;
  XRootDStatus st = fFileSystem->Rm( std::string( path ) );

  if( !st.IsOK() )
  {
    Error( "Unlink", "%s", st.GetErrorMessage().c_str() );
    return -1;
  }

  return 0;
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
Int_t TNetXNGSystem::Locate( const char *path, TString &endurl )
{
  using namespace XrdCl;
  LocationInfo *info = 0;
  URL pathUrl( path );

  XRootDStatus st = fFileSystem->Locate( pathUrl.GetURL(), OpenFlags::None, info );
  if( !st.IsOK() )
  {
    Error( "Locate", "%s", st.GetErrorMessage().c_str() );
    return 1;
  }

  endurl = info->Begin()->GetAddress();
  return 0;
}

