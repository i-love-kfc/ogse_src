#include "weapon.h"
#include "object.h"
#include "game_level.h"
#include "RenderDevice.h"

#define CROW_RADIUS (30.f)
#define CROW_RADIUS2 (60.f)
const	float	base_spu_epsP		= 0.05f;
const	float	base_spu_epsR		= 0.05f;
static float state_time = 0;				// таймер нахождения оружия в текущем состоянии
static float state_time_heat = 0;			// таймер нагрева оружия
static float previous_heating = 0;		// "нагретость" оружия в предыдущем состоянии

_vector4<u32> w_states = {0,0,0,1};
_vector3<float> w_timers = {0,0,0};
BOOL CObject::AlwaysTheCrow()
{
	return vfptr->AlwaysTheCrow(this);
}
CWeapon* CObject::cast_weapon()
{
	return vfptr->cast_weapon(this);
}
void CObject::UpdateWeaponParams()
{
	if (Parent && (0==Parent->Props.net_ID))	// actor's thing
	{
		CWeapon *w = cast_weapon();
		if (w && (w->m_state != eHidden))
		{
			w_states.x = w->m_bZoomMode & 1;			//x = zoom mode, y - текущее состояние, z - старое состояние
			if (w_states.y != w->m_state)	// первый апдейт или стейт изменился
			{
				w_states.z = w_states.y;						// записываем старое состояние
				state_time_heat = state_time = Device.fTimeGlobal;	// инитим счетчики времени
				previous_heating = w_timers.z;				// сохраняем "нагретость" оружия
				w_timers.y = w_timers.x;						// записываем время нахождения в предыдущем состоянии
				w_states.y = w->m_state;				// обновляем состояние
			}
			// флаг бинокля в руках (в этом режиме не нужно размытие)
			if (NULL != strstr(NameSection.c_str(), "binoc"))
				w_states.w = 0;
			else
				w_states.w = 1;
			if ((w_states.y == eFire) || (w_states.y == eFire2))	//стреляем, значит оружие греется
			{
				w_timers.z = Device.fTimeGlobal - state_time_heat + previous_heating;
			}
			else		// не стреляем - оружие охлаждается
			{
				if (w_timers.z > EPS)		// оружие все еще нагрето
				{
					float tm = state_time_heat + previous_heating - Device.fTimeGlobal;
					w_timers.z = (tm<EPS)?0.f:tm;
				}
			}
			w_timers.x = Device.fTimeGlobal - state_time;		// обновляем таймер текущего состояния
		}
	}
}
void CObject::UpdateCL()
{
	spatial_update				(base_spu_epsP*5,base_spu_epsR*5);

	UpdateWeaponParams();

	// crow
	if (Parent == g_pGameLevel->CurrentViewEntity())
	{
		MakeMeCrow	();
	}
	else if (AlwaysTheCrow())		
	{
		MakeMeCrow	();
	}
	else
	{
		float dist = Device.vCameraPosition.distance_to_sqr(Position());
		if (dist < CROW_RADIUS*CROW_RADIUS)	
			MakeMeCrow	();
		else 
		if( (Visual() && Visual()->vis.hom_frame+2 > Device.dwFrame) && (dist < CROW_RADIUS2*CROW_RADIUS2) )
			MakeMeCrow	();
	}
}


//////////////////////////////////////////////////////////////////////////
ISpatial::ISpatial			(ISpatial_DB* space)
{
	spatial.sphere.P.set	(0,0,0);
	spatial.sphere.R		= 0;
	spatial.node_center.set	(0,0,0);
	spatial.node_radius		= 0;
	spatial.node_ptr		= NULL;
	spatial.sector			= NULL;
	spatial.space			= space;
}
ISpatial::~ISpatial			(void)
{
	spatial_unregister		();
}
BOOL	ISpatial::spatial_inside()
{
	float	dr	= -(- spatial.node_radius + spatial.sphere.R);
	if (spatial.sphere.P.x < spatial.node_center.x - dr)	return FALSE;
	if (spatial.sphere.P.x > spatial.node_center.x + dr)	return FALSE;
	if (spatial.sphere.P.y < spatial.node_center.y - dr)	return FALSE;
	if (spatial.sphere.P.y > spatial.node_center.y + dr)	return FALSE;
	if (spatial.sphere.P.z < spatial.node_center.z - dr)	return FALSE;
	if (spatial.sphere.P.z > spatial.node_center.z + dr)	return FALSE;
	return TRUE;
}

BOOL	verify_sp	(ISpatial* sp, Fvector& node_center, float node_radius)
{
	float	dr	= -(- node_radius + sp->spatial.sphere.R);
	if (sp->spatial.sphere.P.x < node_center.x - dr)	return FALSE;
	if (sp->spatial.sphere.P.x > node_center.x + dr)	return FALSE;
	if (sp->spatial.sphere.P.y < node_center.y - dr)	return FALSE;
	if (sp->spatial.sphere.P.y > node_center.y + dr)	return FALSE;
	if (sp->spatial.sphere.P.z < node_center.z - dr)	return FALSE;
	if (sp->spatial.sphere.P.z > node_center.z + dr)	return FALSE;
	return TRUE;
}

void	ISpatial::spatial_register	()
{
	spatial.type			|=	STYPEFLAG_INVALIDSECTOR;
	if (spatial.node_ptr)
	{
		// already registered - nothing to do
	} else {
		// register
		R_ASSERT				(spatial.space);
		spatial.space->insert	(this);
		spatial.sector			=	0;
	}
}

void	ISpatial::spatial_unregister()
{
	if (spatial.node_ptr)
	{
		// remove
		spatial.space->remove	(this);
		spatial.node_ptr		= NULL;
		spatial.sector			= NULL;
	} else {
		// already unregistered
	}
}

void	ISpatial::spatial_move	()
{
	if (spatial.node_ptr)
	{
		//*** somehow it was determined that object has been moved
		spatial.type		|=				STYPEFLAG_INVALIDSECTOR;

		//*** check if we are supposed to correct it's spatial location
		if						(spatial_inside())	return;		// ???
		spatial.space->remove	(this);
		spatial.space->insert	(this);
	} else {
		//*** we are not registered yet, or already unregistered
		//*** ignore request
	}
}
/*
void	ISpatial::spatial_updatesector_internal()
{
	IRender_Sector*		S				=	::Render->detectSector(spatial_sector_point());
	spatial.type						&=	~STYPEFLAG_INVALIDSECTOR;
	if (S)				spatial.sector	=	S;
}*/