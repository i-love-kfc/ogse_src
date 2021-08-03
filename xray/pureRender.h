#pragma once

// messages
#define REG_PRIORITY_LOW		0x11111111ul
#define REG_PRIORITY_NORMAL		0x22222222ul
#define REG_PRIORITY_HIGH		0x33333333ul
#define REG_PRIORITY_CAPTURE	0x7ffffffful
#define REG_PRIORITY_INVALID	0xfffffffful

struct pureRender;

struct pureRenderVtbl
{
  void (__thiscall *OnRender)(pureRender *_this);
};

struct pureRender
{
  pureRenderVtbl *vfptr;
};
