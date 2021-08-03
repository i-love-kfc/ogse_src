#include "RenderDevice.h"
#include "pureRender.h"

int	__cdecl	_REG_Compare(const void *e1, const void *e2)
{
	_REG_INFO *p1 = (_REG_INFO *)e1;
	_REG_INFO *p2 = (_REG_INFO *)e2;
	return (p2->Prio - p1->Prio);
};