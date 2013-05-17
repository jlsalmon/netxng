/*******************************************************************************
 * Copyright (C) 1995-2013, Rene Brun and Fons Rademakers.                     *
 * All rights reserved.                                                        *
 *                                                                             *
 * For the licensing terms see $ROOTSYS/LICENSE.                               *
 * For the list of contributors see $ROOTSYS/README/CREDITS.                   *
 ******************************************************************************/

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// TNetXNGSystem                                                              //
//                                                                            //
// Authors: Lukasz Janyst, Justin Salmon                                      //
//          CERN, 2013                                                        //
//                                                                            //
// Enables access to XRootD files using the new client.                       //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "TNetXNGFile.h"
#include <XrdCl/XrdClURL.hh>
#include <XrdCl/XrdClFile.hh>
#include <XrdCl/XrdClXRootDResponses.hh>
#include <iostream>

ClassImp(TNetXNGFile);

//______________________________________________________________________________
TNetXNGFile::TNetXNGFile(const char *url,
                         Option_t   *mode,
                         const char *title,
                         Int_t       compress,
                         Int_t       /*netopt*/,
                         Bool_t      parallelopen) :
   TFile(url, "NET", title, compress)
{
   // Constructor
   //
   // param url:          URL of the entry-point server to be contacted
   // param mode:         initial file access mode
   // param title:        title of the file (shown by ROOT browser)
   // param compress:     compression level and algorithm
   // param netopt:       TCP window size in bytes (do we need this?)
   // param parallelopen: open asynchronously (do we need this also?)

   using namespace XrdCl;

   fFile = new File();
   fUrl  = new URL(std::string(url));
   fUrl->SetProtocol(std::string("root"));
   fMode = ParseOpenMode(mode);

   XRootDStatus status;
   if (!parallelopen) {

      // Open the file synchronously
      status = fFile->Open(fUrl->GetURL(), fMode);
      if (!status.IsOK()) {
         Error("Open", "%s", status.GetErrorMessage().c_str());
         return;
      } else
         TFile::Init(false);

   } else {

      // Open the file asynchronously
      TNetXNGAsyncOpenHandler *handler = new TNetXNGAsyncOpenHandler(this);
      fInitCondVar = new XrdSysCondVar();
      status = fFile->Open(fUrl->GetURL(), fMode, Access::None, handler);
      if (!status.IsOK()) {
         Error("Open", "%s", status.GetErrorMessage().c_str());
      }
   }
}

//______________________________________________________________________________
TNetXNGFile::~TNetXNGFile()
{
   // Destructor

   if (IsOpen())
      Close();
   delete fFile;
   delete fUrl;
}

//______________________________________________________________________________
void TNetXNGFile::Init(Bool_t create)
{
   // Initialize the file. Makes sure that the file is really open before
   // calling TFile::Init. It may block.

   if (fInitDone) {
      if (gDebug > 1) Info("Init", "TFile::Init already called once");
      return;
   }

   // If the async open didn't return yet, wait for it
   if (!IsOpen()) {
      fInitCondVar.Wait();
   }

   TFile::Init(create);
}

//______________________________________________________________________________
Long64_t TNetXNGFile::GetSize() const
{
   // Get the file size. Returns -1 in the case that the file could not be
   // stat'ed.

   using namespace XrdCl;

   // Check the file isn't a zombie or closed
   if (!IsUseable())
      return -1;

   StatInfo *info = 0;
   fFile->Stat(false, info);
   Long64_t size = info->GetSize();
   delete info;
   return size;
}

//______________________________________________________________________________
Bool_t TNetXNGFile::IsOpen() const
{
   // Check if the file is open

   return fFile->IsOpen();
}

//______________________________________________________________________________
void TNetXNGFile::SetAsyncOpenStatus(EAsyncOpenStatus status)
{
   // Set the status of an asynchronous file open

   fAsyncOpenStatus = status;
   // Unblock Init() if it is waiting
   fInitCondVar.Signal();
}

//______________________________________________________________________________
void TNetXNGFile::Close(const Option_t */*option*/)
{
   // Close the file
   //
   // param option: if == "R", all TProcessIDs referenced by this file are
   //               deleted (is this valid in xrootd context?)

   fFile->Close();
}

//______________________________________________________________________________
Int_t TNetXNGFile::ReOpen(Option_t *modestr)
{
   // Reopen the file with the new access mode
   //
   // param mode: the new access mode
   // returns:    0 in case the mode was successfully modified, 1 in case
   //             the mode did not change (was already as requested or wrong
   //             input arguments) and -1 in case of failure, in which case
   //             the file cannot be used anymore

   using namespace XrdCl;
   OpenFlags::Flags mode = ParseOpenMode(modestr);

   // Only Read and Update are valid modes
   if (mode != OpenFlags::Read && mode != OpenFlags::Update) {
      Error("ReOpen", "mode must be either READ or UPDATE, not %s", modestr);
      return 1;
   }

   // The mode is not really changing
   if (mode == fMode || (mode == OpenFlags::Update
                         && fMode == OpenFlags::New)) {
      return 1;
   }

   fFile->Close();
   fMode = mode;

   XRootDStatus st = fFile->Open(fUrl->GetURL(), fMode);
   if (!st.IsOK()) {
      Error("ReOpen", "%s", st.GetErrorMessage().c_str());
      return 1;
   }

   return 0;
}

//______________________________________________________________________________
Bool_t TNetXNGFile::ReadBuffer(char *buffer, Int_t length)
{
   // Read a data chunk of the given size
   //
   // param buffer: a pointer to a buffer big enough to hold the data
   // param length: number of bytes to be read
   // returns:      kTRUE in case of failure

   return ReadBuffer(buffer, fOffset, length);
}

//______________________________________________________________________________
Bool_t TNetXNGFile::ReadBuffer(char *buffer, Long64_t position, Int_t length)
{
   // Read a data chunk of the given size, starting from the given offset
   //
   // param buffer:   a pointer to a buffer big enough to hold the data
   // param position: offset from the beginning of the file
   // param length:   number of bytes to be read
   // returns:        kTRUE in case of failure

   using namespace XrdCl;
   if (gDebug > 0)
      Info("ReadBuffer", "offset: %lld length: %d", position, length);

   // Check the file isn't a zombie or closed
   if (!IsUseable())
      return kTRUE;

   // Read the data
   uint32_t bytesRead = 0;
   XRootDStatus st = fFile->Read(position, length, buffer, bytesRead);
   if (gDebug > 0)
      Info("ReadBuffer", "%s bytes read: %d", st.ToStr().c_str(), bytesRead);

   if (!st.IsOK()) {
      Error("ReadBuffer", "%s", st.GetErrorMessage().c_str());
      return kTRUE;
   }

   // Bump the globals
   fOffset     += length;
   fBytesRead  += bytesRead;
   fgBytesRead += bytesRead;
   fReadCalls  ++;
   fgReadCalls ++;

   return kFALSE;
}

//______________________________________________________________________________
Bool_t TNetXNGFile::ReadBuffers(char *buffer, Long64_t *position, Int_t *length,
      Int_t nbuffs)
{
   // Read scattered data chunks in one operation
   //
   // param buffer:   a pointer to a buffer big enough to hold all of the
   //                 requested data
   // param position: position[i] is the seek position of chunk i of len
   //                 length[i]
   // param length:   length[i] is the length of the chunk at offset
   //                 position[i]
   // param nbuffs:   number of chunks
   // returns:        kTRUE in case of failure

   using namespace XrdCl;

   // Check the file isn't a zombie or closed
   if (!IsUseable())
      return kTRUE;

   // Find the max size for a single readv buffer
   URL url(fFile->GetDataServer());
   FileSystem fs(url);
   Buffer arg;
   Buffer *response;
   arg.FromString(std::string("readv_ior_max"));

   XRootDStatus status = fs.Query(QueryCode::Config, arg, response);
   if (!status.IsOK()) {
      Error("ReadBuffers", "%s", status.GetErrorMessage().c_str());
      return kTRUE;
   }

   Int_t maxRead = TString(response->ToString()).Remove(response->GetSize() - 1)
                           .Atoi();
   delete response;

   // Build a list of chunks
   ChunkList chunks;
   for (int i = 0; i < nbuffs; ++i) {

      // If the length is bigger than max readv size, split into smaller chunks
      if (length[i] > maxRead) {
         Int_t nsplit = length[i] / maxRead;
         Int_t rem    = length[i] % maxRead;
         Int_t j;

         for (j = 0; j < nsplit; ++j)
            chunks.push_back(ChunkInfo(position[i] + (j * maxRead), maxRead));
         chunks.push_back(ChunkInfo(position[i] + (j * maxRead), rem));
      } else
         chunks.push_back(ChunkInfo(position[i], length[i]));
   }

   // Read the data
   VectorReadInfo *info = 0;
   XRootDStatus st = fFile->VectorRead(chunks, (void *) buffer, info);

   if (!st.IsOK()) {
      Error("ReadBuffers", "%s", st.GetErrorMessage().c_str());
      delete info;
      return kTRUE;
   }

   // Bump the globals
   fBytesRead  += info->GetSize();
   fgBytesRead += info->GetSize();
   fReadCalls  ++;
   fgReadCalls ++;

   delete info;
   return kFALSE;
}

//______________________________________________________________________________
Bool_t TNetXNGFile::WriteBuffer(const char *buffer, Int_t length)
{
   // Write a data chunk
   //
   // param buffer: the data to be written
   // param length: the size of the buffer
   // returns:      kTRUE in case of failure

   using namespace XrdCl;

   // Check the file isn't a zombie or closed
   if (!IsUseable())
      return kTRUE;

   // Write the data
   XRootDStatus st = fFile->Write(fOffset, length, buffer);
   if (!st.IsOK()) {
      Error("WriteBuffer", "%s", st.GetErrorMessage().c_str());
      return kTRUE;
   }

   // Bump the globals
   fOffset      += length;
   fBytesWrite  += length;
   fgBytesWrite += length;

   return kFALSE;
}

//______________________________________________________________________________
void TNetXNGFile::Seek(Long64_t offset, ERelativeTo position)
{
   // Set the position within the file
   //
   // param offset:   the new offset relative to position
   // param position: the relative position, either kBeg, kCur or kEnd

   SetOffset(offset, position);
}

//______________________________________________________________________________
XrdCl::OpenFlags::Flags TNetXNGFile::ParseOpenMode(Option_t *modestr)
{
   // Parse an file open mode given as a string into an integer that the
   // client can use
   //
   // param option: the file open mode as a string
   // returns:      correctly parsed option mode

   using namespace XrdCl;
   OpenFlags::Flags mode = OpenFlags::None;
   TString mod = ToUpper(TString(modestr));

   if (mod == "NEW" || mod == "CREATE")  mode = OpenFlags::New;
   else if (mod == "RECREATE")           mode = OpenFlags::Delete;
   else if (mod == "UPDATE")             mode = OpenFlags::Update;
   else if (mod == "READ")               mode = OpenFlags::Read;

   return mode;
}

//______________________________________________________________________________
Bool_t TNetXNGFile::IsUseable() const
{
   // Check the file is open and isn't a zombie

   if (IsZombie()) {
      Error("TNetXNGFile", "Object is in 'zombie' state");
      return kFALSE;
   }

   if (!IsOpen()) {
      Error("TNetXNGFile", "The remote file is not open");
      return kFALSE;
   }

   return kTRUE;
}

//______________________________________________________________________________
TNetXNGAsyncOpenHandler::TNetXNGAsyncOpenHandler(TNetXNGFile *file)
{
   fFile = file;
   fFile->SetAsyncOpenStatus(TFile::kAOSInProgress);
}

//______________________________________________________________________________
void TNetXNGAsyncOpenHandler::HandleResponse(XrdCl::XRootDStatus *status,
                                             XrdCl::AnyObject    *response)
{
   // Called when a response to associated request arrives or an error occurs
   delete response;
   if (status->IsOK()) {
      fFile->SetAsyncOpenStatus(TFile::kAOSSuccess);
   } else {
      fFile->SetAsyncOpenStatus(TFile::kAOSFailure);
   }

}
