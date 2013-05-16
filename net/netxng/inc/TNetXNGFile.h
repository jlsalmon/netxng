//------------------------------------------------------------------------------
// Copyright (c) 2013 by European Organization for Nuclear Research (CERN)
// Author: Lukasz Janyst <ljanyst@cern.ch>
// Author: Justin Salmon <jsalmon@cern.ch>
//------------------------------------------------------------------------------

#ifndef ROOT_TNetXNGFile
#define ROOT_TNetXNGFile

#include "TFile.h"

#ifndef __CINT__
#include <XrdCl/XrdClFileSystem.hh>
#endif

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
namespace XrdCl
{
  class File;
}

//------------------------------------------------------------------------------
//! Access XRootD file using the new client
//------------------------------------------------------------------------------
class TNetXNGFile: public TFile
{
  public:
    //--------------------------------------------------------------------------
    //! Constructor
    //--------------------------------------------------------------------------
    TNetXNGFile(): TFile(), fFile( 0 ), fUrl( 0 ), fMode( XrdCl::OpenFlags::None ) {}

    //--------------------------------------------------------------------------
    //! Constructor
    //!
    //! @param url          URL of the entry-point server to be contacted
    //! @param mode         initial file access mode
    //! @param title        title of the file (shown by ROOT browser)
    //! @param compress     compression level and algorithm
    //! @param netopt       TCP window size in bytes (do we need this?)
    //! @param parallelopen open asynchronously (do we need this also?)
    //--------------------------------------------------------------------------
    TNetXNGFile( const char  *url,
                 Option_t    *mode         = "",
                 const char  *title        = "",
                 Int_t        compress     = 1,
                 Int_t        netopt       = 0,
                 Bool_t       parallelopen = kFALSE );

    //--------------------------------------------------------------------------
    //! Destructor
    //--------------------------------------------------------------------------
    virtual ~TNetXNGFile();

    //--------------------------------------------------------------------------
    //! Get the file size. Returns -1 in the case that the file could not be
    //! stat'ed.
    //--------------------------------------------------------------------------
    virtual Long64_t GetSize() const;

    //--------------------------------------------------------------------------
    //! Is the file open?
    //--------------------------------------------------------------------------
    virtual Bool_t IsOpen() const;

    //--------------------------------------------------------------------------
    //! Close the file
    //!
    //! @param option if == "R", all TProcessIDs referenced by this file are
    //!               deleted (is this valid in xrootd context?)
    //--------------------------------------------------------------------------
    virtual void Close( const Option_t *option = "" );

    //--------------------------------------------------------------------------
    //! Reopen the file with the new access mode
    //!
    //! @param mode the new access mode
    //! @returns    0 in case the mode was successfully modified, 1 in case
    //!             the mode did not change (was already as requested or wrong
    //!             input arguments) and -1 in case of failure, in which case
    //!             the file cannot be used anymore
    //--------------------------------------------------------------------------
    virtual Int_t ReOpen( Option_t *modestr );

    //--------------------------------------------------------------------------
    //! Read a data chunk of the given size
    //!
    //! @param buffer a pointer to a buffer big enough to hold the data
    //! @param length number of bytes to be read
    //! @returns      kTRUE in case of failure
    //--------------------------------------------------------------------------
    virtual Bool_t ReadBuffer( char *buffer, Int_t length );

    //--------------------------------------------------------------------------
    //! Read a data chunk of the given size, starting from the given offset
    //!
    //! @param buffer   a pointer to a buffer big enough to hold the data
    //! @param position offset from the beginning of the file
    //! @param length   number of bytes to be read
    //! @returns        kTRUE in case of failure
    //--------------------------------------------------------------------------
    virtual Bool_t ReadBuffer( char *buffer, Long64_t position, Int_t length );

    //--------------------------------------------------------------------------
    //! Read scattered data chunks in one operation
    //!
    //! @param buffer   a pointer to a buffer big enough to hold all of the
    //!                 requested data
    //! @param position position[i] is the seek position of chunk i of len
    //!                 length[i]
    //! @param length   length[i] is the length of the chunk at offset
    //!                 position[i]
    //! @param nbuffs   number of chunks
    //! @returns        kTRUE in case of failure
    //--------------------------------------------------------------------------
    virtual Bool_t ReadBuffers( char     *buffer,
                                Long64_t *position,
                                Int_t    *length,
                                Int_t     nbuffs );

    //--------------------------------------------------------------------------
    //! Write a data chunk
    //!
    //! @param buffer the data to be written
    //! @param length the size of the buffer
    //! @returns      kTRUE in case of failure
    //--------------------------------------------------------------------------
    virtual Bool_t WriteBuffer( const char *buffer, Int_t length );

    //--------------------------------------------------------------------------
    //! Set the position within the file
    //!
    //! @param offset   the new offset relative to position
    //! @param position the relative position, either kBeg, kCur or kEnd
    //--------------------------------------------------------------------------
    virtual void Seek( Long64_t offset, ERelativeTo position = kBeg );

    //--------------------------------------------------------------------------
    //! ROOT class definition
    //--------------------------------------------------------------------------
    ClassDef( TNetXNGFile, 0 )

  private:
    virtual Bool_t IsUseable() const;

    //--------------------------------------------------------------------------
    //! Parse an file open mode given as a string into an integer that the
    //! client can use
    //!
    //! @param option the file open mode as a string
    //! @returns      correctly parsed option mode
    //--------------------------------------------------------------------------
#ifndef __CINT__
    XrdCl::OpenFlags::Flags ParseOpenMode( Option_t *modestr );
#endif

    //--------------------------------------------------------------------------
    //! Data members
    //--------------------------------------------------------------------------
#ifndef __CINT__
    XrdCl::File            *fFile;
    XrdCl::URL             *fUrl;
    XrdCl::OpenFlags::Flags fMode;
#endif

    //--------------------------------------------------------------------------
    //! The file is not copyable
    //--------------------------------------------------------------------------
    TNetXNGFile( const TNetXNGFile &other );
    TNetXNGFile &operator = ( const TNetXNGFile &other );
};

#endif // ROOT_TNetXNGFile
