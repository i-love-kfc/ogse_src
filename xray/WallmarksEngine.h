// WallmarksEngine.h: interface for the CWallmarksEngine class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "xrXRC.h"
#include "shader.h"
#include "intrusive_ptr.h"
#include "FVF.h"
#include "Render.h"

namespace WallmarksEngine {
	struct wm_slot;
}
class CSkeletonWallmark : public intrusive_base // 4+4+4+12+4+16+16 = 60 + 4 = 64
{
	CKinematics*		m_Parent;		// 4
	const Fmatrix*		m_XForm;		// 4
	ref_shader			m_Shader;		// 4
	Fvector				m_ContactPoint;	// 12		model space
	float				m_fTimeStart;	// 4
public:
	Fsphere				m_LocalBounds;	// 16		model space
	struct WMFace{
		Fvector			vert	[3];
		Fvector2		uv		[3];
		u16				bone_id	[3][2];
		float			weight	[3];
	};
	DEFINE_VECTOR		(WMFace,WMFacesVec,WMFacesVecIt);
	WMFacesVec			m_Faces;		// 16 
public:
	Fsphere				m_Bounds;		// 16		world space
public:									
/*						CSkeletonWallmark	(CKinematics* p,const Fmatrix* m, ref_shader s, const Fvector& cp, float ts):
						m_Parent(p),m_XForm(m),m_Shader(s),m_fTimeStart(ts),m_ContactPoint(cp)
						{
}
						~CSkeletonWallmark	()							{}*/

	IC CKinematics*		Parent				(){return m_Parent;}
	IC u32				VCount				(){return m_Faces.size()*3;}
//	IC bool				Similar				(ref_shader& sh, const Fvector& cp, float eps){return (m_Shader==sh)&&m_ContactPoint.similar(cp,eps);}
	IC float			TimeStart			(){return m_fTimeStart;}
	IC const Fmatrix*	XFORM				(){return m_XForm;}
	IC const Fvector&	ContactPoint		(){return m_ContactPoint;}
	IC ref_shader		Shader				(){return m_Shader;}
};

class CKinematics;

class CWallmarksEngine
{
public:
	typedef WallmarksEngine::wm_slot	wm_slot;

public:
	struct static_wallmark 
	{
		Fsphere				bounds;
		xr_vector<FVF::LIT>	verts;
		float				ttl;
	};
	template class std::vector<static_wallmark*, xalloc<static_wallmark*>>;
	template class std::vector<wm_slot*, xalloc<wm_slot*>>;
	DEFINE_VECTOR		(static_wallmark*,StaticWMVec,StaticWMVecIt);
	DEFINE_VECTOR		(wm_slot*,WMSlotVec,WMSlotVecIt);

private:
	StaticWMVec			static_pool;
	WMSlotVec			marks;
	ref_geom			hGeom;

	Fvector				sml_normal;
	CFrustum			sml_clipper;
	sPoly				sml_poly_dest;
	sPoly				sml_poly_src;

	xrXRC				xrc;
	CDB::Collector		sml_collector;
	xr_vector<u32>		sml_adjacency;

	xrCriticalSection	lock;

private:
	wm_slot*			FindSlot				(ref_shader shader);
	wm_slot*			AppendSlot				(ref_shader shader);

public:
						CWallmarksEngine		();
						~CWallmarksEngine		();
	void AddSkeletonWallmark	(const Fmatrix* xf, CKinematics* obj, ref_shader& sh, const Fvector& start, const Fvector& dir, float size);
	void AddSkeletonWallmark	(intrusive_ptr<CSkeletonWallmark> wm);
	void clear();
	void static_wm_destroy		(static_wallmark*	W	);

};
