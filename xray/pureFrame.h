#pragma once

class pureFrame;

struct pureFrameVtbl
{
  void (__thiscall *OnFrame)(pureFrame *_this);
};

class pureFrame
{
public:
  pureFrameVtbl *vfptr;
};

