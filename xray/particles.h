#pragma once

#include "kinematics.h"
#include "xr_3da.h"

struct IParticleCustom: public IRender_Visual
{
  ref_geom geom;
};

namespace PAPI
{
	struct Particle;
	struct ParticleEffect;
	struct PAHeader;
	struct ParticleAction;
	typedef xr_vector<ParticleAction*> PAVec;
}
struct EParticleAction;    

namespace PS
{
	class CParticleEffect;
	typedef BOOL ( * CollisionCallback)(CParticleEffect* E, PAPI::Particle& P, const Fvector& pt, const Fvector& norm); // TRUE-continue collision exec
	typedef void ( * DestroyCallback)	(CParticleEffect* E, PAPI::Particle& P);

	class PFunction;
	struct SFrame
	{
		Fvector2			m_fTexSize;
		Fvector2			reserved; 
		int     			m_iFrameDimX;
		int 				m_iFrameCount;
		float				m_fSpeed;
	};

	class CPEDef
	{
	public:
		enum{
			dfSprite		= (1<<0),
//			dfObject		= (1<<1),

			dfFramed		= (1<<10),
			dfAnimated		= (1<<11),
			dfRandomFrame   = (1<<12),
			dfRandomPlayback= (1<<13),
            
			dfTimeLimit		= (1<<14),

            dfAlignToPath	= (1<<15),
            dfCollision		= (1<<16),
            dfCollisionDel	= (1<<17),
            dfVelocityScale	= (1<<18),
            dfCollisionDyn	= (1<<19),
			dfWorldAlign	= (1<<20),
            dfFaceAlign		= (1<<21),
            dfCulling		= (1<<22),
            dfCullCCW		= (1<<23),
		};
		shared_str		  	m_Name;
		Flags32				m_Flags;
	// texture
		shared_str		  	m_ShaderName;
		shared_str		  	m_TextureName;
		ref_shader			m_CachedShader;
		SFrame				m_Frame;
	// compiled actions
        CMemoryWriter		m_Actions;
	// def        
		float				m_fTimeLimit;			// time limit
		int					m_MaxParticles;			// max particle count
	    Fvector				m_VelocityScale;		// velocity scale
	    Fvector				m_APDefaultRotation;	// align to path
    // collision
	    float 				m_fCollideOneMinusFriction;
        float 				m_fCollideResilience;
        float 				m_fCollideSqrCutoff; 
	};

	class CParticleEffect: public IParticleCustom
	{
		friend class CPEDef;
	protected:
		CPEDef*				m_Def;

		float				m_fElapsedLimit;

		int					m_HandleEffect;
		int					m_HandleActionList;

		s32					m_MemDT;

		Fvector				m_InitialPosition;
	public:
        Fmatrix				m_XFORM;
    protected:
    	DestroyCallback		m_DestroyCallback;
        CollisionCallback	m_CollisionCallback;
	public:
		enum{
			flRT_Playing		= (1<<0),
			flRT_DefferedStop	= (1<<1),
			flRT_XFORM			= (1<<2),
		};
		Flags8				m_RT_Flags;
	};

	class CPGDef
	{
	public:
		shared_str			m_Name;
		Flags32				m_Flags;
		float				m_fTimeLimit;
		struct SEffect{
			enum{
				flDefferedStop		= (1<<0),
				flOnPlayChild		= (1<<1),
				flEnabled			= (1<<2),
                flOnPlayChildRewind	= (1<<4),
                flOnBirthChild		= (1<<5),
				flOnDeadChild		= (1<<6),
			};
			Flags32			m_Flags;
			shared_str		m_EffectName;  
			shared_str		m_OnPlayChildName;
			shared_str		m_OnBirthChildName;
			shared_str		m_OnDeadChildName;
			float			m_Time0;
			float			m_Time1;
		};
		typedef xr_vector<SEffect*> EffectVec;
		EffectVec			m_Effects;
	};

	class CParticleGroup: public IParticleCustom
	{
		const CPGDef*		m_Def;
		float				m_CurrentTime;
		Fvector				m_InitialPosition;
	public:
		typedef xr_vector<IRender_Visual*> VisualVec;
    	struct SItem		{
        	IRender_Visual*	_effect;
            VisualVec		_children_related;
            VisualVec		_children_free;
        };
		typedef xr_vector<SItem> SItemVec;
		SItemVec			items;
	public:
		enum{
			flRT_Playing		= (1<<0),
			flRT_DefferedStop	= (1<<1),
		};
		Flags8				m_RT_Flags;
	};

	typedef xr_vector<PS::CPEDef*> PEDVec;
	typedef xr_vector<PS::CPGDef*> PGDVec;
}

class CPSLibrary	
{
    PS::PEDVec			m_PEDs;
    PS::PGDVec			m_PGDs;
public:
	void				OnCreate		();

	// wrappers
	void				w_OnCreate		();
};
inline void CPSLibrary::w_OnCreate		()
{
	__asm
	{
		mov		ebx, ecx;
		call    CPSLibrary::OnCreate;
	};
};