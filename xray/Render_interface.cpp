#include "Render_interface.h"

void IRender_interface::Screenshot (IRender_interface::ScreenshotMode mode, LPCSTR name)
{
	return vfptr->Screenshot(this, mode, name);
};
void IRender_interface::light_destroy (IRender_Light *light)
{
	return vfptr->light_destroy(this, light);
};
void IRender_interface::add_Geometry (IRender_Visual*	V)
{
	return vfptr->add_Geometry(this, V);
};
IRender_interface::GenerationLevel IRender_interface::get_generation ()
{
	return vfptr->get_generation(this);
};
void IRender_interface::Statistics (CGameFont *f)
{
	return vfptr->Statistics(this, f);
};