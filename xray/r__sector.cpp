#include "Render.h"
#include "xr_3da.h"
#include "r2.h"
#include "game_level.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
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
CSector::~CSector()
{
}

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
}

void CSector::load		(IReader& fs)
{
	// Assign portal polygons
	u32 size			= fs.find_chunk(fsP_Portals); R_ASSERT(0==(size&1));
	u32 count			= size/2;
	m_portals.reserve	(count);
	while (count) {
		u16 ID		= fs.r_u16();
		CPortal* P	= (CPortal*)RImplementation->getPortal	(ID);
		m_portals.push_back(P);
		count--;
	}

	if	(g_dedicated_server)	m_root	= 0;
	else {
		// Assign visual
		size	= fs.find_chunk(fsP_Root);	R_ASSERT(size==4);
		m_root	= RImplementation->getVisual	(fs.r_u32());
	}
}
