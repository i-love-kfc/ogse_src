#include "r2.h"
#include "r_occlusion.h"
#include "xr_render_console.h"

void CHOM::Disable		()
{
	bEnabled			= FALSE;
}
void	R_occlusion::occq_create	(u32	limit	)
{
	pool.reserve	(limit);
	used.reserve	(limit);
	fids.reserve	(limit);
	for (u32 it=0; it<limit; it++)	{
		_Q	q;	q.order	= it;
		if	(FAILED(HW.pDevice->CreateQuery(D3DQUERYTYPE_OCCLUSION,&q.Q)))	break;
		pool.push_back	(q);
	}
	std::reverse	(pool.begin(), pool.end());
}
void	R_occlusion::occq_destroy	(				)
{
	while	(!used.empty())	{
		_RELEASE(used.back().Q);
		used.pop_back	();
	}
	while	(!pool.empty())	{
		_RELEASE(pool.back().Q);
		pool.pop_back	();
	}
	used.clear	();
	pool.clear	();
	fids.clear	();
}
u32		R_occlusion::occq_get		(u32&	ID		)
{
	if (!enabled)		return 0xffffffff;

	DWORD	fragments	= 0;
	HRESULT hr;
	// CHK_DX		(used[ID].Q->GetData(&fragments,sizeof(fragments),D3DGETDATA_FLUSH));
	// Msg			("get  : [%2d] - %d => %d", used[ID].order, ID, fragments);
	CTimer	T;
	T.Start	();
	Device.Statistic->RenderDUMP_Wait.Begin	();
	while	((hr=used[ID].Q->GetData(&fragments,sizeof(fragments),D3DGETDATA_FLUSH))==S_FALSE) {
		if (!SwitchToThread())			Sleep(ps_r2_wait_sleep);
		if (T.GetElapsed_ms() > 500)	{
			fragments	= 0xffffffff;
			break;
		}
	}
	Device.Statistic->RenderDUMP_Wait.End	();
	if		(hr == D3DERR_DEVICELOST)	fragments = 0xffffffff;

	if (0==fragments)	RImplementation.stats.o_culled	++;

	// insert into pool (sorting in decreasing order)
	_Q&		Q			= used[ID];
	if (pool.empty())	pool.push_back(Q);
	else	{
		int		it		= int(pool.size())-1;
		while	((it>=0) && (pool[it].order < Q.order))	it--;
		pool.insert		(pool.begin()+it+1,Q);
	}

	// remove from used and shrink as nescessary
	used[ID].Q			= 0;
	fids.push_back		(ID);
	ID					= 0;
	return	fragments;
}