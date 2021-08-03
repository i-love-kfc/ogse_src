#include "effect_rain.h"
#include "game_level.h"
#include "Game_Persistent.h"
#include "RenderDevice.h"
#include "xrCommon.h"
#include "xr_render_console.h"
#include "FVF.h"
#include "Backend.h"
#include "Render.h"
#include "r2_rendertarget.h"
#include "r2.h"

//const float drop_max_angle = DEG2RAD(10); // 10 * M_PI / 180 = 0.17453294f; // отклонение от вертикали
const float drop_speed_min		= 40.0;
const float drop_speed_max		= 80.0;
const float source_offset		= 40.0;
const u32	max_desired_items	= 3000;
const float source_radius		= 12.5;
const float max_distance		= source_offset * 1.25f;
const float sink_offset			= -(max_distance - source_offset);
const float drop_length			= 5.0;
const float	drop_width			= 0.3;
const u32	particles_cache		= 400;
const float particles_time		= 0.3;
const u32	max_particles		= 4000;

Fvector4 wet_params;
Fvector4 rain_params;

void CEffect_Rain::Born(Item &dest, float radius)
{
	//Log("CEffect_Rain::Born");
	CEnvironment &env = g_pGamePersistent->Environment();
	float angle = env.wind_strength_factor * 0.1f * env.CurrentEnv.wind_velocity * 0.05f;
	clamp(angle, 0.0f, 1.0f);
	float vert_angle = env.CurrentEnv.params_ex->rain_max_drop_angle * angle - 0.5f * M_PI;
//	Msg("wind_strength_factor = %f, wind_velocity = %f, angle = %f, vert_angle = %f", env.wind_strength_factor, env.CurrentEnv.wind_velocity, angle, 180 * vert_angle / M_PI);
	_vector3<float> axis;
	axis.setHP(env.CurrentEnv.wind_direction, vert_angle);

	dest.D.random_dir(axis, DEG2RAD(3), ::Random);
	float v12 = 2 * M_PI * ::Random.randI() / 32767.0f; // 0.0 - 2*pi
	radius *= sqrt(::Random.randI() / 32767.0f); //  0.0 - 1.0

	_vector3<float>&	view	= Device.vCameraPosition;
	dest.P.set(
		cos(v12) * radius + view.x - dest.D.x * source_offset,
		                    view.y            + source_offset,
		sin(v12) * radius + view.z - dest.D.z * source_offset);

	dest.fSpeed = ::Random.randF(drop_speed_min, drop_speed_max);
	float max_dist = max_distance;
	int ray_pick_res = RayPick(dest.P, dest.D, max_dist, collide::rqtBoth);
	RenewItem(dest, max_dist, ray_pick_res);
};


void	CEffect_Rain::Hit		(Fvector& pos)
{
	if (0!=::Random.randI(2))	return;
	Particle*	P	= p_allocate();
	if (0==P)	return;

	P->time						= particles_time;
	P->mXForm.rotateY			(::Random.randF(PI_MUL_2));
	P->mXForm.translate_over	(pos);
	P->mXForm.transform_tiny	(P->bounds.P,DM_Drop->bv_sphere.P);
	P->bounds.R					= DM_Drop->bv_sphere.R;
}

enum
{
	NO_RAIN,
	IS_RAIN,
};

BOOL rain_timer_params::RayPick(const Fvector& s, const Fvector& d, float& range, collide::rq_target tgt)
{
	BOOL bRes 			= TRUE;
	collide::rq_result	RQ;
	CObject* E 			= g_pGameLevel->CurrentViewEntity();
	bRes 				= g_pGameLevel->ObjectSpace.RayPick( s,d,range,tgt,RQ,E);	
    if (bRes) range 	= RQ.range;
    return bRes;
}
int rain_timer_params::Update	(BOOL state, bool need_raypick)
{
	float	factor				= g_pGamePersistent->Environment().CurrentEnv.rain_density;
	CEffect_Rain* rain_manager  = g_pGamePersistent->Environment().eff_Rain;
	if (factor>EPS_L)
	{
		// is raining	
		if (state)
		{
			// effect is enabled
			Fvector P, D;
			P = Device.vCameraPosition;	// cam position
			D.set(0,1,0);				// direction to sky
			float max_dist = max_distance;
			if (!need_raypick || !RayPick(P, D, max_dist, collide::rqtBoth))
			{
				// under the sky
				if (!not_first_frame) 
				{
					// first frame
					not_first_frame = TRUE;
					rain_drop_time = rain_drop_time_basic / factor;		// speed of getting wet
					rain_timestamp = Device.fTimeGlobal;
					if (rain_timer > EPS)
						rain_timestamp += last_rain_duration - rain_timer - _min(rain_drop_time, last_rain_duration);
					last_rain_duration = 0;
				}
				// проверяем, не отрицателен ли дождевой таймер, если отрицателен - обнуляем
				// такое может быть при первом кадре с дождем, если до этого дождь уже как-то раз был в текущей игровой сессии
				if (rain_timer < 0)
					rain_timer = 0;
				rain_timer = Device.fTimeGlobal - rain_timestamp;
			}
			else
			{
				// under the cover. but may be it just appear
				if (rain_timer > EPS)
				{
					// yes, actor was under the sky recently
					float delta = rain_timer - (Device.fTimeGlobal - previous_frame_time);
					rain_timer = (delta>0)?delta:0;
					if (not_first_frame) 
					{
						// first update since rain was stopped
						not_first_frame = FALSE;
						last_rain_duration = Device.fTimeGlobal - rain_timestamp;
					}
				}
			}
		}
		else
		{
			// effect is disabled, reset all
			not_first_frame = FALSE;
			last_rain_duration = 0;
			rain_timer = 0;
			rain_timestamp = Device.fTimeGlobal;
		}
		previous_frame_time = Device.fTimeGlobal;
		timer.set(rain_timer, last_rain_duration, rain_drop_time);
		return IS_RAIN;
	}
	else
	{
		// no rain. but may be it just stop
		if (rain_timer > EPS)
		{
			// yes, it has been raining recently
			// so decrease timer
			float delta = rain_timer - (Device.fTimeGlobal - previous_frame_time);
			rain_timer = (delta>0)?delta:0;
			if (not_first_frame) 
			{
				// first update since rain was stopped
				not_first_frame = FALSE;
				last_rain_duration = Device.fTimeGlobal - rain_timestamp;
			}
			previous_frame_time = Device.fTimeGlobal;
		}
		timer.set(rain_timer, last_rain_duration, rain_drop_time);
		return NO_RAIN;
	}
}
static bool rain_flag = false;
static float start_timer = 0.0;
void	CEffect_Rain::Render	()
{
	if (!g_pGameLevel)			return;

	RImplementation.Target->phase_rain();

	float	factor				= g_pGamePersistent->Environment().CurrentEnv.rain_density;
//	if (factor<EPS_L)			return;

//	factor *= saturate(g_pGamePersistent->Environment().CurrentEnv.params_ex->rain_increase_speed * (Device.fTimeGlobal - start_timer));

	rain_timers->Update(true/*ps_r2_test_flags.test(R2FLAG_RAIN_MAP & R2FLAG_WET_SURFACES)*/, false);
	//rain params update
	{

		rain_params.x = (rain_timers->timer.x - rain_timers->timer.y)/ps_r2_rain_params.x
					+ lerp<float>(0, saturate(rain_timers->timer.y/ps_r2_rain_params.x), saturate(rain_timers->timer.y));
		rain_params.y = (rain_timers->timer.x - rain_timers->timer.y)/ps_r2_rain_params.y
					+ lerp<float>(0, saturate(rain_timers->timer.y/ps_r2_rain_params.y), saturate(rain_timers->timer.y));
		rain_params.z = (rain_timers->timer.x - rain_timers->timer.y)/ps_r2_rain_params.z
					+ lerp<float>(0, saturate(rain_timers->timer.y/ps_r2_rain_params.z), saturate(rain_timers->timer.y));
		rain_params.w = factor;
	}
	if (rain_params.x > EPS) phase_rmap();
//	Msg("rain_params: %f,%f,%f,%f", wet_params.x, wet_params.y, wet_params.z, wet_params.w);
	if (NO_RAIN==rain_timers_raycheck->Update(ps_r2_pp_flags.test(R2PP_FLAG_RAIN_DROPS), true)) {
		rain_flag = false;
		return;
	}
	if (!rain_flag) {
		start_timer = Device.fTimeGlobal;
		rain_flag = true;
	}

	u32 desired_items			= iFloor	(saturate(g_pGamePersistent->Environment().CurrentEnv.params_ex->rain_increase_speed*factor*factor)*float(max_desired_items));
	if (desired_items < 1)
		desired_items = 1;
//	Msg("desired items: %d", desired_items);
	// visual
	float		factor_visual	= factor/2.f+.5f;
	Fvector3	f_rain_color	= g_pGamePersistent->Environment().CurrentEnv.params_ex->rain_color;
	u32			u_rain_color	= color_rgba_f(f_rain_color.x,f_rain_color.y,f_rain_color.z,factor_visual);

	// born _new_ if needed
	float	b_radius_wrap_sqr	= _sqr((source_radius+.5f));
	if (items.size()<desired_items)	{
		// items.reserve		(desired_items);
		while (items.size()<desired_items)	{
			Item				one;
			Born				(one,source_radius);
			items.push_back		(one);
		}
	}/* else {
		items.erase(items.begin()+desired_items, items.end());
	}*/

	// build source plane
    Fplane src_plane;
    Fvector norm	={0.f,-1.f,0.f};
    Fvector upper; 	upper.set(Device.vCameraPosition.x,Device.vCameraPosition.y + source_offset,Device.vCameraPosition.z);
    src_plane.build(upper,norm);
	
	// perform update
	u32			vOffset;
	FVF::LIT	*verts		= (FVF::LIT	*) RCache.Vertex.Lock(desired_items*4,hGeom_Rain->vb_stride,vOffset);
	FVF::LIT	*start		= verts;
	const Fvector&	vEye	= Device.vCameraPosition;
	for (u32 I=0; I<desired_items/*items.size()*/; I++){
		// physics and time control
		Item&	one		=	items[I];

		if (one.dwTime_Hit<Device.dwTimeGlobal)		Hit (one.Phit);
		if (one.dwTime_Life<Device.dwTimeGlobal)	Born(one,source_radius);

// последняя дельта ??
//.		float xdt		= float(one.dwTime_Hit-Device.dwTimeGlobal)/1000.f;
//.		float dt		= Device.fTimeDelta;//xdt<Device.fTimeDelta?xdt:Device.fTimeDelta;
		float dt		= Device.fTimeDelta;
		one.P.mad		(one.D,one.fSpeed*dt);

		Device.Statistic->TEST1.Begin();
		Fvector	wdir;	wdir.set(one.P.x-vEye.x,0,one.P.z-vEye.z);
		float	wlen	= wdir.square_magnitude();
		if (wlen>b_radius_wrap_sqr)	{
			wlen		= _sqrt(wlen);
//.			Device.Statistic->TEST3.Begin();
			if ((one.P.y-vEye.y)<sink_offset){
				// need born
				one.invalidate();
			}else{
				Fvector		inv_dir, src_p;
				inv_dir.invert(one.D);
				wdir.div	(wlen);
				one.P.mad	(one.P, wdir, -(wlen+source_radius));
				if (src_plane.intersectRayPoint(one.P,inv_dir,src_p)){
					float dist_sqr	= one.P.distance_to_sqr(src_p);
					float height	= max_distance;
					if (RayPick(src_p,one.D,height,collide::rqtBoth)){	
						if (_sqr(height)<=dist_sqr){ 
							one.invalidate	();								// need born
//							Log("1");
						}else{	
							RenewItem	(one,height-_sqrt(dist_sqr),TRUE);		// fly to point
//							Log("2",height-dist);
						}
					}else{
						RenewItem		(one,max_distance-_sqrt(dist_sqr),FALSE);		// fly ...
//						Log("3",1.5f*b_height-dist);
					}
				}else{
					// need born
					one.invalidate();
//					Log("4");
				}
			}
//.			Device.Statistic->TEST3.End();
		}
		Device.Statistic->TEST1.End();

		// Build line
		Fvector&	pos_head	= one.P;
		Fvector		pos_trail;	pos_trail.mad	(pos_head,one.D,-drop_length*factor_visual);
		
		// Culling
		Fvector sC,lineD;	float sR; 
		sC.sub			(pos_head,pos_trail);
		lineD.normalize	(sC);
		sC.mul			(.5f);
		sR				= sC.magnitude();
		sC.add			(pos_trail);
		if (!RImplementation.ViewBase.testSphere_dirty(sC,sR))	continue;
		
		static Fvector2 UV[2][4]={
			{{0,1},{0,0},{1,1},{1,0}},
			{{1,0},{1,1},{0,0},{0,1}}
		};

		// Everything OK - build vertices
		Fvector	P,lineTop,camDir;
		camDir.sub			(sC,vEye);
		camDir.normalize	();
		lineTop.crossproduct(camDir,lineD);
		float w = drop_width;
		u32 s	= one.uv_set;
		P.mad(pos_trail,lineTop,-w);	verts->set(P,u_rain_color,UV[s][0].x,UV[s][0].y);	verts++;
		P.mad(pos_trail,lineTop,w);		verts->set(P,u_rain_color,UV[s][1].x,UV[s][1].y);	verts++;
		P.mad(pos_head, lineTop,-w);	verts->set(P,u_rain_color,UV[s][2].x,UV[s][2].y);	verts++;
		P.mad(pos_head, lineTop,w);		verts->set(P,u_rain_color,UV[s][3].x,UV[s][3].y);	verts++;
	}
	u32 vCount					= (u32)(verts-start);
	RCache.Vertex.Unlock		(vCount,hGeom_Rain->vb_stride);
	
	// Render if needed
	if (vCount)	{
		HW.pDevice->SetRenderState	(D3DRS_CULLMODE,D3DCULL_NONE);
		RCache.set_xform_world		(Fidentity);
		RCache.set_Shader			(SH_Rain);
		RCache.set_Geometry			(hGeom_Rain);
		RCache.Render				(D3DPT_TRIANGLELIST,vOffset,0,vCount,0,vCount/2);
		HW.pDevice->SetRenderState	(D3DRS_CULLMODE,D3DCULL_CCW);
	}
	
	// Particles
	Particle*	P		= particle_active;
	if (0==P)			return;
	
	{
		float	dt				= Device.fTimeDelta;
		_IndexStream& _IS		= RCache.Index;
		RCache.set_Shader		(DM_Drop->shader);
		
		Fmatrix					mXform,mScale;
		int						pcount  = 0;
		u32						v_offset,i_offset;
		u32						vCount_Lock		= particles_cache*DM_Drop->number_vertices;
		u32						iCount_Lock		= particles_cache*DM_Drop->number_indices;
		IRender_DetailModel::fvfVertexOut* v_ptr= (IRender_DetailModel::fvfVertexOut*) RCache.Vertex.Lock	(vCount_Lock, hGeom_Drops->vb_stride, v_offset);
		u16*					i_ptr			= _IS.Lock													(iCount_Lock, i_offset);
		while (P)	{
			Particle*	next	=	P->next;
			
			// Update
			// P can be zero sometimes and it crashes
			P->time				-=	dt;
			if (P->time<0)	{
				p_free			(P);
				P				=	next;
				continue;
			}

			// Render
			if (::Render->ViewBase.testSphere_dirty(P->bounds.P, P->bounds.R))
			{
				// Build matrix
				float scale			=	P->time / particles_time;
				mScale.scale		(scale,scale,scale);
				mXform.mul_43		(P->mXForm,mScale);
				
				// XForm verts
				DM_Drop->transfer	(mXform,v_ptr,u_rain_color,i_ptr,pcount*DM_Drop->number_vertices);
				v_ptr			+=	DM_Drop->number_vertices;
				i_ptr			+=	DM_Drop->number_indices;
				pcount			++;

				if (pcount >= particles_cache) {
					// flush
					u32	dwNumPrimitives		= iCount_Lock/3;
					RCache.Vertex.Unlock	(vCount_Lock,hGeom_Drops->vb_stride);
					_IS.Unlock				(iCount_Lock);
					RCache.set_Geometry		(hGeom_Drops);
					RCache.Render			(D3DPT_TRIANGLELIST,v_offset, 0,vCount_Lock,i_offset,dwNumPrimitives);
					
					v_ptr					= (IRender_DetailModel::fvfVertexOut*)			RCache.Vertex.Lock	(vCount_Lock, hGeom_Drops->vb_stride, v_offset);
					i_ptr					= _IS.Lock										(iCount_Lock, i_offset);
					
					pcount	= 0;
				}
			}
			
			P = next;
		}

		// Flush if needed
		vCount_Lock						= pcount*DM_Drop->number_vertices;
		iCount_Lock						= pcount*DM_Drop->number_indices;
		u32	dwNumPrimitives				= iCount_Lock/3;
		RCache.Vertex.Unlock			(vCount_Lock,hGeom_Drops->vb_stride);
		_IS.Unlock						(iCount_Lock);
		if (pcount)	{
			RCache.set_Geometry		(hGeom_Drops);
			RCache.Render			(D3DPT_TRIANGLELIST,v_offset,0,vCount_Lock,i_offset,dwNumPrimitives);
		}
	}
}

// initialize particles pool
void CEffect_Rain::p_create		()
{
	// pool
	particle_pool.resize	(max_particles);
	for (u32 it=0; it<particle_pool.size(); it++)
	{
		Particle&	P	= particle_pool[it];
		P.prev			= it?(&particle_pool[it-1]):0;
		P.next			= (it<(particle_pool.size()-1))?(&particle_pool[it+1]):0;
	}
	
	// active and idle lists
	particle_active	= 0;
	particle_idle	= &particle_pool.front();
}

// destroy particles pool
void CEffect_Rain::p_destroy	()
{
	// active and idle lists
	particle_active	= 0;
	particle_idle	= 0;
	
	// pool
	particle_pool.clear	();
}

// _delete_ node from _list_
void CEffect_Rain::p_remove	(Particle* P, Particle* &LST)
{
	VERIFY		(P);
	Particle*	prev		= P->prev;	P->prev = NULL;
	Particle*	next		= P->next;	P->next	= NULL;
	if (prev) prev->next	= next;
	if (next) next->prev	= prev;
	if (LST==P)	LST			= next;
}

// insert node at the top of the head
void CEffect_Rain::p_insert	(Particle* P, Particle* &LST)
{
	VERIFY		(P);
	P->prev					= 0;
	P->next					= LST;
	if (LST)	LST->prev	= P;
	LST						= P;
}

// determine size of _list_
int CEffect_Rain::p_size	(Particle* P)
{
	if (0==P)	return 0;
	int cnt = 0;
	while (P)	{
		P	=	P->next;
		cnt +=	1;
	}
	return cnt;
}

// alloc node
CEffect_Rain::Particle*	CEffect_Rain::p_allocate	()
{
	Particle*	P			= particle_idle;
	if (0==P)				return NULL;
	p_remove	(P,particle_idle);
	p_insert	(P,particle_active);
	return		P;
}

// xr_free node
void	CEffect_Rain::p_free(Particle* P)
{
	p_remove	(P,particle_active);
	p_insert	(P,particle_idle);
}

void	CEffect_Rain::phase_rmap()
{
	if (!ps_r2_test_flags.test(R2FLAG_RAIN_MAP) || !RImplementation.o.HW_smap) return;
	RImplementation.render_rain_near();
//	RImplementation.render_rain();
//	RImplementation.Target->rmap_blend			();
}