#pragma once
#include "base_types.h"

struct IRenderable;
struct IRenderableVtbl
{
	void *(__thiscall *__vecDelDtor)(IRenderable *_this, unsigned int);
	void (__thiscall *renderable_Render)(IRenderable *_this);
	int (__thiscall *renderable_ShadowGenerate)(IRenderable *_this);
	int (__thiscall *renderable_ShadowReceive)(IRenderable *_this);
};

class IRender_Visual;
struct IRender_ObjectSpecific;
struct IRenderable
{
	IRenderableVtbl *vfptr;
	struct
	{
		_matrix<float> xform;
		IRender_Visual *visual;
		IRender_ObjectSpecific *pROS;
		int pROS_Allowed;
	} renderable;
	void						renderable_Render				()	
	{
		return vfptr->renderable_Render(this);
	};
	IRender_ObjectSpecific*		renderable_ROS();
};
