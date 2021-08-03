#pragma once

#include "base_types.h"
#include "Shader.h"
#include "sound.h"
#include "xr_collide.h"


//#include "xr_collide_defs.h"

//refs
class IRender_DetailModel;


/*struct SRainParams
{
public:
int		max_desired_items;
float	source_radius, source_offset;
float	max_distance, sink_offset, drop_length;
float	drop_width, drop_angle, drop_max_angle;
float	drop_max_wind_vel, drop_speed_min, drop_speed_max;
int		max_particles, particles_cache;
float	particles_time;
u32		dwReferences;
public:
SRainParams();		
};*/

//
class /*__declspec(dllimport)*/ CEffect_Rain
{
	struct Item
	{
		_vector3<float> P;
		_vector3<float> Phit;
		_vector3<float> D;
		float fSpeed;
		unsigned int dwTime_Life;
		unsigned int dwTime_Hit;
		unsigned int uv_set;
		void			invalidate	()
		{
			dwTime_Life	= 0;
		}
	};
	struct Particle
	{
		CEffect_Rain::Particle *next;
		CEffect_Rain::Particle *prev;
		_matrix<float> mXForm;
		_sphere<float> bounds;
		float time;
	};
	enum States
	{
		stIdle = 0x0,
		stWorking = 0x1,
	};

	resptr_core<Shader,resptrcode_shader> SH_Rain;
	resptr_core<SGeometry,resptrcode_geom> hGeom_Rain;
	IRender_DetailModel *DM_Drop;
	resptr_core<SGeometry,resptrcode_geom> hGeom_Drops;
	xr_vector<CEffect_Rain::Item,xalloc<CEffect_Rain::Item> > items;
	CEffect_Rain::States state;
	xr_vector<CEffect_Rain::Particle,xalloc<CEffect_Rain::Particle> > particle_pool;
	CEffect_Rain::Particle *particle_active;
	CEffect_Rain::Particle *particle_idle;
	ref_sound snd_Ambient;
public:
	CEffect_Rain(CEffect_Rain const &);
	CEffect_Rain(void);
	~CEffect_Rain(void);
	CEffect_Rain& operator=(CEffect_Rain const &);
	void OnFrame(void);
	void Render(void);
	void phase_rmap();
	
private:
	//__declspec(dllexport)
	void Born(Item &,float);
	void Hit(_vector3<float> &);
	int RayPick(_vector3<float> const &,_vector3<float> const &,float &, collide::rq_target);
	void RenewItem(Item &,float,int);	
	
	Particle * p_allocate(void);
	void p_create(void);
	void p_destroy(void);
	void p_free(Particle *);
	void p_insert(Particle *,Particle * &);
	void p_remove(Particle *,Particle * &);
	int p_size(Particle *);
};

class rain_timer_params
{
public:
	Fvector		timer;
	bool		not_first_frame;
	float		rain_timestamp;
	float		rain_drop_time_basic;
	float		previous_frame_time;
	float		last_rain_duration;
	float		rain_drop_time;
	float		rain_timer;

	rain_timer_params():not_first_frame(FALSE),rain_timestamp(0), rain_drop_time_basic(20.0), previous_frame_time(0), last_rain_duration(0), rain_drop_time(0), rain_timer(0) {timer.set(0.0,0.0,0.0);};
	IC void SetDropTime(float time)
	{
		rain_drop_time_basic = time;
	}
	int Update(BOOL state, bool need_raypick);
	BOOL RayPick(const Fvector& s, const Fvector& d, float& range, collide::rq_target tgt);
};

extern rain_timer_params *rain_timers_raycheck;
extern rain_timer_params *rain_timers;
extern Fvector4 rain_params;
/*
class __declspec(dllimport) CEffect_Rain
{
private:
	struct	Item
	{
		_vector3<float> P;
		_vector3<float> Phit;
		_vector3<float> D;
		float			fSpeed;
		u32				dwTime_Life;
		u32				dwTime_Hit;
		u32				uv_set;
		void			invalidate	()
		{
			dwTime_Life	= 0;
		}
	};
	struct	Particle
	{
		Particle		*next,*prev;
		Fmatrix			mXForm;
		Fsphere			bounds;
		float			time;
	};
	enum	States
	{
		stIdle		= 0,
		stWorking
	};
private:
	// Visualization	(rain)
	ref_shader						SH_Rain;
	ref_geom						hGeom_Rain;

	// Visualization	(drops)
	IRender_DetailModel*			DM_Drop;
	ref_geom						hGeom_Drops;

	// Data and logic
	xr_vector<Item>					items;
	States							state;

	// Particles
	xr_vector<Particle>				particle_pool;
	Particle*						particle_active;
	Particle*						particle_idle;

	// Sounds
	ref_sound						snd_Ambient;

	// Utilities
	void							p_create		();
	void							p_destroy		();

	void							p_remove		(Particle* P, Particle* &LST);
	void							p_insert		(Particle* P, Particle* &LST);
	int								p_size			(Particle* LST);
	Particle*						p_allocate		();
	void							p_free			(Particle* P);

	// Some methods
	void							Born			(Item& dest, float radius);
	void							Hit				(Fvector& pos);
	BOOL							RayPick			(const Fvector& s, const Fvector& d, float& range, collide::rq_target tgt);
	void							RenewItem		(Item& dest, float height, BOOL bHit);
public:
	CEffect_Rain	();
	~CEffect_Rain	();

	void							Render			();
	void							OnFrame			();
};

*/