//------------------------------------------------------------------------------
// Copyright (c) 2013 by European Organization for Nuclear Research (CERN)
// Author: Lukasz Janyst <ljanyst@cern.ch>
//------------------------------------------------------------------------------

#include "TNetXNGSystem.h"
#include "Rtypes.h"

ClassImp(TNetXNGSystem);

TNetXNGSystem::TNetXNGSystem( Bool_t /*owner*/ )
{
}

TNetXNGSystem::TNetXNGSystem( const char */*url*/, Bool_t /*owner*/)
{
}

void *TNetXNGSystem::OpenDirectory( const char */*dir*/)
{
  return 0;
}

void TNetXNGSystem::FreeDirectory( void */*dirp*/)
{
}

Int_t TNetXNGSystem::MakeDirectory( const char */*dir*/ )
{
  return -1;
}

const char *TNetXNGSystem::GetDirEntry( void */*dirp*/ )
{
  return 0;
}

Int_t TNetXNGSystem::GetPathInfo( const char */*path*/, FileStat_t &/*buf*/ )
{
  return -1;
}

Bool_t TNetXNGSystem::ConsistentWith( const char */*path*/, void */*dirptr*/ )
{
  return kFALSE;
}

int TNetXNGSystem::Unlink( const char */*path*/ )
{
  return -1;
}

Bool_t TNetXNGSystem::IsPathLocal( const char */*path*/ )
{
  return kFALSE;
}

Int_t TNetXNGSystem::Locate( const char */*path*/, TString &/*eurl*/ )
{
  return -1;
}
