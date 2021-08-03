#pragma once
#include "FastDelegate.h"

class CObject;

struct CObjectList
{
	template class std::map<unsigned int,CObject *,std::less<unsigned int>,xalloc<std::pair<unsigned int,CObject *> > >;
	xr_map<unsigned int,CObject *> map_NETID;
	xr_vector<CObject *> destroy_queue;
	xr_vector<CObject *> objects_active;
	xr_vector<CObject *> objects_sleeping;
	xr_vector<CObject *> crows_0;
	xr_vector<CObject *> crows_1;
	xr_vector<CObject *> *crows;
	CObject **objects_dup;
	unsigned int objects_dup_memsz;
	struct SRelcasePair
	{
		template class fastdelegate::FastDelegate1<CObject *,void>;
		int *m_ID;
		fastdelegate::FastDelegate1<CObject *,void> m_Callback;
	};
	template class std::vector<SRelcasePair, xalloc<SRelcasePair>>;
	xr_vector<SRelcasePair> m_relcase_callbacks;
};
