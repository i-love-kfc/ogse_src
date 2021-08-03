#include "models.h"
/*
motions_container::motions_container()
{
}

motions_container::~motions_container()
{
	VERIFY	(container.empty());
}*/

inline CKinematics* PCKinematics		(IRender_Visual* V)		{ return V?(CKinematics*)V->dcast_PKinematics():0; }

CModelPool::CModelPool()
{
	bLogging				= TRUE;
    bForceDiscard 			= FALSE;
    bAllowChildrenDuplicate	= TRUE; 
	g_pMotionsContainer		= xr_new<motions_container>();
}
/*
void	CModelPool::Discard	(IRender_Visual* &V, BOOL b_complete)
{
	//
	REGISTRY_IT	it		= Registry.find	(V);
	if (it!=Registry.end())
	{
		// Pool - OK

			// Base
			const shared_str&	name	= it->second;
			xr_vector<ModelDef>::iterator I = Models.begin();
			xr_vector<ModelDef>::iterator I_e = Models.end();
			
			for (; I!=I_e; ++I)
			{
				if (I->name==name)
				{
					if(b_complete || strchr(*name,'#'))
					{
						VERIFY(I->refs>0);
            			I->refs--; 
						if (0==I->refs)
						{
                			bForceDiscard		= TRUE;
	            			I->model->Release	();
							xr_delete			(I->model);	
							Models.erase		(I);
							bForceDiscard		= FALSE;
						}
						break;
					}else{
					if(I->refs>0)
						I->refs--;
					break;
					}
				}
			}
		// Registry
		xr_delete		(V);	
//.		xr_free			(name);
		Registry.erase	(it);
	} else {
		// Registry entry not-found - just special type of visual / particles / etc.
		xr_delete		(V);
	}
	V	=	NULL;
}

extern __declspec(dllimport) BOOL				g_bRendering; 
void	CModelPool::DeleteInternal	(IRender_Visual* &V, BOOL bDiscard)
{
	VERIFY					(!g_bRendering);
    if (!V)					return;
	V->Depart				();
	if (bDiscard||bForceDiscard){
    	Discard	(V, TRUE); 
	}else{
		//
		REGISTRY_IT	it		= Registry.find	(V);
		if (it!=Registry.end())
		{
			// Registry entry found - move it to pool
			Pool.insert			(mk_pair(it->second,V));
		} else {
			// Registry entry not-found - just special type of visual / particles / etc.
			xr_delete			(V);
		}
	}
	V	=	NULL;
}
void CModelPool::Destroy()
{
	// Pool
	Pool.clear			();

	// Registry
	while(!Registry.empty()){
		REGISTRY_IT it	= Registry.begin();
		IRender_Visual* V=(IRender_Visual*)it->first;
		DeleteInternal	(V,TRUE);
	}

	// Base/Reference
	xr_vector<ModelDef>::iterator	I = Models.begin();
	xr_vector<ModelDef>::iterator	E = Models.end();
	for (; I!=E; I++)
	{
		I->model->Release();
		xr_delete(I->model);
	}
	
	Models.clear();

	// cleanup motions container
	g_pMotionsContainer->clean(false);
}
/*void CModelPool::dump()
{
	Log	("--- model pool --- begin:");
	u32 sz					= 0;
	u32 k					= 0;
	for (xr_vector<ModelDef>::iterator I=Models.begin(); I!=Models.end(); I++) {
		CKinematics* K		= PCKinematics(I->model);
		if (K){
			u32 cur			= K->mem_usage	(false);
			sz				+= cur;
			Msg("#%3d: [%3d/%5d Kb] - %s",k++,I->refs,cur/1024,I->name.c_str());
		}
	}
	Msg ("--- models: %d, mem usage: %d Kb ",k,sz/1024);
	sz						= 0;
	k						= 0;
	int free_cnt			= 0;
	for (REGISTRY_IT it=Registry.begin(); it!=Registry.end(); it++)
	{
		CKinematics* K		= PCKinematics((IRender_Visual*)it->first);
		VERIFY				(K);
		if (K){
			u32 cur			= K->mem_usage	(true);
			sz				+= cur;
			bool b_free		= (Pool.find(it->second)!=Pool.end() );
			if(b_free)		++free_cnt;
			Msg("#%3d: [%s] [%5d Kb] - %s",k++, (b_free)?"free":"used", cur/1024,it->second.c_str());
		}
	}
	Msg ("--- instances: %d, free %d, mem usage: %d Kb ",k, free_cnt, sz/1024);
	Log	("--- model pool --- end.");
}*/
CModelPool::~CModelPool()
{
	w_Destroy					();
	xr_delete				(g_pMotionsContainer);
}
