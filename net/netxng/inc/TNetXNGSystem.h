//------------------------------------------------------------------------------
// Copyright (c) 2013 by European Organization for Nuclear Research (CERN)
// Author: Lukasz Janyst <ljanyst@cern.ch>
// Author: Justin Salmon <jsalmon@cern.ch>
//------------------------------------------------------------------------------

#ifndef ROOT_TNetXNGSystem
#define ROOT_TNetXNGSystem

#include "TSystem.h"
#include <XrdCl/XrdClXRootDResponses.hh>
#include <XrdCl/XrdClURL.hh>

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
namespace XrdCl
{
  class FileSystem;
}

//------------------------------------------------------------------------------
//! Access system interface using the new client
//------------------------------------------------------------------------------
class TNetXNGSystem: public TSystem
{
  public:
    //--------------------------------------------------------------------------
    //! Constructor - Create system class without connecting to server
    //!
    //! @param owner (unused)
    //--------------------------------------------------------------------------
    TNetXNGSystem( Bool_t owner = kTRUE );

    //--------------------------------------------------------------------------
    //! Constructor - Create system class and connect to server
    //!
    //! @param url   URL of the entry-point server to be contacted
    //! @param owner (unused)
    //--------------------------------------------------------------------------
    TNetXNGSystem( const char *url, Bool_t owner = kTRUE );

    //--------------------------------------------------------------------------
    //! Destructor
    //--------------------------------------------------------------------------
    virtual ~TNetXNGSystem();

    //--------------------------------------------------------------------------
    //! Open a directory
    //!
    //! @param dir the name of the directory to open
    //! @returns   a non-zero pointer (with no special purpose) in case of
    //!            success, 0 in case of error
    //--------------------------------------------------------------------------
    virtual void* OpenDirectory( const char* dir );

    //--------------------------------------------------------------------------
    //! Create a directory
    //!
    //! @param dir the directory name
    //! @returns   0 on success, -1 otherwise
    //--------------------------------------------------------------------------
    virtual Int_t MakeDirectory( const char* dir );

    //--------------------------------------------------------------------------
    //! Free a directory
    //!
    //! @param dirp the pointer to the directory to be freed
    //--------------------------------------------------------------------------
    virtual void FreeDirectory( void *dirp );

    //--------------------------------------------------------------------------
    //! Get a directory entry.
    //!
    //! @param dirp the directory pointer
    //! @returns    0 in case there are no more entries
    //--------------------------------------------------------------------------
    virtual const char* GetDirEntry( void *dirp );

    //--------------------------------------------------------------------------
    //! Get info about a file (stat)
    //!
    //! @param path the path of the file to stat (in)
    //! @param buf  structure that will hold the stat info (out)
    //! @returns    0 if success, 1 if the file could not be stat'ed
    //--------------------------------------------------------------------------
    virtual Int_t GetPathInfo( const char* path, FileStat_t &buf );

    //--------------------------------------------------------------------------
    //! Check consistency of this helper with the one required by 'path' or
    //! 'dirptr'
    //!
    //! @param path   the path to check
    //! @param dirptr the directory pointer to check
    //--------------------------------------------------------------------------
    virtual Bool_t ConsistentWith( const char *path, void *dirptr );

    //--------------------------------------------------------------------------
    //! Unlink a file on the remote server
    //!
    //! @param path the path of the file to unlink
    //! @returns    0 on success, -1 otherwise
    //--------------------------------------------------------------------------
    virtual int Unlink( const char *path );

    //--------------------------------------------------------------------------
    //! Is this path a local path?
    //!
    //! @param path the URL of the path to check
    //! @returns    kTRUE if the path is local, kFALSE otherwise
    //--------------------------------------------------------------------------
    virtual Bool_t IsPathLocal( const char *path );

    //--------------------------------------------------------------------------
    //! Get the endpoint URL of a file.
    //!
    //! @param path   the entry-point URL of the file (in)
    //! @param endurl the endpoint URL of the file (out)
    //! @returns      0 in case of success and 1 if the file could not be
    //!               stat'ed.
    //--------------------------------------------------------------------------
    virtual Int_t Locate( const char* path, TString &endurl );

    //--------------------------------------------------------------------------
    //! ROOT class definition
    //--------------------------------------------------------------------------
    ClassDef(TNetXNGSystem, 0 )

  private:
    //--------------------------------------------------------------------------
    //! Data members
    //--------------------------------------------------------------------------
    XrdCl::FileSystem              *fFileSystem;
    XrdCl::URL                     *fUrl;
    XrdCl::DirectoryList           *fDirList;
    XrdCl::DirectoryList::Iterator  fDirListIter;
};

#endif
