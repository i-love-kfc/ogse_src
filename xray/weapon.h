#include "xrCommon.h"
#pragma once

	enum EWeaponStates {
		eIdle		= 0,
		eFire,
		eFire2,
		eReload,
		eShowing,
		eHiding,
		eHidden,
		eMisfire,
		eMagEmpty,
		eSwitch,
	};

#pragma pack(push, 1)
struct __declspec(align(8)) CWeapon
{
	char dummy[0x28C];
	u32	m_state;
	char dummy2[0x164];
	bool m_bZoomMode;
};
#pragma pack(pop)
