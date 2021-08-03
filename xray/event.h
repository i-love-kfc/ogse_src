#pragma once

#include "base_types.h"

struct IEventReceiver;

struct CEvent
{
  char *Name;
  xr_vector<IEventReceiver *> Handlers;
  unsigned int dwRefCount;
};

struct IEventReceiverVtbl
{
	char dummy[1];
//void (__thiscall *OnEvent)(IEventReceiver *_this, CEvent *, unsigned __int64, unsigned __int64);
};

struct IEventReceiver
{
  IEventReceiverVtbl *vfptr;
};

