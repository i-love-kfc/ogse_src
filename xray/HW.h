#pragma once

#include <d3dx9.h>

class CHW
{
  HINSTANCE__ *hD3D9;
public:
  IDirect3D9 *pD3D;
  IDirect3DDevice9 *pDevice;
  IDirect3DSurface9 *pBaseRT;
  IDirect3DSurface9 *pBaseZB;
  struct CHWCaps
	{
		struct		caps_Geometry
		{
			u32	dwRegisters		: 16;
			u32 dwInstructions	: 16;
			u32	bSoftware		: 1;
			u32	bPointSprites	: 1;
			u32	bVTF			: 1;		// vertex-texture-fetch
			u32	bNPatches		: 1;
			u32 dwClipPlanes	: 4;
			u32 dwVertexCache	: 8;
		};
		struct		caps_Raster
		{
			u32	dwRegisters		: 16;
			u32 dwInstructions	: 16;
			u32	dwStages		: 4;		// number of tex-stages
			u32	dwMRT_count		: 4;
			u32 b_MRT_mixdepth	: 1;
			u32	bNonPow2		: 1;
			u32	bCubemap		: 1;
		};
			// force flags
		BOOL			bForceGPU_REF;
		BOOL			bForceGPU_SW;
		BOOL			bForceGPU_NonPure;
		BOOL			SceneMode;

		// device format
		D3DFORMAT		fTarget;
		D3DFORMAT		fDepth;
		u32				dwRefreshRate;

		// caps itself
		u16				geometry_major	;
		u16				geometry_minor	;
		caps_Geometry	geometry		;
		u16				raster_major	;
		u16				raster_minor	;
		caps_Raster		raster			;

		u32				id_vendor		;
		u32				id_device		;

		BOOL			bStencil;			// stencil buffer present
		BOOL			bScissor;			// scissor rect supported
		BOOL			bTableFog;			//

		// some precalculated values
		D3DSTENCILOP	soDec, soInc;		// best stencil OPs for shadows
		u32				dwMaxStencilValue;  // maximum value the stencil buffer can hold
	} Caps;
  unsigned int DevAdapter;
  _D3DDEVTYPE DevT;
  _D3DPRESENT_PARAMETERS_ DevPP;

  BOOL					support					(D3DFORMAT fmt, DWORD type, DWORD usage);
};

extern __declspec(dllimport) CHW HW;