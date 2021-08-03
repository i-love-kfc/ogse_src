#include "WallmarksEngine.h"
#include "xr_render_console.h"
#include "r2.h"
#include "RenderDevice.h"

namespace WallmarksEngine {
	struct wm_slot
	{
		typedef CWallmarksEngine::StaticWMVec	StaticWMVec;
		ref_shader		shader;
		StaticWMVec		static_items;
		xr_vector< intrusive_ptr<CSkeletonWallmark> > skeleton_items;
						wm_slot		(ref_shader sh)	{shader=sh;static_items.reserve(256);skeleton_items.reserve(256);}
	};
}
IC bool operator == (const CWallmarksEngine::wm_slot* slot, const ref_shader& shader){return slot->shader==shader;}
CWallmarksEngine::wm_slot* CWallmarksEngine::FindSlot	(ref_shader shader)
{
	WMSlotVecIt it				= std::find(marks.begin(),marks.end(),shader);
	return						(it!=marks.end())?*it:0;
}
CWallmarksEngine::wm_slot* CWallmarksEngine::AppendSlot	(ref_shader shader)
{
	marks.push_back				(xr_new<wm_slot>(shader));
	return marks.back			();
}

void CWallmarksEngine::AddSkeletonWallmark	(const Fmatrix* xf, CKinematics* obj, ref_shader& sh, const Fvector& start, const Fvector& dir, float size)
{	
	if(!(ps_r2_ls_flags.test(R2FLAG_BLOODMARKS)) || RImplementation.phase != CRender::PHASE_NORMAL)				return;
	// optimization cheat: don't allow wallmarks more than 50 m from viewer/actor
	if (xf->c.distance_to_sqr(Device.vCameraPosition) > _sqr(50.f))				return;

	VERIFY					(obj&&xf&&(size>EPS_L));
	lock.Enter				();
	obj->AddWallmark		(xf,start,dir,sh,size);
	lock.Leave				();
}

void CWallmarksEngine::AddSkeletonWallmark(intrusive_ptr<CSkeletonWallmark> wm)
{
	if(!(ps_r2_ls_flags.test(R2FLAG_BLOODMARKS)) || RImplementation.phase != CRender::PHASE_NORMAL) return;

	if (!RImplementation.val_bHUD)
	{
		lock.Enter			();
		// search if similar wallmark exists
		wm_slot* slot		= FindSlot	(wm->Shader());
		if (0==slot) slot	= AppendSlot(wm->Shader());
		// no similar - register _new_
		slot->skeleton_items.push_back(wm);
		lock.Leave			();
	}
}
CWallmarksEngine::CWallmarksEngine	()
{
	static_pool.reserve		(256);
	marks.reserve			(256);
	hGeom.create			(FVF::F_LIT, RCache.Vertex.Buffer(), NULL);
}

CWallmarksEngine::~CWallmarksEngine	()
{
	clear			();
	hGeom.destroy	();
}
void CWallmarksEngine::clear()
{
	{
		for (WMSlotVecIt p_it=marks.begin(); p_it!=marks.end(); p_it++){
			for (StaticWMVecIt m_it=(*p_it)->static_items.begin(); m_it!=(*p_it)->static_items.end(); m_it++)
				static_wm_destroy	(*m_it);
			xr_delete		(*p_it);
		}
		marks.clear	();
	}
	{
		for (u32 it=0; it<static_pool.size(); it++)
			xr_delete		(static_pool[it]);
		static_pool.clear	();
	}
}
void		CWallmarksEngine::static_wm_destroy		(CWallmarksEngine::static_wallmark*	W	)
{
	static_pool.push_back	(W);
}