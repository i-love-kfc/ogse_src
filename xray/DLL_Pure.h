#pragma once

struct DLL_Pure;

struct DLL_PureVtbl
{
  DLL_Pure *(__thiscall *_construct)(DLL_Pure *_this);
  void *(__thiscall *__vecDelDtor)(DLL_Pure *_this, unsigned int);
};

struct DLL_Pure
{
  DLL_PureVtbl *vfptr;
  unsigned __int64 CLS_ID;
};
