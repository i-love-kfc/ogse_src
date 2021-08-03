#pragma once
#include "xrCommon.h"
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

namespace ALife {
	enum EHitType
	{
		eHitTypeBurn = 0x0,
		eHitTypeShock = 0x1,
		eHitTypeStrike = 0x2,
		eHitTypeWound = 0x3,
		eHitTypeRadiation = 0x4,
		eHitTypeTelepatic = 0x5,
		eHitTypeChemicalBurn = 0x6,
		eHitTypeExplosion = 0x7,
		eHitTypeFireWound = 0x8,
		eHitTypeWound_2 = 0x9,
		eHitTypePhysicStrike = 0xA,
		eHitTypeMax = 0xB,
	};
};
class CObject;
//_vector3

struct SHit
{
#pragma pack( show )
  unsigned int Time;
  unsigned __int16 PACKET_TYPE;
  unsigned __int16 DestID;
  float power;
  _vector3<float> dir;
  CObject *who;
  unsigned __int16 whoID;
  unsigned __int16 weaponID;
  unsigned __int16 boneID;
  _vector3<float> p_in_bone_space;
  __declspec(align(4)) float impulse;
  ALife::EHitType hit_type;
  float ap;
  bool aim_bullet;
  __declspec(align(4)) unsigned int BulletID;
  unsigned int SenderID;
};