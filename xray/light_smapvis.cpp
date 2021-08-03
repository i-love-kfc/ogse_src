#include "kinematics.h"
#include "RenderDevice.h"
#include "r2.h"

void	smapvis::flushoccq	()
{
	// the tough part
	if	(testQ_frame != Device.dwFrame)			return;
	u32	fragments	=	RImplementation.occq_get(testQ_id);
	if	(0==fragments)			{
		// this is invisible shadow-caster, register it
		// next time we will not get this caster, so 'test_current' remains the same
		invisible.push_back	(testQ_V);
		test_count			--;
		testQ_V				= 0;
	} else {
		// this is visible shadow-caster, advance testing
		test_current		++;
	}
	if (test_current==test_count)	{
		// we are at the end of list
		if (state==state_working)	state	= state_usingTC;
	}
}
void	smapvis::resetoccq	()
{
	if (testQ_frame==(Device.dwFrame+1))		testQ_frame--;
	flushoccq		();
}