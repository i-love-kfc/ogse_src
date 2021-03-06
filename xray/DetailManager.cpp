#include "Game_Persistent.h"
#include "DetailManager.h"
#include "r2.h"
#include "xr_3da.h"
#include "xrCore.h"
#include "perf_misc.h"

const float dbgOffset			= 0.f;
const int	dbgItems			= 128;

int reset_frame = 0;

//--------------------------------------------------- Decompression
static int magic4x4[4][4] =
{
 	{ 0, 14,  3, 13},
	{11,  5,  8,  6},
	{12,  2, 15,  1},
	{ 7,  9,  4, 10}
};

void bwdithermap	(int levels, int magic[16][16])
{
	/* Get size of each step */
    float N = 255.0f / (levels - 1);

	/*
	* Expand 4x4 dither pattern to 16x16.  4x4 leaves obvious patterning,
	* and doesn't give us full intensity range (only 17 sublevels).
	*
	* magicfact is (N - 1)/16 so that we get numbers in the matrix from 0 to
	* N - 1: mod N gives numbers in 0 to N - 1, don't ever want all
	* pixels incremented to the next level (this is reserved for the
	* pixel value with mod N == 0 at the next level).
	*/

    float	magicfact = (N - 1) / 16;
    for ( int i = 0; i < 4; i++ )
		for ( int j = 0; j < 4; j++ )
			for ( int k = 0; k < 4; k++ )
				for ( int l = 0; l < 4; l++ )
					magic[4*k+i][4*l+j] =
					(int)(0.5 + magic4x4[i][j] * magicfact +
					(magic4x4[k][l] / 16.) * magicfact);
}
//--------------------------------------------------- Decompression

void CDetailManager::SSwingValue::lerp(const SSwingValue& A, const SSwingValue& B, float f)
{
	float fi	= 1.f-f;
	amp1		= fi*A.amp1  + f*B.amp1;
	amp2		= fi*A.amp2  + f*B.amp2;
	rot1		= fi*A.rot1  + f*B.rot1;
	rot2		= fi*A.rot2  + f*B.rot2;
	speed		= fi*A.speed + f*B.speed;
}
//---------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDetailManager::CDetailManager	()
{
	dtFS 		= 0;
	dtSlots		= 0;
	soft_Geom	= 0;
	hw_Geom		= 0;
	hw_BatchSize= 0;
	hw_VB		= 0;
	hw_IB		= 0;

//	hw_lod_Geom		= 0;
//	hw_lod_VB		= 0;
//	hw_lod_IB		= 0;

#ifdef DYNAMIC_DETAILS
	dm_current_size			= dm_size;
	dm_current_cache_line	= dm_cache_line;
	dm_current_cache1_line	= dm_cache1_line;
	dm_current_cache_size	= dm_cache_size;
	dm_current_fade			= dm_fade;
	ps_r__Detail_density	= ps_current_detail_density;

/*	CacheSlot1 						cache_level1[dm_cache1_line][dm_cache1_line];
	Slot*							cache		[dm_cache_line][dm_cache_line];	// grid-cache itself
	Slot							cache_pool	[dm_cache_size];				// just memory for slots*/
	cache_level1	= (CacheSlot1**)Memory.mem_alloc(dm_current_cache1_line*sizeof(CacheSlot1*));
	for (u32 i = 0; i < dm_current_cache1_line; ++i)
	{
		cache_level1[i]	= (CacheSlot1*)Memory.mem_alloc(dm_current_cache1_line*sizeof(CacheSlot1));
		for (u32 j = 0; j < dm_current_cache1_line; ++j)
			new (&(cache_level1[i][j])) CacheSlot1();
	}

	cache	= (Slot***)Memory.mem_alloc(dm_current_cache_line*sizeof(Slot**));
	for (u32 i = 0; i < dm_current_cache_line; ++i)
		cache[i] = (Slot**)Memory.mem_alloc(dm_current_cache_line*sizeof(Slot*));

	cache_pool	= (Slot *)Memory.mem_alloc(dm_current_cache_size*sizeof(Slot));
	for (u32 i = 0; i < dm_current_cache_size; ++i)
		new (&(cache_pool[i])) Slot();

#endif
}

CDetailManager::~CDetailManager	()
{
#ifdef DYNAMIC_DETAILS
	for (u32 i = 0; i < dm_current_cache_size; ++i)
		cache_pool[i].~Slot();
	Memory.mem_free(cache_pool);

	for (u32 i = 0; i < dm_current_cache_line; ++i)
		Memory.mem_free(cache[i]);
	Memory.mem_free(cache);

	for (u32 i = 0; i < dm_current_cache1_line; ++i)
	{
		for (u32 j = 0; j < dm_current_cache1_line; ++j)
			cache_level1[i][j].~CacheSlot1();
		Memory.mem_free(cache_level1[i]);
	}
	Memory.mem_free(cache_level1);

#endif
}

void CDetailManager::Load		()
{
	// Open file stream
	if (!FS.exist("$level$","level.details"))
	{
		dtFS	= NULL;
		return;
	}

	string_path			fn;
	FS.update_path		(fn,"$level$","level.details");
	dtFS				= FS.r_open(fn);

	// Header
	dtFS->r_chunk_safe	(0,&dtH,sizeof(dtH));
	R_ASSERT			(dtH.version == DETAIL_VERSION);
	u32 m_count			= dtH.object_count;

	// Models
	IReader* m_fs		= dtFS->open_chunk(1);
	for (u32 m_id = 0; m_id < m_count; m_id++)
	{
		CDetail*		dt	= xr_new<CDetail> ();
		IReader* S			= m_fs->open_chunk(m_id);
		dt->Load			(S);
		objects.push_back	(dt);
		S->close			();
	}
	m_fs->close		();

	// Get pointer to database (slots)
	IReader* m_slots	= dtFS->open_chunk(2);
	dtSlots				= (DetailSlot*)m_slots->pointer();
	m_slots->close		();

	// Initialize 'vis' and 'cache'
	for (u32 i=0; i<3; ++i)	m_visibles[i].resize(objects.size());
	cache_Initialize	();

	// Make dither matrix
	bwdithermap		(2,dither);

	// Hardware specific optimizations
	if (UseVS())	hw_Load		();
	else			soft_Load	();

	// swing desc
	// normal
	swing_desc[0].amp1	= pSettings->r_float("details","swing_normal_amp1");
	swing_desc[0].amp2	= pSettings->r_float("details","swing_normal_amp2");
	swing_desc[0].rot1	= pSettings->r_float("details","swing_normal_rot1");
	swing_desc[0].rot2	= pSettings->r_float("details","swing_normal_rot2");
	swing_desc[0].speed	= pSettings->r_float("details","swing_normal_speed");
	// fast
	swing_desc[1].amp1	= pSettings->r_float("details","swing_fast_amp1");
	swing_desc[1].amp2	= pSettings->r_float("details","swing_fast_amp2");
	swing_desc[1].rot1	= pSettings->r_float("details","swing_fast_rot1");
	swing_desc[1].rot2	= pSettings->r_float("details","swing_fast_rot2");
	swing_desc[1].speed	= pSettings->r_float("details","swing_fast_speed");
}
void CDetailManager::Unload		()
{
	if (UseVS())	hw_Unload	();
	else			soft_Unload	();

	for (DetailIt it=objects.begin(); it!=objects.end(); it++){
		(*it)->Unload();
		xr_delete		(*it);
    }
	objects.clear		();
	m_visibles[0].clear	();
	m_visibles[1].clear	();
	m_visibles[2].clear	();
	FS.r_close			(dtFS);
}

extern __declspec(dllimport) float r_ssaDISCARD;

void CDetailManager::UpdateVisibleM()
{
	Fvector		EYE				= Device.vCameraPosition;
	
	CFrustum	View;
//	View.CreateFromMatrix		(Device.mFullTransform, FRUSTUM_P_LRTB + FRUSTUM_P_FAR);
	/* KD: there is some bug: frustrum created from full transform matrix seems to be broken in some frames, so we should use saved frustrum from render interface*/
	View = RImplementation.ViewBase;

	float fade_limit			= dm_current_fade;	fade_limit=fade_limit*fade_limit;
//	float fade_start			= 1.f;		fade_start=fade_start*fade_start;
	float fade_start			= 0.f;		fade_start=fade_start*fade_start;
	float fade_range			= fade_limit-fade_start;
	float		r_ssaCHEAP		= 140*r_ssaDISCARD;//16*r_ssaDISCARD;

	//clear 'vis'
	for (int i = 0; i != 3; i++) {
		vis_list& list = m_visibles[i];
		for (u32 j = 0; j != list.size(); j++)
			list[j].clear_not_free();
	}

	// Initialize 'vis' and 'cache'
	// Collect objects for rendering
	Device.Statistic->RenderDUMP_DT_VIS.Begin	();
	for (u32 _mz=0; _mz<dm_current_cache1_line; _mz++){
		for (u32 _mx=0; _mx<dm_current_cache1_line; _mx++){
			CacheSlot1& MS		= cache_level1[_mz][_mx];
			if (MS.empty)		continue;

			u32 mask			= 0xff;
			u32 res				= View.testSAABB		(MS.vis.sphere.P,MS.vis.sphere.R,MS.vis.box.data(),mask);
			if (fcvNone==res)						 	continue;	// invisible-view frustum
			// test slots
			for (int _i=0; _i<dm_cache1_count*dm_cache1_count; _i++){
				Slot*	PS		= *MS.slots[_i];
				Slot& 	S 		= *PS;

				// if slot empty - continue
				if (S.empty)	continue;

				if (!RImplementation.HOM.w_visible(S.vis))	continue;	// invisible-occlusion

				// if upper test = fcvPartial - test inner slots
				// KD - if slot is far far away - do not test it.
//				if () {
					if (fcvPartial==res){
						u32 _mask	= mask;
						u32 _res	= View.testSAABB			(S.vis.sphere.P,S.vis.sphere.R,S.vis.box.data(),_mask);
						if (fcvNone==_res)						continue;	// invisible-view frustum
					}
//				}
				
				// Add to visibility structures
				if (Device.dwFrame>S.frame){
					// Calc fade factor	(per slot)
					float	dist_sq		= EYE.distance_to_sqr	(S.vis.sphere.P);
					if		(dist_sq>fade_limit)				continue;
					float	alpha		= (dist_sq<fade_start)?0.f:(dist_sq-fade_start)/fade_range;
					float	alpha_i		= 1.f - alpha;
					float	dist_sq_rcp	= 1.f / dist_sq;

					S.frame			= Device.dwFrame+Random.randI(15,30);
					for (int sp_id=0; sp_id<dm_obj_in_slot; sp_id++){
						SlotPart&			sp	= S.G		[sp_id];
						if (sp.id==DetailSlot::ID_Empty)	continue;

						sp.r_items[0].clear_not_free();
						sp.r_items[1].clear_not_free();
						sp.r_items[2].clear_not_free();

						float				R		= objects	[sp.id]->bv_sphere.R;
						float				Rq_drcp	= R*R*dist_sq_rcp;	// reordered expression for 'ssa' calc

						SlotItem			**siIT=&(*sp.items.begin()), **siEND=&(*sp.items.end());
						u32 si_idx = 0;
						for (; siIT!=siEND; siIT++){
							if (!(*siIT))
							{
/*#pragma todo				("Don't forget to remove this in release!")
								Msg("[DETAILS MANAGER]: SlotItem ptr is broken! siIT=%d, *siIT=%d in mz=%d, mx=%d, _i=%d, sp_id=%d, si_idx=%d", (u32)siIT, (u32)(*siIT), _mz, _mx, _i, sp_id, si_idx);*/
								continue;
							}
							SlotItem& Item			= *(*siIT);
							float   scale			= Item.scale_calculated	= Item.scale*alpha_i;
							float	ssa				= scale*scale*Rq_drcp;
							if (ssa < r_ssaDISCARD) continue;
							u32		vis_id			= 0;
							if (ssa > r_ssaCHEAP)	vis_id = Item.vis_ID;
							
							sp.r_items[vis_id].push_back	(*siIT);

							++si_idx;
//2							visible[vis_id][sp.id].push_back(&Item);
						}
					}
				}
				for (int sp_id=0; sp_id<dm_obj_in_slot; sp_id++){
					SlotPart&			sp	= S.G		[sp_id];
					if (sp.id==DetailSlot::ID_Empty)	continue;
					if (!sp.r_items[0].empty()) m_visibles[0][sp.id].push_back(&sp.r_items[0]);
					if (!sp.r_items[1].empty()) m_visibles[1][sp.id].push_back(&sp.r_items[1]);
					if (!sp.r_items[2].empty()) m_visibles[2][sp.id].push_back(&sp.r_items[2]);
				}
			}
		}
	}
	Device.Statistic->RenderDUMP_DT_VIS.End	();
}

void CDetailManager::Render	()
{
	if (0==RImplementation.Details)		return;	// possibly deleted
	if (0==dtFS)						return;
	if (!psDeviceFlags.is(rsDetails))	return;
	if (IsMainMenuActive())				return;

	// MT
	MT_SYNC					();

	Device.Statistic->RenderDUMP_DT_Render.Begin	();
	BEGIN_PERF_EVENT(D3DCOLOR_XRGB(0,0,255), L"DETAILS");

	float factor			= g_pGamePersistent->Environment().wind_strength_factor;
	swing_current.lerp		(swing_desc[0],swing_desc[1],factor);

	RCache.set_CullMode		(CULL_NONE);
	RCache.set_xform_world	(Fidentity);
	if (UseVS())			hw_Render	();
	else					soft_Render	();
	RCache.set_CullMode		(CULL_CCW);
	Device.Statistic->RenderDUMP_DT_Render.End	();
	m_frame_rendered		= Device.dwFrame;
	END_PERF_EVENT;
}


void __stdcall	CDetailManager::MT_CALC		()
{
	if (reset_frame == Device.dwFrame)	return;
	// ????????
	if (0==RImplementation.Details)		return;	// possibly deleted
	if (!psDeviceFlags.is(rsDetails))	return;
	if (0==dtFS)						return;
	if (IsMainMenuActive())				return;

	MT.Enter					();
	if (m_frame_calc!=Device.dwFrame)
		if ((m_frame_rendered+1)==Device.dwFrame) //already rendered
		{
			Fvector		EYE				= Device.vCameraPosition;
			int s_x	= iFloor			(EYE.x/dm_slot_size+.5f);
			int s_z	= iFloor			(EYE.z/dm_slot_size+.5f);

			Device.Statistic->RenderDUMP_DT_Cache.Begin	();
			cache_Update				(s_x,s_z,EYE,dm_max_decompress);
			Device.Statistic->RenderDUMP_DT_Cache.End	();

			UpdateVisibleM				();
			m_frame_calc				= Device.dwFrame;
		}
	MT.Leave					        ();
}