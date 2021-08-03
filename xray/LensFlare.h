#pragma once
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#include "windows.h"
#include "IniFile.h"
#include "shader.h"
#include "xrCommon.h"
#include "xr_collide.h"

class /*__declspec(dllimport)*/ CLensFlareDescriptor
{
public:
	struct SFlare
	{
    	float fOpacity;
	    float fRadius;
    	float fPosition;
        shared_str texture;
        shared_str shader;
        ref_shader hShader;
    	SFlare()
			:fOpacity(0)
			,fRadius(0)
			,fPosition(0)
		{}
	};
    struct SSource: public SFlare
    {
    	BOOL ignore_color;
    };
	typedef xr_vector<CLensFlareDescriptor::SFlare,xalloc<CLensFlareDescriptor::SFlare> > Flares_Vector;
    Flares_Vector m_Flares;

	enum {
    	flFlare 		= 1,
    	flSource		= 2,
    	flGradient 		= 4,
    };
	Flags32				m_Flags;
    
	// source
    SSource				m_Source;
    
	// gradient
    SFlare				m_Gradient;

    float				m_StateBlendUpSpeed;
    float				m_StateBlendDnSpeed;
    
	void				SetGradient		(float fMaxRadius, float fOpacity, LPCSTR tex_name, LPCSTR sh_name);
    void				SetSource		(float fRadius, BOOL ign_color, LPCSTR tex_name, LPCSTR sh_name);
    void				AddFlare		(float fRadius, float fOpacity, float fPosition, LPCSTR tex_name, LPCSTR sh_name);
    ref_shader			CreateShader	(LPCSTR tex_name, LPCSTR sh_name);

	shared_str			section;
public:
    					CLensFlareDescriptor()
						{
							Log("CLensFlareDescriptor - constructor");
							m_Flags.zero();
							//section=0;
							m_StateBlendUpSpeed=m_StateBlendDnSpeed=0.1f;
						}
	//__declspec(dllexport)
    void				load				(CInifile* pIni, LPCSTR section);
	void 				OnDeviceCreate	();
	void 				OnDeviceDestroy	();
};


class /*__declspec(dllimport)*/ CLensFlare
{
public:
	enum
	{
		MAX_RAYS	= 5
	};
private:
	collide::rq_results	r_dest;
	//collide::ray_cache	m_ray_cache[MAX_RAYS];
protected:
	float				fBlend;
	u32					dwFrame;

	Fvector				vSunDir;
	Fvector				vecLight;
	Fvector				vecX, vecY, vecDir, vecAxis, vecCenter;
	BOOL				bRender;

	// variable
    Fcolor				LightColor;
	float				fGradientValue;

	ref_geom			hGeom;
	typedef xr_vector<CLensFlareDescriptor,xalloc<CLensFlareDescriptor> > LensFlareDescriptor_Vector;
    LensFlareDescriptor_Vector m_Palette;
	CLensFlareDescriptor* m_Current;

    enum LFState{
        lfsNone,
        lfsIdle,
    	lfsHide,
        lfsShow,
    };
    LFState				m_State;
    float				m_StateBlend;

public:
	collide::ray_cache m_ray_cache;
	CLensFlare ();
	virtual ~CLensFlare();

	//__declspec(dllexport)
	void OnFrame(int id);
	//__declspec(dllexport)
	void __fastcall Render(BOOL bSun, BOOL bFlares, BOOL bGradient);
	void OnDeviceCreate();
	void OnDeviceDestroy();

	int AppendDef(/*const */CInifile* pIni, char const *sect);
	void Invalidate();
};

struct STranspParam
{
	_vector3<float> P;
	_vector3<float> D;
	float f;
	CLensFlare *parent;
	collide::ray_cache*	pray_cache;
	float vis;
	float vis_threshold;
	STranspParam(
		CLensFlare *p, 
		const Fvector& _P, 
		const Fvector& _D, 
		float _f, 
		float _vis_threshold)
		:P(_P),
		D(_D),
		f(_f),
		parent(p),
		vis(1.f),
		vis_threshold(_vis_threshold)
	{
	}
	STranspParam		(collide::ray_cache	*p, const Fvector& _P, const Fvector& _D, float _f, float _vis_threshold):P(_P),D(_D),f(_f),pray_cache(p),vis(1.f),vis_threshold(_vis_threshold){}
};

struct STranspParamFlareLamp
{
	_vector3<float> P;
	_vector3<float> D;
	float f;
	collide::ray_cache*	pray_cache;
	float vis;
	float vis_threshold;
	STranspParamFlareLamp		(collide::ray_cache	*p, const Fvector& _P, const Fvector& _D, float _f, float _vis_threshold):P(_P),D(_D),f(_f),pray_cache(p),vis(1.f),vis_threshold(_vis_threshold){}
};

inline void	blend_lerp(float& cur, float tgt, float speed, float dt);