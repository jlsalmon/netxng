//------------------------------------------------------------------------------
// Copyright (c) 2013 by European Organization for Nuclear Research (CERN)
// Author: Lukasz Janyst <ljanyst@cern.ch>
//------------------------------------------------------------------------------

#ifndef ROOT_TNetXNGFile
#define ROOT_TNetXNGFile

#include "TFile.h"

//------------------------------------------------------------------------------
// Forward decls
//------------------------------------------------------------------------------
namespace XrdCl
{
  class File;
};

//------------------------------------------------------------------------------
// Access XRootD file using the new client
//------------------------------------------------------------------------------
class TNetXNGFile: public TFile
{
  public:
    //--------------------------------------------------------------------------
    // Constructor
    //--------------------------------------------------------------------------
    TNetXNGFile(): TFile(), fFile(0) {}

    //--------------------------------------------------------------------------
    // Constructor
    //--------------------------------------------------------------------------
    TNetXNGFile( const char  *url,
                 Option_t    *option       = "",
                 const char  *title        = "",
                 Int_t        compress     = 1,
                 Int_t        netopt       = 0,
                 Bool_t       parallelopen = kFALSE );

    //--------------------------------------------------------------------------
    // Desctructor
    //--------------------------------------------------------------------------
    virtual ~TNetXNGFile();

    //--------------------------------------------------------------------------
    // 
    //--------------------------------------------------------------------------
    virtual Long64_t    GetSize() const;
    virtual Bool_t      IsOpen() const;
    virtual void     Close(const Option_t *opt ="");
    //virtual Int_t       ReOpen(Option_t *mode);
    virtual Bool_t   ReadBuffer( char *buf, Int_t len );
    virtual Bool_t   ReadBuffer( char *buf, Long64_t pos, Int_t len );
    virtual Bool_t   ReadBuffers(char *buf,  Long64_t *pos, Int_t *len, Int_t nbuf);
//    virtual Bool_t   WriteBuffer(const char *buffer, Int_t BufferLength);
    virtual void        Seek(Long64_t offset, ERelativeTo pos = kBeg);
    ClassDef( TNetXNGFile,0 )

  private:
    //--------------------------------------------------------------------------
    // Data members
    //--------------------------------------------------------------------------
    XrdCl::File *fFile;

    //--------------------------------------------------------------------------
    // 
    //--------------------------------------------------------------------------
    TNetXNGFile( const TNetXNGFile &other );
    TNetXNGFile &operator = (const TNetXNGFile &other );

};

#endif // ROOT_TNetXNGFile
