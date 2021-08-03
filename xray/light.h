#pragma once

#pragma warning(disable:4237)	// shut up export warning

#include "Render.h"
#include "object.h"

struct light_indirect;
class light_Package;

class	light		:	public IRender_Light, public ISpatial
{
public:
	struct {
		u32			type	:	4;
		u32			bStatic	:	1;
		u32			bActive	:	1;
		u32			bShadow	:	1;
	}				flags;
	Fvector			position	;
	Fvector			direction	;
	Fvector			right		;
	float			range		;
	float			cone		;
	Fcolor			color		;

	vis_data		hom			;
	u32				frame_render;

	light*						omnipart	[6]	;
	xr_vector<light_indirect>	indirect		;
	u32							indirect_photons;

	smapvis			svis;		// used for 6-cubemap faces

	ref_shader		s_spot;
	ref_shader		s_point;

	u32				m_xform_frame;
	Fmatrix			m_xform;

	struct _vis		{
		u32			frame2test;		// frame the test is sheduled to
		u32			query_id;		// ID of occlusion query
		u32			query_order;	// order of occlusion query
		bool		visible;		// visible/invisible
		bool		pending;		// test is still pending
		u16			smap_ID;
	}				vis;

	union			_xform	{
		struct		_D		{
			Fmatrix						combine	;
			s32							minX,maxX	;
			s32							minY,maxY	;
			BOOL						transluent	;
		}	D;
		struct		_P		{
			Fmatrix						world		;
			Fmatrix						view		;
			Fmatrix						project		;
			Fmatrix						combine		;
		}	P;
		struct		_S		{
			Fmatrix						view		;
			Fmatrix						project		;
			Fmatrix						combine		;
			u32							size		;
			u32							posX		;
			u32							posY		;
			BOOL						transluent	;
		}	S;
	}	X;
	float	smap_near;
	float	smap_near_neg;
	float	lsf_speed;
	float	lsf_amount;
	float	lsf_smap_jitter;
	u32		is_require_flare;
	u32		is_volumetric;
	float	fBlend;
	u32		is_actor_torch;

public:
	virtual void	set_type				(LT type)						{ flags.type = type;		}
	virtual void	set_active				(bool b);
	virtual bool	get_active				()								{ return flags.bActive;		}
	virtual void	set_shadow				(bool b)						
	{ 
		flags.bShadow=b;			
	}
	virtual void	set_position			(const Fvector& P);
	virtual void	set_rotation			(const Fvector& D, const Fvector& R);
	virtual void	set_cone				(float angle);
	virtual void	set_range				(float R);
	virtual void	set_virtual_size		(float R)						{};
	virtual void	set_color				(const Fcolor& C)				{ color.set(C);				}
	virtual void	set_color				(float r, float g, float b)		{ color.set(r,g,b,1);		}
	virtual void	set_texture				(LPCSTR name);

	virtual	void	spatial_move			();
	virtual	Fvector	spatial_sector_point	();

	virtual IRender_Light*	dcast_Light		()	{ return this; }

/*	vis_data&		get_homdata				();*/

	void			gi_generate				();
	void			xform_calc				();
/*	void			vis_prepare				();
	void			vis_update				();*/
//	void			export 					(light_Package& dest);

	float			get_LOD					();

	light();
	virtual ~light();
};

class light_Package
{
public:
  xr_vector<light *> v_point;
  xr_vector<light *> v_spot;
  xr_vector<light *> v_shadowed;

  void	clear();
};
struct	light_indirect		{
	Fvector			P;
	Fvector			D;
	float			E;
	IRender_Sector*	S;
};
class	CLight_DB
{
private:
	xr_vector<ref_light>	v_static;
public:
	ref_light				sun_original;
	ref_light				sun_adapted;
	light_Package			package;

	void					add_light			(light*		L	);
	void					Update				();
	void					Load				(IReader*	fs	);
	void					Unload				();

	light*					Create				();

	// wrappers
//	void	__thiscall		w_Load				(IReader*	fs	);
};

struct	CLight_Compute_XFORM_and_VIS
{
	void	compute_xf_spot		(light* L);		// 
};
/*
inline void	/*__declspec(naked)*//* CLight_DB::w_Load				(IReader*	fs	)
{
	__asm
	{
		mov		eax, [esp+8];
		push	eax;
		mov		ebx, ecx;
		call	CLight_DB::Load;
	};
};*/