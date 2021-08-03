#include "EntityCondition.h"
/*
void CEntityCondition::UpdateConditionTime()
{
  CMapLocationWrapper *v2; // ecx@1
  unsigned __int64 v3; // qax@2
  unsigned int v4; // edi@5
  unsigned int v5; // ecx@5
  __int64 v6; // edx@6
  unsigned __int64 v7; // kr00_8@6
  __int128 v8; // xmm0@7

  v2 = g_pGameLevel[4468].m_locations;
  if ( v2[2].baseclass_0.vfptr == (CALifeRegistryWrapperObject<CALifeRegistryWrapper<CMapLocationRegistry> >Vtbl *)1 )
    LODWORD(v3) = ((int (*)(void))v2->baseclass_0.vfptr[7].__vecDelDtor)();
  else
    v3 = (unsigned int)IPureClient::timeServer(&g_pGameLevel[88]);
  if ( this->m_bTimeValid )
  {
    v4 = HIDWORD(this->m_iLastTimeCalled);
    v5 = LODWORD(this->m_iLastTimeCalled);
    if ( v3 > __PAIR__(v4, v5) )
    {
      v7 = v3 - __PAIR__(v4, v5);
      HIDWORD(v6) = (v3 - __PAIR__(v4, v5)) >> 32;
      this->m_iLastTimeCalled = v3;
      this->m_fDeltaTime = ((double)(signed __int64)__PAIR__(HIDWORD(v6) & 0x7FFFFFFF, (unsigned int)v7)
                        - (double)(signed __int64)(v6 & 0x8000000000000000ui64))
                       * 0.001;
      return;
    }
    v8 = 0;
  }
  else
  {
	  this->m_fDeltaTime = 0;
    this->m_bTimeValid = true;
    LODWORD(this->m_fDeltaHealth) = 0;
    LODWORD(this->m_fDeltaPower) = 0;
    LODWORD(this->m_fDeltaRadiation) = 0;
    LODWORD(this->m_fDeltaCircumspection) = 0;
    LODWORD(this->m_fDeltaEntityMorale) = 0;
  }
  LODWORD(this->m_fDeltaTime) = v8;
  this->m_iLastTimeCalled = v3;
}
*/