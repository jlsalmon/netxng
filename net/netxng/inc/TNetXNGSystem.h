//------------------------------------------------------------------------------
// Copyright (c) 2013 by European Organization for Nuclear Research (CERN)
// Author: Lukasz Janyst <ljanyst@cern.ch>
//------------------------------------------------------------------------------

#ifndef ROOT_TNetXNGSystem
#define ROOT_TNetXNGSystem

#include "TSystem.h"

class TNetXNGSystem: public TSystem
{
  public:
    TNetXNGSystem(Bool_t owner = kTRUE);
    TNetXNGSystem(const char *url, Bool_t owner = kTRUE);
    virtual ~TNetXNGSystem() { }

    virtual Bool_t      ConsistentWith(const char *path, void *dirptr);
    virtual void        FreeDirectory(void *dirp);
    virtual const char *GetDirEntry(void *dirp);
    virtual Int_t       GetPathInfo(const char* path, FileStat_t &buf);
    virtual Bool_t      IsPathLocal(const char *path);
    virtual Int_t       Locate(const char* path, TString &endurl);
    virtual Int_t       MakeDirectory(const char* dir);
    virtual void       *OpenDirectory(const char* dir);
    virtual int         Unlink(const char *path);

   ClassDef(TNetXNGSystem, 0 )
};

#endif
