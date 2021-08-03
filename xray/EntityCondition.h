#pragma once
//#include <vector>
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "base_types.h"

namespace ALife
{
	enum EHitType
	{
		eHitTypeBurn,
		eHitTypeShock,
		eHitTypeStrike,
		eHitTypeWound,
		eHitTypeRadiation,
		eHitTypeTelepatic,
		eHitTypeChemicalBurn,
		eHitTypeExplosion,
		eHitTypeFireWound,
		eHitTypeWound_2,
		eHitTypePhysicStrike,
		eHitTypeMax,
	};
};
namespace HitImmunity
{
	typedef	svector<float,ALife::eHitTypeMax> HitTypeSVec;
};


struct CHitImmunity
{
	HitImmunity::HitTypeSVec m_HitTypeK;
};
struct CEntityConditionSimple
{
	float					m_fHealth;
	float					m_fHealthMax;
};
class CEntityAlive;
class CObject;
class CWound;
typedef xr_vector<CWound*> WOUND_VECTOR;

struct CEntityCondition: public CEntityConditionSimple, public CHitImmunity
{
	bool					m_use_limping_state;
	CEntityAlive			*m_object;

	WOUND_VECTOR			m_WoundVector;
	float m_fPower;
	float m_fRadiation;
	float m_fPsyHealth;
	float m_fEntityMorale;
	float m_fPowerMax;
	float m_fRadiationMax;
	float m_fPsyHealthMax;
	float m_fEntityMoraleMax;
	float m_fDeltaHealth;
	float m_fDeltaPower;
	float m_fDeltaRadiation;
	float m_fDeltaPsyHealth;
	float m_fDeltaCircumspection;
	float m_fDeltaEntityMorale;

	struct SConditionChangeV
	{
		float			m_fV_Radiation;
		float			m_fV_PsyHealth;
		float			m_fV_Circumspection;
		float			m_fV_EntityMorale;
		float			m_fV_RadiationHealth;
		float			m_fV_Bleeding;
		float			m_fV_WoundIncarnation;
		float			m_fV_HealthRestore;
	} m_change_v;

	float				m_fMinWoundSize;
	bool				m_bIsBleeding;
	float				m_fHealthHitPart;
	float				m_fPowerHitPart;
	float				m_fHealthLost;
	__int64				m_iLastTimeCalled;
	float				m_fDeltaTime;
	CObject*			m_pWho;
	unsigned short		m_iWhoID;
	float				m_fHitBoneScale;
	float				m_fWoundBoneScale;
	float				m_limping_threshold;
	bool				m_bTimeValid;
	bool				m_bCanBeHarmed;
	//
	//void UpdateConditionTime(void);
};
