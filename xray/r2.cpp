#include "r2_rendertarget.h"
#include "DetailManager.h"
#include "r2.h"
#include "xr_render_console.h"
#include "pureRender.h"
#include "RenderDevice.h"
#include "xrCore.h"
#include "Backend.h"
#include "WallmarksEngine.h"

#ifdef _WIN7_COMPILER
#include "proxy_d3dcompiler.h"
#endif

#ifdef	D3DXSHADER_USE_LEGACY_D3DX9_31_DLL
#undef D3DXSHADER_USE_LEGACY_D3DX9_31_DLL
#endif

#ifndef _OGSE_DEBUG
#define USE_SHADER_CACHE
#endif

IRender_Target*			CRender::getTarget					() {return Target;};

extern __declspec(dllimport) BOOL r2_sun_static;
static class cl_parallax		: public R_constant_setup		{	virtual void setup	(R_constant* C)
{
	float			h			=	ps_r2_df_parallax_h;
	RCache.set_c	(C,h,-h/2.f,1.f/(r_dtex_range),1.f/(r_dtex_range));
}}	binder_parallax;
/*
void CRender::render_forward()
{
	__asm
	{
		push	edx;
		push	esi;
		mov		esi, ecx;
		mov		edx, BASE_ADDRESS_XRRENDER_R2;
		add		edx, ADDR_CRENDER_RENDER_FORWARD;
		call	edx;									// void CRender::render_forward
		pop		esi;
		pop		edx;
	};
}*/
void CRender::create()
{
	Device.seqFrame.Add	(this,REG_PRIORITY_HIGH+0x12345678);
/*
	// отладка для Gun12
	D3DCAPS9					_caps	;
	HW.pD3D->GetDeviceCaps		(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&_caps);
	Msg("VIDEO ADAPTER CAPABILITIES");
	Msg("DeviceType: %d", _caps.DeviceType);
	Msg("AdapterOrdinal: %d", _caps.AdapterOrdinal);
	Msg("Caps: %d", _caps.Caps);
	Msg("Caps2: %d", _caps.Caps2);
	Msg("Caps3: %d", _caps.Caps3);
	Msg("PresentationIntervals: %d", _caps.PresentationIntervals);
	Msg("CursorCaps: %d", _caps.CursorCaps);
	Msg("DevCaps: %d", _caps.DevCaps);
	Msg("PrimitiveMiscCaps: %d", _caps.PrimitiveMiscCaps);
	Msg("RasterCaps: %d", _caps.RasterCaps);
	Msg("ZCmpCaps: %d", _caps.ZCmpCaps);
	Msg("SrcBlendCaps: %d", _caps.SrcBlendCaps);
	Msg("DestBlendCaps: %d", _caps.DestBlendCaps);
	Msg("AlphaCmpCaps: %d", _caps.AlphaCmpCaps);
	Msg("ShadeCaps: %d", _caps.ShadeCaps);
	Msg("TextureCaps: %d", _caps.TextureCaps);
	Msg("TextureFilterCaps: %d", _caps.TextureFilterCaps);
	Msg("CubeTextureFilterCaps: %d", _caps.CubeTextureFilterCaps);
	Msg("VolumeTextureFilterCaps: %d", _caps.VolumeTextureFilterCaps);
	Msg("TextureAddressCaps: %d", _caps.TextureAddressCaps);
	Msg("VolumeTextureAddressCaps: %d", _caps.VolumeTextureAddressCaps);
	Msg("LineCaps: %d", _caps.LineCaps);
	Msg("MaxTextureWidth: %d", _caps.MaxTextureWidth);
	Msg("MaxTextureHeight: %d", _caps.MaxTextureHeight);
	Msg("MaxVolumeExtent: %d", _caps.MaxVolumeExtent);
	Msg("MaxTextureRepeat: %d", _caps.MaxTextureRepeat);
	Msg("MaxTextureAspectRatio: %d", _caps.MaxTextureAspectRatio);
	Msg("MaxAnisotropy: %d", _caps.MaxAnisotropy);
	Msg("MaxVertexW: %f", _caps.MaxVertexW);
	Msg("GuardBandLeft: %f", _caps.GuardBandLeft);
	Msg("GuardBandTop: %f", _caps.GuardBandTop);
	Msg("GuardBandRight: %f", _caps.GuardBandRight);
	Msg("GuardBandBottom: %f", _caps.GuardBandBottom);
	Msg("ExtentsAdjust: %f", _caps.ExtentsAdjust);
	Msg("StencilCaps: %d", _caps.StencilCaps);
	Msg("FVFCaps: %d", _caps.FVFCaps);
	Msg("TextureOpCaps: %d", _caps.TextureOpCaps);
	Msg("MaxTextureBlendStages: %d", _caps.MaxTextureBlendStages);
	Msg("MaxSimultaneousTextures: %d", _caps.MaxSimultaneousTextures);
	Msg("VertexProcessingCaps: %d", _caps.VertexProcessingCaps);
	Msg("MaxActiveLights: %d", _caps.MaxActiveLights);
	Msg("MaxUserClipPlanes: %d", _caps.MaxUserClipPlanes);
	Msg("MaxVertexBlendMatrices: %d", _caps.MaxVertexBlendMatrices);
	Msg("MaxVertexBlendMatrixIndex: %d", _caps.MaxVertexBlendMatrixIndex);
	Msg("MaxPointSize: %f", _caps.MaxPointSize);
	Msg("MaxPrimitiveCount: %d", _caps.MaxPrimitiveCount);
	Msg("MaxVertexIndex: %d", _caps.MaxVertexIndex);
	Msg("MaxStreams: %d", _caps.MaxStreams);
	Msg("MaxStreamStride: %d", _caps.MaxStreamStride);
	Msg("VertexShaderVersion: %d", _caps.VertexShaderVersion);
	Msg("MaxVertexShaderConst: %d", _caps.MaxVertexShaderConst);
	Msg("PixelShaderVersion: %d", _caps.PixelShaderVersion);
	Msg("PixelShader1xMaxValue: %f", _caps.PixelShader1xMaxValue);
	Msg("DevCaps2: %d", _caps.DevCaps2);
	Msg("MasterAdapterOrdinal: %d", _caps.MasterAdapterOrdinal);
	Msg("AdapterOrdinalInGroup: %d", _caps.AdapterOrdinalInGroup);
	Msg("NumberOfAdaptersInGroup: %d", _caps.NumberOfAdaptersInGroup);
	Msg("DeclTypes: %d", _caps.DeclTypes);
	Msg("NumSimultaneousRTs: %d", _caps.NumSimultaneousRTs);
	Msg("StretchRectFilterCaps: %d", _caps.StretchRectFilterCaps);
	Msg("VS20Caps.Caps: %d", _caps.VS20Caps.Caps);
	Msg("VS20Caps.DynamicFlowControlDepth: %d", _caps.VS20Caps.DynamicFlowControlDepth);
	Msg("VS20Caps.NumTemps: %d", _caps.VS20Caps.NumTemps);
	Msg("VS20Caps.StaticFlowControlDepth: %d", _caps.VS20Caps.StaticFlowControlDepth);
	Msg("PS20Caps.Caps: %d", _caps.PS20Caps.Caps);
	Msg("PS20Caps.DynamicFlowControlDepth: %d", _caps.PS20Caps.DynamicFlowControlDepth);
	Msg("PS20Caps.NumTemps: %d", _caps.PS20Caps.NumTemps);
	Msg("PS20Caps.StaticFlowControlDepth: %d", _caps.PS20Caps.StaticFlowControlDepth);
	Msg("VertexTextureFilterCaps: %d", _caps.VertexTextureFilterCaps);
	Msg("MaxVShaderInstructionsExecuted: %d", _caps.MaxVShaderInstructionsExecuted);
	Msg("MaxPShaderInstructionsExecuted: %d", _caps.MaxPShaderInstructionsExecuted);
	Msg("MaxVertexShader30InstructionSlots: %d", _caps.MaxVertexShader30InstructionSlots);
	Msg("MaxPixelShader30InstructionSlots: %d", _caps.MaxPixelShader30InstructionSlots);
*/
	m_skinning			= -1;

	// hardware
	o.smapsize			= 2048;
	o.mrt				= (HW.Caps.raster.dwMRT_count >= 3);
	o.mrtmixdepth		= (HW.Caps.raster.b_MRT_mixdepth);

	// Check for NULL render target support
	D3DFORMAT	nullrt	= (D3DFORMAT)MAKEFOURCC('N','U','L','L');
	o.nullrt			= HW.support	(nullrt,			D3DRTYPE_SURFACE, D3DUSAGE_RENDERTARGET);
	/*
	if (o.nullrt)		{
	Msg				("* NULLRT supported and used");
	};
	*/
	if (o.nullrt)		{
		Msg				("* NULLRT supported");

		//.	    _tzset			();
		//.		??? _strdate	( date, 128 );	???
		//.		??? if (date < 22-march-07)		
		if (0)
		{
			u32 device_id	= HW.Caps.id_device;
			bool disable_nullrt = false;
			switch (device_id)	
			{
			case 0x190:
			case 0x191:
			case 0x192:
			case 0x193:
			case 0x194:
			case 0x197:
			case 0x19D:
			case 0x19E:{
				disable_nullrt = true;	//G80
				break;
					   }
			case 0x400:
			case 0x401:
			case 0x402:
			case 0x403:
			case 0x404:
			case 0x405:
			case 0x40E:
			case 0x40F:{
				disable_nullrt = true;	//G84
				break;
					   }
			case 0x420:
			case 0x421:
			case 0x422:
			case 0x423:
			case 0x424:
			case 0x42D:
			case 0x42E:
			case 0x42F:{
				disable_nullrt = true;	// G86
				break;
					   }
			}
			if (disable_nullrt)	o.nullrt=false;
		};
		if (o.nullrt)	Msg				("* ...and used");
	};


	// SMAP / DST
	o.HW_smap_FETCH4	= FALSE;
	o.HW_smap			= HW.support	(D3DFMT_D24X8,			D3DRTYPE_TEXTURE,D3DUSAGE_DEPTHSTENCIL);
	o.HW_smap_PCF		= o.HW_smap		;
	if (o.HW_smap)		{
		o.HW_smap_FORMAT	= D3DFMT_D24X8;
		Msg				("* HWDST/PCF supported and used");
	}

	o.fp16_filter		= HW.support	(D3DFMT_A16B16G16R16F,	D3DRTYPE_TEXTURE,D3DUSAGE_QUERY_FILTER);
	o.fp16_blend		= HW.support	(D3DFMT_A16B16G16R16F,	D3DRTYPE_TEXTURE,D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING);

	// search for ATI formats
	if (!o.HW_smap && (0==strstr(Core.Params,"-nodf24")) )		{
		o.HW_smap		= HW.support	((D3DFORMAT)(MAKEFOURCC('D','F','2','4')),	D3DRTYPE_TEXTURE,D3DUSAGE_DEPTHSTENCIL);
		if (o.HW_smap)	{
			o.HW_smap_FORMAT= MAKEFOURCC	('D','F','2','4');
			o.HW_smap_PCF	= FALSE			;
			o.HW_smap_FETCH4= TRUE			;
		}
		Msg				("* DF24/F4 supported and used [%X]", o.HW_smap_FORMAT);
	}

	// emulate ATI-R4xx series
	if (strstr(Core.Params,"-r4xx"))	{
		o.mrtmixdepth	= FALSE;
		o.HW_smap		= FALSE;
		o.HW_smap_PCF	= FALSE;
		o.fp16_filter	= FALSE;
		o.fp16_blend	= FALSE;
	}

	VERIFY2				(o.mrt && (HW.Caps.raster.dwInstructions>=256),"Hardware doesn't meet minimum feature-level");
	if (o.mrtmixdepth)		o.albedo_wo		= FALSE	;
	else if (o.fp16_blend)	o.albedo_wo		= FALSE	;
	else					o.albedo_wo		= TRUE	;

	// nvstencil on NV40 and up
	o.nvstencil			= FALSE;
	// this hack leads to strange bugs, so it is disabled.
/*	if ((HW.Caps.id_vendor==0x10DE)&&(HW.Caps.id_device>=0x40))	o.nvstencil = TRUE;
	if (strstr(Core.Params,"-nonvs"))		o.nvstencil	= FALSE;*/

	// nv-dbt
	o.nvdbt				= HW.support	((D3DFORMAT)MAKEFOURCC('N','V','D','B'), D3DRTYPE_SURFACE, 0);
	if (o.nvdbt)		Msg	("* NV-DBT supported and used");

	// options (smap-pool-size)
	if (strstr(Core.Params,"-smap1536"))	o.smapsize	= 1536;
	if (strstr(Core.Params,"-smap2048"))	o.smapsize	= 2048;
	if (strstr(Core.Params,"-smap2560"))	o.smapsize	= 2560;
	if (strstr(Core.Params,"-smap3072"))	o.smapsize	= 3072;
	if (strstr(Core.Params,"-smap4096"))	o.smapsize	= 4096;

	D3DCAPS9 caps;
	CHK_DX(HW.pDevice->GetDeviceCaps(&caps));
	u32 video_mem = HW.pDevice->GetAvailableTextureMem();

	if (strstr(Core.Params,"-smap8192"))
	{
		if ((caps.MaxTextureHeight >= 8192) &&
		(video_mem > 512))
		{
			o.smapsize	= 8192;
		} else {
			Msg("* WARNING!!! Your videoadapter doesn't support smap resolution to 8192x8192");
			o.smapsize	= 4096;
		}
	}

	Msg("Shadow Map resolution: %dx%d", o.smapsize, o.smapsize);
	
	// gloss
	char*	g			= strstr(Core.Params,"-gloss ");
	o.forcegloss		= g?	TRUE	:FALSE	;
	if (g)				{
		o.forcegloss_v		= float	(atoi	(g+xr_strlen("-gloss ")))/255.f;
	}

	// get version of OS
	DWORD dwMajorVersion = 0;
	DWORD dwVersion = 0;
	dwVersion = GetVersion();

	dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));

	if (dwMajorVersion > 5)
		Msg("OS VERSION: VISTA OR ABOVE");
	else 
		Msg("OS VERSION: XP OR BELOW");

	// options
//	o.bug				= (strstr(Core.Params,"-bug"))?			TRUE	:FALSE	;
	o.bug				= (dwMajorVersion > 5)?					TRUE	:FALSE	;	// now this is a flag indicating OS version. TRUE -> Win Vista and above, FALSE -> Win XP and below
	o.sunfilter			= (strstr(Core.Params,"-sunfilter"))?	TRUE	:FALSE	;
	o.sunstatic			= r2_sun_static;
	o.sjitter			= (strstr(Core.Params,"-sjitter"))?		TRUE	:FALSE	;
	o.depth16			= (strstr(Core.Params,"-depth16"))?		TRUE	:FALSE	;
	o.noshadows			= (strstr(Core.Params,"-noshadows"))?	TRUE	:FALSE	;
	o.Tshadows			= (strstr(Core.Params,"-tsh"))?			TRUE	:FALSE	;
	o.mblur				= (strstr(Core.Params,"-mblur"))?		TRUE	:FALSE	;
	o.distortion_enabled= (strstr(Core.Params,"-nodistort"))?	FALSE	:TRUE	;
	o.distortion		= o.distortion_enabled;
	o.disasm			= (strstr(Core.Params,"-disasm"))?		TRUE	:FALSE	;
	o.forceskinw		= (strstr(Core.Params,"-skinw"))?		TRUE	:FALSE	;

	// constants
	::Device.Resources->RegisterConstantSetup	("parallax",	&binder_parallax);

	c_lmaterial					= "L_material";
	c_sbase						= "s_base";

	Target						= xr_new<CRenderTarget>		();	// Main target

	Models						= xr_new<CModelPool>		();
	PSLibrary.w_OnCreate			();

	HWOCC.occq_create			(occq_size);

	//rmNormal					();
	marker						= 0;
	R_CHK						(HW.pDevice->CreateQuery(D3DQUERYTYPE_EVENT,&q_sync_point[0]));
	R_CHK						(HW.pDevice->CreateQuery(D3DQUERYTYPE_EVENT,&q_sync_point[1]));

	xrRender_apply_tf			();
	PortalTraverser.initialize();
};
extern int reset_frame;
void CRender::reset_begin()
{
	// Update incremental shadowmap-visibility solver
	// BUG-ID: 10646
	{
		u32 it=0;
		for (it=0; it<Lights_LastFrame.size(); it++)	{
			if (0==Lights_LastFrame[it])	continue	;
			try {
				Lights_LastFrame[it]->svis.resetoccq ()	;
			} catch (...)
			{
				Msg	("! Failed to flush-OCCq on light [%d] %X",it,*(u32*)(&Lights_LastFrame[it]));
			}
		}
		Lights_LastFrame.clear	();
	}
	reset_frame = Device.dwFrame;
	xr_delete					(Target);
	if (psDeviceFlags.is(rsDetails) && b_loaded && ((dm_current_size != dm_size) || (ps_r__Detail_density	!= ps_current_detail_density)))
	{
		Details->Unload				();
		xr_delete					(Details);
	}
	HWOCC.occq_destroy			();
	_RELEASE					(q_sync_point[1]);
	_RELEASE					(q_sync_point[0]);
}
void CRender::reset_end()
{
	R_CHK						(HW.pDevice->CreateQuery(D3DQUERYTYPE_EVENT,&q_sync_point[0]));
	R_CHK						(HW.pDevice->CreateQuery(D3DQUERYTYPE_EVENT,&q_sync_point[1]));
	HWOCC.occq_create			(occq_size);

	Target						=	xr_new<CRenderTarget>	();
	if (psDeviceFlags.is(rsDetails) && b_loaded && ((dm_current_size != dm_size) || (ps_r__Detail_density	!= ps_current_detail_density)))
	{
		Details						=	xr_new<CDetailManager>	();
		Details->Load();
	}

	xrRender_apply_tf			();
};
HRESULT	CRender::shader_compile			(
	LPCSTR							name,
	LPCSTR                          pSrcData,
	UINT                            SrcDataLen,
	void*							_pDefines,
	void*							_pInclude,
	LPCSTR                          pFunctionName,
	LPCSTR                          pTarget,
	DWORD                           Flags,
	void*							_ppShader,
	void*							_ppErrorMsgs,
	void*							_ppConstantTable)
{
	D3DXMACRO						defines			[128];
	int								def_it			= 0;
	CONST D3DXMACRO*                pDefines		= (CONST D3DXMACRO*)	_pDefines;
	char							c_smapsize		[32];
	char							c_rmapsize		[32];
	char							sunshafts_quality		[32];
	char							dof_quality		[32];
	char							ssao_quality	[32];
	char							aa_quality		[32];
	char							parallax_quality	[32];
	char							soft_shadows_quality	[32];
	char							c_gloss			[32];
	char							str				[32];
	char							str1			[32];
	char							str2			[32];
	char							refl_quality			[32];
	char							refl_mode			[32];
	char							lmap_shadow_mode			[32];
	if (pDefines)	{
		// transfer existing defines
		for (;;def_it++)	{
			if (0==pDefines[def_it].Name)	break;
			defines[def_it]			= pDefines[def_it];
		}
	}
//	Msg("Compiling shader %s.%s", name, pTarget);
	// options
	{
		sprintf						(c_smapsize,"%d",u32(o.smapsize));
		defines[def_it].Name		=	"SMAP_size";
		defines[def_it].Definition	=	c_smapsize;
		def_it						++	;
	}
	if (ps_r2_test_flags.test(R2FLAG_RAIN_MAP)) {
		sprintf						(c_rmapsize,"%d",u32(rmap_size));
		defines[def_it].Name		=	"RMAP_size";
		defines[def_it].Definition	=	c_rmapsize;
		def_it						++	;

/*		sprintf						(c_rmap_dist,"%d",u32(rmap_dist));
		defines[def_it].Name		=	"RMAP_dist";
		defines[def_it].Definition	=	c_rmapsize;
		def_it						++	;*/
	}
	if (o.fp16_filter)		{
		defines[def_it].Name		=	"FP16_FILTER";
		defines[def_it].Definition	=	"1";
		def_it						++	;
	}
	if (o.fp16_blend)		{
		defines[def_it].Name		=	"FP16_BLEND";
		defines[def_it].Definition	=	"1";
		def_it						++	;
	}
	if (o.HW_smap)			{
		defines[def_it].Name		=	"USE_HWSMAP";
		defines[def_it].Definition	=	"1";
		def_it						++	;
	}
	if (o.HW_smap_PCF)			{
		defines[def_it].Name		=	"USE_HWSMAP_PCF";
		defines[def_it].Definition	=	"1";
		def_it						++	;
	}
	if (o.HW_smap_FETCH4)			{
		defines[def_it].Name		=	"USE_FETCH4";
		defines[def_it].Definition	=	"1";
		def_it						++	;
	}
	if (o.sjitter)			{
		defines[def_it].Name		=	"USE_SJITTER";
		defines[def_it].Definition	=	"1";
		def_it						++	;
	}
	if (HW.Caps.raster_major >= 3)	{
		defines[def_it].Name		=	"USE_BRANCHING";
		defines[def_it].Definition	=	"1";
		def_it						++	;
	}
	if (HW.Caps.geometry.bVTF)	{
		defines[def_it].Name		=	"USE_VTF";
		defines[def_it].Definition	=	"1";
		def_it						++	;
	}
	if (o.Tshadows)			{
		defines[def_it].Name		=	"USE_TSHADOWS";
		defines[def_it].Definition	=	"1";
		def_it						++	;
	}
	if (o.mblur || ps_r2_pp_flags.test(R2PP_FLAG_MBLUR))			{
		defines[def_it].Name		=	"USE_MBLUR";
		defines[def_it].Definition	=	"1";
		def_it						++	;
	}
	if (o.sunfilter)		{
		defines[def_it].Name		=	"USE_SUNFILTER";
		defines[def_it].Definition	=	"1";
		def_it						++	;
	}
	if (o.sunstatic)		{
		defines[def_it].Name		=	"USE_R2_STATIC_SUN";
		defines[def_it].Definition	=	"1";
		def_it						++	;
	}
	if (o.forcegloss)		{
		sprintf						(c_gloss,"%f",o.forcegloss_v);
		defines[def_it].Name		=	"FORCE_GLOSS";
		defines[def_it].Definition	=	c_gloss;
		def_it						++	;
	}
	if (o.forceskinw)		{
		defines[def_it].Name		=	"SKIN_COLOR";
		defines[def_it].Definition	=	"1";
		def_it						++;
	}
/*	if (o.nvdbt && ps_r2_ls_flags.test(R2FLAG_USE_NVDBT))			{
		defines[def_it].Name		=	"USE_NVDBT";
		defines[def_it].Definition	=	"1";
		def_it						++	;
	}*/

	// skinning
	if (m_skinning<0)		{
		defines[def_it].Name		=	"SKIN_NONE";
		defines[def_it].Definition	=	"1";
		def_it						++	;
	}
	if (0==m_skinning)		{
		defines[def_it].Name		=	"SKIN_0";
		defines[def_it].Definition	=	"1";
		def_it						++;
	}
	if (1==m_skinning)		{
		defines[def_it].Name		=	"SKIN_1";
		defines[def_it].Definition	=	"1";
		def_it						++;
	}
	if (2==m_skinning)		{
		defines[def_it].Name		=	"SKIN_2";
		defines[def_it].Definition	=	"1";
		def_it						++;
	}

	// KD
	if (ps_r2_dof_flags.test(R2FLAG_DOF_COMMON|R2FLAG_DOF_ZOOM|R2FLAG_DOF_RELOAD))		
	{
		defines[def_it].Name		=	"USE_DDOF";
		defines[def_it].Definition	=	"1";
		def_it						++;

		if (ps_r2_dof_flags.test(R2FLAG_DOF_COMMON))		
		{
			defines[def_it].Name		=	"USE_DYNAMIC_DDOF";
			defines[def_it].Definition	=	"1";
			def_it						++;
		}

		if (ps_r2_dof_flags.test(R2FLAG_DOF_ZOOM))		
		{
			defines[def_it].Name		=	"USE_ZOOM_DDOF";
			defines[def_it].Definition	=	"1";
			def_it						++;
		}

		if (ps_r2_dof_flags.test(R2FLAG_DOF_RELOAD))		
		{
			defines[def_it].Name		=	"USE_RELOAD_DDOF";
			defines[def_it].Definition	=	"1";
			def_it						++;
		}

		// quality
		sprintf						(dof_quality,"%d",ps_dof_quality);
		defines[def_it].Name		=	"DDOF_QUALITY";
		defines[def_it].Definition	=	dof_quality;
		def_it						++;
	}
	if (ps_r2_pp_flags.test(R2PP_FLAG_RAIN_DROPS))		
	{
		defines[def_it].Name		=	"USE_RAINDROPS";
		defines[def_it].Definition	=	"1";
		def_it						++;
	}
	if (ps_r2_pp_flags.test(R2PP_FLAG_SUNSHAFTS))		
	{
		defines[def_it].Name		=	"USE_SUNSHAFTS";
		defines[def_it].Definition	=	"1";
		def_it						++;

		// quality
		sprintf						(sunshafts_quality,"%d",ps_sunshafts_quality);
		defines[def_it].Name		=	"SUNSHAFTS_QUALITY";
		defines[def_it].Definition	=	sunshafts_quality;
		def_it						++;
	}
	if (ps_r2_pp_flags.test(R2PP_FLAG_SSAO))		{
		defines[def_it].Name		=	"USE_SSAO";
		defines[def_it].Definition	=	"1";
		def_it						++;

		// quality
		sprintf						(ssao_quality,"%d",ps_ssao_quality);
		defines[def_it].Name		=	"SSAO_QUALITY";
		defines[def_it].Definition	=	ssao_quality;
		def_it						++;
	}

	if (ps_r2_pp_flags.test(R2PP_FLAG_AA))		{
		// quality
		sprintf						(aa_quality,"%d",ps_aa_quality);
		defines[def_it].Name		=	"AA_QUALITY";
		defines[def_it].Definition	=	aa_quality;
		def_it						++;
	}

	if (0 != ps_soft_shadows)		{
		sprintf						(soft_shadows_quality,"%d",ps_soft_shadows);
		defines[def_it].Name		=	"SOFT_SHADOWS";
		defines[def_it].Definition	=	soft_shadows_quality;
		def_it						++;
	}
	if (0 != ps_steep_parallax)		{
		sprintf						(parallax_quality,"%d",ps_steep_parallax);
		defines[def_it].Name		=	"ALLOW_STEEP_PARALLAX";
		defines[def_it].Definition	=	parallax_quality;
		def_it						++;
	}
	if (ps_r2_ls_flags.test(R2FLAG_SOFT_WATER))		{
		defines[def_it].Name		=	"USE_SOFT_WATER";
		defines[def_it].Definition	=	"1";
		def_it						++;
	}
	if (ps_r2_ls_flags.test(R2FLAG_SOFT_PARTICLES))		{
		defines[def_it].Name		=	"USE_SOFT_PARTICLES";
		defines[def_it].Definition	=	"1";
		def_it						++;
	}
	if (ps_r2_test_flags.test(R2FLAG_HW_INSTANSING))		{
		defines[def_it].Name		=	"USE_HWI";
		defines[def_it].Definition	=	"1";
		def_it						++;
	}
	if (ps_r2_ls_flags.test(R2FLAG_DETAIL_BUMP_DEBUG))		{
		defines[def_it].Name		=	"DETAIL_BUMP_DEBUG";
		defines[def_it].Definition	=	"1";
		def_it						++;
	}
	if (ps_r2_test_flags.test(R2FLAG_REPROJECT))		{
		defines[def_it].Name		=	"SMAA_REPROJECTION";
		defines[def_it].Definition	=	"1";
		def_it						++;
	}

	if (ps_r2_test_flags.test(R2FLAG_RAIN_MAP))		{
		defines[def_it].Name		=	"USE_RAIN_MAP";
		defines[def_it].Definition	=	"1";
		def_it						++;
	}

	if (ps_r2_ls_flags.test(R2FLAG_GBUFFER_OPT))		{
		defines[def_it].Name		=	"USE_GBUFFER_OPTIMIZATION";
		defines[def_it].Definition	=	"1";
		def_it						++;
	}

	if (ps_r2_ls_flags.test(R2FLAG_LENS_FLARES))		{
		defines[def_it].Name		=	"USE_LENS_FLARES";
		defines[def_it].Definition	=	"1";
		def_it						++;
	}
	if (ps_r2_ls_flags.test(R2FLAG_LENS_DIRT))		{
		defines[def_it].Name		=	"USE_LENS_DIRT";
		defines[def_it].Definition	=	"1";
		def_it						++;
	}
	if (ps_r2_ls_flags.test(R2FLAG_AO))		{
		defines[def_it].Name		=	"USE_AO";
		defines[def_it].Definition	=	"1";
		def_it						++;
	}

	sprintf						(str,"%d",u32(ps_r2_blur_params.y));
	defines[def_it].Name		=	"NUM_OF_BLUR_SAMPLES";
	defines[def_it].Definition	=	str;
	def_it						++;

	if (ps_r2_sharpening > 0.0)				{
		sprintf						(str1,"%f",ps_r2_sharpening);
		defines[def_it].Name		=	"IMAGE_SHARPENING";
		defines[def_it].Definition	=	str1;
		def_it						++;
	}
	if (ps_r2_ao_intensity > 0.0)				{
		sprintf						(str2,"%f",ps_r2_ao_intensity);
		defines[def_it].Name		=	"AO_INTENSITY";
		defines[def_it].Definition	=	str2;
		def_it						++;
	}
	if (ps_r2_pp_flags.test(R2PP_FLAG_REFLECTIONS))		{
		defines[def_it].Name		=	"USE_REFLECTIONS";
		defines[def_it].Definition	=	"1";
		def_it						++;

/*		if (ps_r2_ls_flags.test(R2FLAG_HQ_REFLECTIONS))		{
			defines[def_it].Name		=	"USE_HQ_REFLECTIONS";
			defines[def_it].Definition	=	"1";
			def_it						++;
		}*/

		// quality
		sprintf						(refl_quality,"%d",ps_refl_quality);
		defines[def_it].Name		=	"REFLECTIONS_QUALITY";
		defines[def_it].Definition	=	refl_quality;
		def_it						++;
	}

	// where to draw
	sprintf						(refl_mode,"%d",ps_refl_mode);
	defines[def_it].Name		=	"REFLECTIONS_MODE";
	defines[def_it].Definition	=	refl_mode;
	def_it						++;

	if (ps_r2_ls_flags.test(R2FLAG_WET_SURFACES))		{
		defines[def_it].Name		=	"USE_WET_SURFACES";
		defines[def_it].Definition	=	"1";
		def_it						++;
	}
	if (ps_r2_ls_flags.test(R2FLAG_PUDDLES))		{
		defines[def_it].Name		=	"USE_PUDDLES";
		defines[def_it].Definition	=	"1";
		def_it						++;
	}
	if (ps_r2_test_flags.test(R2FLAG_PCSS))		{
		defines[def_it].Name		=	"USE_PCSS";
		defines[def_it].Definition	=	"1";
		def_it						++;
	}
	if (ps_r2_pp_flags.test(R2PP_FLAG_LIGHT_SHADOW_FADE))		{
		sprintf						(lmap_shadow_mode,"%d",ps_light_shadow_quality);
		defines[def_it].Name		=	"LAMP_SHADOW_FADE_QUALITY";
		defines[def_it].Definition	=	lmap_shadow_mode;
		def_it						++;
	}

	// finish
	defines[def_it].Name			=	0;
	defines[def_it].Definition		=	0;
	def_it							++;
	
#ifdef USE_SHADER_CACHE
	char* defs = (char*)Memory.mem_alloc(1024);

	R_ASSERT(defs);

	strconcat(1024, defs, defines[0].Name,defines[0].Definition);

	for (u32 i = 1;;i++)	{
		if (0==defines[i].Name)	break;
		strcat(defs, defines[i].Name);
		strcat(defs, defines[i].Definition);
	}

	string_path			sh_name;
	strconcat			(sizeof(sh_name),sh_name,"shader_cache\\",name,('v'==pTarget[0])?".vs":".ps" );
	IReader*	R		= FS.r_open("$logs$",sh_name);
	if (R)
	{
//		Msg ("Found saved shader: %s", sh_name);
		char* saved_defs = (char*)Memory.mem_alloc(1024);
		R_ASSERT(saved_defs);
		R->r_stringZ(saved_defs, 1024);
		if(!strcmp(defs, saved_defs))
		{
#ifdef _WIN7_COMPILER
			ID3DBlob**                  ppShader		= (ID3DBlob**)		_ppShader;
			D3DCreateBlob(R->elapsed(), ppShader);
#else
			LPD3DXBUFFER*                   ppShader		= (LPD3DXBUFFER*)		_ppShader;
			D3DXCreateBuffer(R->elapsed(), ppShader);
#endif
			R->r((*ppShader)->GetBufferPointer(), (int)(*ppShader)->GetBufferSize());
			FS.r_close			(R);
			Memory.mem_free((void*)defs);
			Memory.mem_free((void*)saved_defs);
			return D3D_OK;
		}
		else 
		{
			Memory.mem_free((void*)saved_defs);
		}
	}

#endif
	// 
	if (0==xr_strcmp(pFunctionName,"main"))	{
		if ('v'==pTarget[0])			pTarget = D3DXGetVertexShaderProfile	(HW.pDevice);	// vertex	"vs_2_a"; //	
		else							pTarget = D3DXGetPixelShaderProfile		(HW.pDevice);	// pixel	"ps_2_a"; //	
	}

	HRESULT		_result;
#ifdef _WIN7_COMPILER
		ID3DInclude*                pInclude		= (ID3DInclude *)	_pInclude;
		ID3DBlob**                  ppShader		= (ID3DBlob**)		_ppShader;
		ID3DBlob**                  ppErrorMsgs		= (ID3DBlob**)		_ppErrorMsgs;
		_result	= D3DCompile((LPCVOID)pSrcData,(SIZE_T)SrcDataLen, name,(const D3D_SHADER_MACRO *)defines,pInclude,pFunctionName,pTarget,
			D3DCOMPILE_DEBUG|D3DCOMPILE_PACK_MATRIX_ROW_MAJOR/*|D3DCOMPILE_SKIP_OPTIMIZATION*/|D3DCOMPILE_OPTIMIZATION_LEVEL3,0,ppShader,ppErrorMsgs);

		if (SUCCEEDED(_result)) 
		{
			ID3DBlob*		code	= *((ID3DBlob**)_ppShader);
#ifdef USE_SHADER_CACHE
			string_path			cname;
			strconcat			(sizeof(cname),cname,"shader_cache\\",name,('v'==pTarget[0])?".vs":".ps" );
			IWriter*			W		= FS.w_open("$logs$",cname);
			W->w_stringZ		(defs);
			W->w				(code->GetBufferPointer(),(int)code->GetBufferSize());
			FS.w_close			(W);
			Memory.mem_free((void*)defs);
#endif
			if (o.disasm)
			{
				ID3DBlob*		disasm	= 0;
				D3DDisassemble		(LPCVOID(code->GetBufferPointer()), code->GetBufferSize(), 0, 0, &disasm );
				string_path			dname;
				strconcat			(sizeof(dname),dname,"disasm\\",name,('v'==pTarget[0])?".vs":".ps"/*"_vs.txt":"_ps.txt"*/ );
				IWriter*			W		= FS.w_open("$logs$",dname);
				W->w				(disasm->GetBufferPointer(),(u32)disasm->GetBufferSize());
				FS.w_close			(W);
				_RELEASE			(disasm);
			}
		} else
		{
			VERIFY	(ppErrorMsgs);
			Log		("! error: ",(LPCSTR)(*((ID3DBlob**)ppErrorMsgs))->GetBufferPointer());
		}
#else
		
		LPD3DXINCLUDE                   pInclude		= (LPD3DXINCLUDE)		_pInclude;
		LPD3DXBUFFER*                   ppShader		= (LPD3DXBUFFER*)		_ppShader;
		LPD3DXBUFFER*                   ppErrorMsgs		= (LPD3DXBUFFER*)		_ppErrorMsgs;
		LPD3DXCONSTANTTABLE*            ppConstantTable	= (LPD3DXCONSTANTTABLE*)_ppConstantTable;
#ifdef	D3DXSHADER_USE_LEGACY_D3DX9_31_DLL	//	December 2006 and later
		_result	= D3DXCompileShader(pSrcData,SrcDataLen,defines,pInclude,pFunctionName,pTarget,Flags|D3DXSHADER_USE_LEGACY_D3DX9_31_DLL,ppShader,ppErrorMsgs,ppConstantTable);
#else
		_result	= D3DXCompileShader(pSrcData,SrcDataLen,defines,pInclude,pFunctionName,pTarget,Flags/*|D3DXSHADER_DEBUG*//*|D3DXSHADER_SKIPOPTIMIZATION*/,ppShader,ppErrorMsgs,ppConstantTable);
#endif
		Msg("[SHADER COMPILATION] Name = %s, Target = %s, compilation result = %d", name, pTarget, _result);
		if (SUCCEEDED(_result)) 
		{
			ID3DXBuffer*		code	= *((LPD3DXBUFFER*)_ppShader);
#ifdef USE_SHADER_CACHE
			string_path			cname;
			strconcat			(sizeof(cname),cname,"shader_cache\\",name,('v'==pTarget[0])?".vs":".ps" );
			IWriter*			W		= FS.w_open("$logs$",cname);
			W->w_stringZ		(defs);
			W->w				(code->GetBufferPointer(),code->GetBufferSize());
			FS.w_close			(W);
			Memory.mem_free((void*)defs);
#endif
// отладка для Gun12
			/*
			IDirect3DPixelShader9*				ps;
			HRESULT _hr = HW.pDevice->CreatePixelShader	((DWORD *)(code->GetBufferPointer()), &ps);
			if (SUCCEEDED(_hr))	{
				Msg("Pixel shader created");
				LPCVOID			data		= NULL;
				_hr	= D3DXFindShaderComment	((DWORD *)(code->GetBufferPointer()),MAKEFOURCC('C','T','A','B'),&data,NULL);
				if (SUCCEEDED(_hr) && data)
				{
					_RELEASE(ps);
					Msg("Constants parsed");
				}
				else
					Msg("Can't parse constants, result: %d", _hr);
			} else
			{
				Msg("Can't create pixel shader, result: %d. Replacing for dumb shader.", _hr);
				// Open file
				string_path					cname;
				strconcat					(sizeof(cname), cname,"r2\\", "dumb.ps");
				FS.update_path				(cname,	"$game_shaders$", cname);

				// duplicate and zero-terminate
				IReader*		R		= FS.r_open(cname);
				R_ASSERT2				(R,cname);
				u32				size	= R->length();
				char*			data	= xr_alloc<char>(size + 1);
				CopyMemory			(data,R->pointer(),size);
				data[size]				= 0;
				FS.r_close				(R);
				_result	= D3DXCompileShader(data,size,defines,pInclude,pFunctionName,pTarget,Flags/*|D3DXSHADER_DEBUG*//*|D3DXSHADER_SKIPOPTIMIZATION*//*,ppShader,ppErrorMsgs,ppConstantTable);
			}
*/

			if (o.disasm)
			{
	//			ID3DXBuffer*		code	= *((LPD3DXBUFFER*)_ppShader);
				ID3DXBuffer*		disasm	= 0;
				D3DXDisassembleShader		(LPDWORD(code->GetBufferPointer()), FALSE, 0, &disasm );
				string_path			dname;
				strconcat			(sizeof(dname),dname,"disasm\\",name,('v'==pTarget[0])?"_vs.txt":"_ps.txt" );
				IWriter*			W		= FS.w_open("$logs$",dname);
				W->w				(disasm->GetBufferPointer(),disasm->GetBufferSize());
				FS.w_close			(W);
				_RELEASE			(disasm);
			}
		}
#endif

	return		_result;
}
void CRender::add_SkeletonWallmark	(intrusive_ptr<CSkeletonWallmark> wm)
{
	Wallmarks->AddSkeletonWallmark				(wm);
}
void CRender::add_SkeletonWallmark	(const Fmatrix* xf, CKinematics* obj, ref_shader& sh, const Fvector& start, const Fvector& dir, float size)
{
	Wallmarks->AddSkeletonWallmark				(xf, obj, sh, start, dir, size);
}

IRender_Light*			CRender::light_create			()					{ return Lights.Create();								}

BOOL CRender::is_sun()
{
	if (o.sunstatic)		return FALSE;
	Fcolor					sun_color			= ((light*)Lights.sun_adapted._get())->color;
	return					(ps_r2_ls_flags.test(R2FLAG_SUN) && (u_diffuse2s(sun_color.r,sun_color.g,sun_color.b)>EPS));
}