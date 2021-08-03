#include "kinematics.h"
#include "RenderDevice.h"

void IRender_Visual::Render(float lod)
{
	return vfptr->Render(this, lod);
}
void IRender_Visual::Depart()
{
	return vfptr->Depart(this);
}
void IRender_Visual::Release()
{
	return vfptr->Release(this);
}
CKinematics* IRender_Visual::dcast_PKinematics()
{
	return vfptr->dcast_PKinematics(this);
}
void IRender_Visual::Load(char const *name, IReader *r, u32 id)
{
	return vfptr->Load(this, name, r, id);
}
int			ps_r__LightSleepFrames		= 10	;
void	smapvis::invalidate	()
{
	state		=	state_counting;
	frame_sleep	=	Device.dwFrame + ps_r__LightSleepFrames;
	invisible.clear	();
}