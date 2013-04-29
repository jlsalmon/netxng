//------------------------------------------------------------------------------
// Copyright (c) 2013 by European Organization for Nuclear Research (CERN)
// Author: Lukasz Janyst <ljanyst@cern.ch>
//------------------------------------------------------------------------------

#ifndef ROOT_TNetXNGFileStager
#define ROOT_TNetXNGFileStager

//------------------------------------------------------------------------------
// Interface to the 'XRD' staging capabilities.
//------------------------------------------------------------------------------

#include "TFileStager.h"

class TCollection;
class TNetXNGSystem;
class TFileCollection;

class TNetXNGFileStager: public TFileStager
{
  public:
    TNetXNGFileStager(const char *stager = "");
    virtual ~TNetXNGFileStager();

    Bool_t  IsStaged(const char *path);
    Int_t   Locate(const char *path, TString &endpath);
    Int_t   LocateCollection(TFileCollection *fc, Bool_t addDummyUrl = kFALSE);
    Bool_t  Matches(const char *s);
    Bool_t  Stage(const char *path, Option_t *opt = 0);
    Bool_t  Stage(TCollection *pathlist, Option_t *opt = 0);
    Bool_t  IsValid() const { return (fSystem ? kTRUE : kFALSE); }
    void    Print(Option_t *option = "") const;

   ClassDef( TNetXNGFileStager, 0 )  // Interface to a 'XRD' staging

  private:
    TNetXNGSystem   *fSystem;
};

#endif
