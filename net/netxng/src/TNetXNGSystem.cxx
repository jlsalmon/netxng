//------------------------------------------------------------------------------
// Copyright (c) 2013 by European Organization for Nuclear Research (CERN)
// Author: Lukasz Janyst <ljanyst@cern.ch>
// Author: Justin Salmon <jsalmon@cern.ch>
//------------------------------------------------------------------------------

#include "TNetXNGSystem.h"
#include "TFileStager.h"
#include "Rtypes.h"
#include "TList.h"
#include "TUrl.h"
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
  //----------------------------------------------------------------------------
  // Name must start with '-' to bypass the TSystem singleton check
  //----------------------------------------------------------------------------
  SetName("root");
}

//------------------------------------------------------------------------------
//! Constructor - Create system class and connect to server
//------------------------------------------------------------------------------
TNetXNGSystem::TNetXNGSystem( const char *url, Bool_t /*owner*/ ):
  TSystem( "-root", "Net file Helper System" ),
  fUrl( 0 ), fDirList(0), fDirListIter( 0 )
{
  using namespace XrdCl;

  //----------------------------------------------------------------------------
  // Name must start with '-' to bypass the TSystem singleton check
  //----------------------------------------------------------------------------
  SetName("root");
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
  FreeDirectory( (void *) fUrl );

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
  if( fUrl && (XrdCl::URL *) dirp != fUrl )
  {
    Error( "FreeDirectory", "invalid directory pointer" );
    return;
  }

  delete fFileSystem;
  delete fUrl;
  delete fDirList;
}

//------------------------------------------------------------------------------
//! Get a directory entry.
//------------------------------------------------------------------------------
const char* TNetXNGSystem::GetDirEntry( void *dirp )
{
  using namespace XrdCl;
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
  StatInfo *info = 0;
  URL target( path );
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
Bool_t TNetXNGSystem::ConsistentWith( const char *path, void *dirptr )
{
  using namespace XrdCl;

  //----------------------------------------------------------------------------
  // Standard check: only the protocol part of 'path' is required to match
  //----------------------------------------------------------------------------
  Bool_t checkstd = TSystem::ConsistentWith(path, dirptr);
  if (!checkstd) return kFALSE;

  URL url( path );
  Bool_t checknet = path ? kFALSE : kTRUE;

  if( gDebug > 1 )
    Info( "ConsistentWith", "fUser:'%s' (%s), fHost:'%s' (%s), fPort:%d (%d)",
        fUrl->GetUserName().c_str(), url.GetUserName().c_str(),
        fUrl->GetHostName().c_str(), url.GetHostName().c_str(),
        fUrl->GetPort(), url.GetPort() );

  //----------------------------------------------------------------------------
  // Require match of 'user' and 'host'
  //----------------------------------------------------------------------------
  if( fUrl->GetUserName() == url.GetUserName() &&
      fUrl->GetHostName() == url.GetHostName() &&
      fUrl->GetPort() == url.GetPort() )
    checknet = kTRUE;

  return ( checkstd && checknet );
}

//------------------------------------------------------------------------------
//! Unlink a file on the remote server
//------------------------------------------------------------------------------
int TNetXNGSystem::Unlink( const char *path )
{
  using namespace XrdCl;
  StatInfo *info;
  URL url( path );

  //----------------------------------------------------------------------------
  // Stat the path to find out if it's a file or a directory
  //----------------------------------------------------------------------------
  XRootDStatus st = fFileSystem->Stat( url.GetPath(), info );
  if( !st.IsOK() )
  {
    Error( "Unlink", "%s", st.GetErrorMessage().c_str() );
    delete info;
    return -1;
  }

  if( info->TestFlags( StatInfo::IsDir ) )
    st = fFileSystem->RmDir( url.GetPath() );
  else
    st = fFileSystem->Rm( url.GetPath() );
  delete info;

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
Bool_t TNetXNGSystem::IsPathLocal( const char *path )
{
  using namespace XrdCl;
  ProtocolInfo *info = 0;
  FileSystem fs = FileSystem( URL( path ) );

  //----------------------------------------------------------------------------
  // Grab the protocol info for this server
  //----------------------------------------------------------------------------
  XRootDStatus st = fs.Protocol( info );
  if( !st.IsOK() )
  {
    Error( "IsPathLocal", "%s", st.GetErrorMessage().c_str() );
    delete info;
    return kFALSE;
  }

  //----------------------------------------------------------------------------
  // Cannot assert locality if not an endpoint data server
  //----------------------------------------------------------------------------
  if( !info->TestHostInfo( ProtocolInfo::IsServer ) )
    return kFALSE;

  delete info;
  //----------------------------------------------------------------------------
  // Either an end-point data server or 'rootd': check for locality
  //----------------------------------------------------------------------------
  return TSystem::IsPathLocal(path);
}

//------------------------------------------------------------------------------
//! Get the endpoint URL of a file.
//------------------------------------------------------------------------------
Int_t TNetXNGSystem::Locate( const char *path, TString &endurl )
{
  using namespace XrdCl;
  LocationInfo *info = 0;
  URL pathUrl( path );

  //----------------------------------------------------------------------------
  // Locate the file
  //----------------------------------------------------------------------------
  XRootDStatus st = fFileSystem->Locate( pathUrl.GetPath(),
                                         OpenFlags::None,
                                         info );
  if( !st.IsOK() )
  {
    Error( "Locate", "%s", st.GetErrorMessage().c_str() );
    delete info;
    return 1;
  }

  //----------------------------------------------------------------------------
  // Return the first address
  //----------------------------------------------------------------------------
  endurl = info->Begin()->GetAddress();
  delete info;
  return 0;
}

//------------------------------------------------------------------------------
//! Issue a stage request for a single file
//------------------------------------------------------------------------------
Int_t TNetXNGSystem::Stage( const char* path, UChar_t priority )
{
  TList *files = new TList();
  files->Add( (TObject *) new TUrl( path ) );
  return Stage( (TCollection *) files, priority );
}

//------------------------------------------------------------------------------
//! Issue stage requests for multiple files
//------------------------------------------------------------------------------
Int_t TNetXNGSystem::Stage( TCollection *files, UChar_t priority )
{
  using namespace XrdCl;
  std::vector<std::string> fileList;
  TIter it( files );
  TObject *object = 0;

  while( ( object = (TObject *) it.Next() ) )
  {
    TString path = TFileStager::GetPathName( object );
    if( path == "" )
    {
      Warning( "Stage", "object is of unexpected type %s - ignoring",
                         object->ClassName() );
      continue;
    }

    fileList.push_back( std::string( URL( path.Data() ).GetPath() ) );
  }

  Buffer *response;
  XRootDStatus st = fFileSystem->Prepare( fileList, PrepareFlags::Stage,
                                          (uint8_t) priority, response );
  if( !st.IsOK() )
  {
    Error( "Stage", "%s", st.GetErrorMessage().c_str() );
    return -1;
  }

  return 0;
}


