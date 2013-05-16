/*******************************************************************************
 * Copyright (C) 1995-2013, Rene Brun and Fons Rademakers.                     *
 * All rights reserved.                                                        *
 *                                                                             *
 * For the licensing terms see $ROOTSYS/LICENSE.                               *
 * For the list of contributors see $ROOTSYS/README/CREDITS.                   *
 ******************************************************************************/

#ifndef ROOT_TNetXNGFile
#define ROOT_TNetXNGFile

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// TNetXNGFile                                                                //
//                                                                            //
// Authors: Lukasz Janyst, Justin Salmon                                      //
//          CERN, 2013                                                        //
//                                                                            //
// Enables access to XRootD files using the new client.                       //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "TFile.h"
#ifndef __CINT__
#include <XrdCl/XrdClFileSystem.hh>
#endif

namespace XrdCl {
   class File;
}

class TNetXNGFile: public TFile {
private:
#ifndef __CINT__
   XrdCl::File            *fFile; // Underlying XRootD file
   XrdCl::URL             *fUrl;  // URL of the current file
   XrdCl::OpenFlags::Flags fMode; // Open mode of the current file
#endif

public:
   TNetXNGFile() :
         TFile(), fFile(0), fUrl(0), fMode(XrdCl::OpenFlags::None) {}
   TNetXNGFile(const char *url, Option_t *mode = "", const char *title = "",
         Int_t compress = 1, Int_t netopt = 0, Bool_t parallelopen = kFALSE);
   virtual ~TNetXNGFile();

   virtual Long64_t GetSize() const;
   virtual void     Close(const Option_t *option = "");
   virtual void     Seek(Long64_t offset, ERelativeTo position = kBeg);
   virtual Int_t    ReOpen(Option_t *modestr);
   virtual Bool_t   IsOpen() const;
   virtual Bool_t   WriteBuffer(const char *buffer, Int_t length);
   virtual Bool_t   ReadBuffer(char *buffer, Int_t length);
   virtual Bool_t   ReadBuffer(char *buffer, Long64_t position, Int_t length);
   virtual Bool_t   ReadBuffers(char *buffer, Long64_t *position, Int_t *length,
                                Int_t nbuffs);

ClassDef( TNetXNGFile, 0 ) // ROOT class definition

private:
   virtual Bool_t IsUseable() const;
#ifndef __CINT__
   XrdCl::OpenFlags::Flags ParseOpenMode(Option_t *modestr);
#endif

   TNetXNGFile(const TNetXNGFile &other);             // Not implemented
   TNetXNGFile &operator =(const TNetXNGFile &other); // Not implemented
};

#endif // ROOT_TNetXNGFile
