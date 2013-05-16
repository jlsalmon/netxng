//------------------------------------------------------------------------------
// Copyright (c) 2013 by European Organization for Nuclear Research (CERN)
// Author: Lukasz Janyst <ljanyst@cern.ch>
// Author: Justin Salmon <jsalmon@cern.ch>
//------------------------------------------------------------------------------

#ifndef ROOT_TNetXNGFileStager
#define ROOT_TNetXNGFileStager

#include "TFileStager.h"

class TCollection;
class TNetXNGSystem;
class TFileCollection;

//------------------------------------------------------------------------------
// Interface to the 'XRD' staging capabilities.
//------------------------------------------------------------------------------
class TNetXNGFileStager: public TFileStager
{
  public:
    //--------------------------------------------------------------------------
    //! Constructor
    //!
    //! @param url the URL of the entry-point server
    //--------------------------------------------------------------------------
    TNetXNGFileStager( const char *url = "" );

    //--------------------------------------------------------------------------
    //! Destructor
    //--------------------------------------------------------------------------
    virtual ~TNetXNGFileStager();

    //--------------------------------------------------------------------------
    //! Check if a file is ready to be used
    //!
    //! @param path the URL of the file
    //--------------------------------------------------------------------------
    Bool_t IsStaged( const char *path );

    //--------------------------------------------------------------------------
    //! Get actual endpoint URL
    //!
    //! @param path    the entry-point URL
    //! @param endpath the actual endpoint URL
    //! @returns       0 in the case of success and 1 if any error occurred
    //--------------------------------------------------------------------------
    Int_t  Locate( const char *path, TString &endpath );

    //--------------------------------------------------------------------------
    //! Bulk locate request for a collection of files
    //!
    //! @param fc          collection of files to be located
    //! @param addDummyUrl append a dummy noop URL if the file is not staged or
    //!                    redirector == endpoint
    //! @returns           < 0 in case of errors, number of files processed
    //!                    otherwise
    //--------------------------------------------------------------------------
    Int_t  LocateCollection( TFileCollection *fc, Bool_t addDummyUrl = kFALSE );

    //--------------------------------------------------------------------------
    //! Returns kTRUE if stager 's' is compatible with current stager. Avoids
    //! multiple instantiations of the potentially the same TNetXNGFileStager.
    //--------------------------------------------------------------------------
    Bool_t Matches( const char *s );

    //--------------------------------------------------------------------------
    //! Issue a stage request for a single file
    //!
    //! @param path the path of the file to stage
    //! @param opt  defines 'option' and 'priority' for 'Prepare': the format is
    //!             opt = "option=o priority=p"
    //--------------------------------------------------------------------------
    Bool_t Stage( const char *path, Option_t *opt = 0 );

    //--------------------------------------------------------------------------
    //! Issue stage requests for multiple files
    //!
    //! @param pathlist list of paths of files to stage
    //! @param opt      defines 'option' and 'priority' for 'Prepare': the
    //!                 format is opt = "option=o priority=p"
    //--------------------------------------------------------------------------
    Bool_t Stage( TCollection *pathlist, Option_t *opt = 0 );

    //--------------------------------------------------------------------------
    //! Is this stager valid (instantiated)
    //--------------------------------------------------------------------------
    Bool_t IsValid() const { return ( fSystem ? kTRUE : kFALSE ); }

  private:
    //--------------------------------------------------------------------------
    //! Get a staging priority value from an option string
    //--------------------------------------------------------------------------
    UChar_t ParseStagePriority( Option_t *opt );

    //--------------------------------------------------------------------------
    //! Data members
    //--------------------------------------------------------------------------
    TNetXNGSystem *fSystem;

    //--------------------------------------------------------------------------
    //! Interface to a 'XRD' staging
    //--------------------------------------------------------------------------
    ClassDef( TNetXNGFileStager, 0 )
};

#endif
