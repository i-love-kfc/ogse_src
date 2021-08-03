#include "Render.h"
#include "FVF.h"
#include "Backend.h"
#include "xr_3da.h"
#include "r2.h"

void CPortalTraverser::initialize	()
{
	f_shader.create					("portal");
	f_geom.create					(FVF::F_L, RCache.Vertex.Buffer(), 0);
}
// resources
IRender_Light::~IRender_Light	()			{	
	::Render->light_destroy		(this	);
}
// hemi & sun: update and smooth
/*
void	CROS_impl::update_smooth	(IRenderable* O)
{
	if (dwFrameSmooth == Device.dwFrame)
		return;

	dwFrameSmooth			=	Device.dwFrame					;
	if (O && (0==result_count))	update(O)						;	// First time only
	float	l_f				=	Device.fTimeDelta*1.0;
	clamp	(l_f,0.f,1.f)	;
	float	l_i				=	1.f-l_f							;
	hemi_smooth				=	hemi_value*l_f + hemi_smooth*l_i;
	sun_smooth				=	sun_value *l_f + sun_smooth *l_i;
}
*/
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*
CPortal::CPortal		()
{
#ifdef DEBUG
	Device.seqRender.Add(this,REG_PRIORITY_LOW-1000);
#endif
}

CPortal::~CPortal		()
{
#ifdef DEBUG
	Device.seqRender.Remove(this);
#endif
}

//
void	CPortal::Setup	(Fvector* V, int vcnt, CSector* face, CSector* back)
{
	// calc sphere
	Fbox				BB;
	BB.invalidate		();
	for (int v=0; v<vcnt; v++)
		BB.modify		(V[v]);
	BB.getsphere		(S.P,S.R);

	// 
	poly.assign			(V,vcnt);
	pFace				= face; 
	pBack				= back;
	marker				= 0xffffffff; 

	Fvector				N,T;
	N.set				(0,0,0);

	FPU::m64r();
	u32	_cnt			= 0;
	for (int i=2; i<vcnt; i++) {
		T.mknormal_non_normalized		(poly[0],poly[i-1],poly[i]);
		float		m	= T.magnitude	();
		if (m>EPS_S)	{
			N.add		(T.div(m))	;
			_cnt		++			;
		}
	}
	R_ASSERT2	(_cnt, "Invalid portal detected");
	N.div		(float(_cnt));
	P.build		(poly[0],N);
	FPU::m24r	();

	/*
	if (_abs(1-P.n.magnitude())<EPS)
	Debug.fatal		(DEBUG_INFO,"Degenerated portal found at {%3.2f,%3.2f,%3.2f}.",VPUSH(poly[0]));
	*/
/*}

//
CSector::~CSector()
{
}

void CSector::load		(IReader& fs)
{
	// Assign portal polygons
	u32 size			= fs.find_chunk(fsP_Portals); R_ASSERT(0==(size&1));
	u32 count			= size/2;
	m_portals.reserve	(count);
	while (count) {
		u16 ID		= fs.r_u16();
		CPortal* P	= (CPortal*)RImplementation.getPortal	(ID);
		m_portals.push_back(P);
		count--;
	}

	if	(g_dedicated_server)	m_root	= 0;
	else {
		// Assign visual
		size	= fs.find_chunk(fsP_Root);	R_ASSERT(size==4);
		m_root	= RImplementation.getVisual	(fs.r_u32());
	}
}*/


IRender_Mesh::~IRender_Mesh()		
{ 
	_RELEASE(p_rm_Vertices); 
	_RELEASE(p_rm_Indices);		
}