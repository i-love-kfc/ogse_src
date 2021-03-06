#include "blenders.h"
#include "RenderDevice.h"
#include "fog_volumes.h"
#include "xr_render_console.h"

static D3DVERTEXELEMENT9 vFormat[] =
{
	{ 0, 0,  D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_POSITION,	0 },	// pos
//	{ 0, 12, D3DDECLTYPE_SHORT4,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_TEXCOORD,	0 },	// uv
	D3DDECL_END()
};

CLevelFog::CLevelFog()
{
	b_fvolumes = xr_new<CBlender_fog_volume>			();
	s_fvolumes.create				(b_fvolumes,					"r2\\fog_volumes");

	float w,h;
	w = Device.dwWidth/2.f;
	h = Device.dwHeight/2.f;

	rt_FV_faces.create			(r2_RT_FV_faces,	(u32)w,(u32)h,D3DFMT_A16B16G16R16F		);
	rt_FV_color.create			(r2_RT_FV_color,	(u32)w,(u32)h,D3DFMT_A16B16G16R16F		);
	rt_FV_tc.create				(r2_RT_FV_tc,		(u32)w,(u32)h,D3DFMT_A16B16G16R16F		);

	m_bLoaded = false;
}

CLevelFog::~CLevelFog()
{
	xr_delete		(b_fvolumes);
}

void CLevelFog::Load()
{
	// Open file stream
	if (!ps_r2_ls_flags.test(R2FLAG_FOG_VOLUME) || !FS.exist("$level$","level.fvolumes"))
	{
		m_volumes.clear();
		return;
	}
	string_path fn;
	FS.update_path(fn, "$level$", "level.fvolumes");
	IReader *fs = FS.r_open(fn);

	IReader *g_chunk = fs->open_chunk(FV_HEADER);
	m_version = fs->r_u16();
	u16 size = fs->r_u16();
	g_chunk->close();

	if (0==size)
	{
		m_volumes.clear();
		FS.r_close(fs);
		return;
	}

	string_path ini_file;
	FS.update_path				(ini_file,"$game_config$", "environment\\fog_volumes.ltx");
	CInifile ini(ini_file);

	g_chunk = fs->open_chunk(FV_MAIN);
	m_volumes.reserve(size);

	for (u16 i = 0; i < size; ++i)
	{
		// open chunk with next volume
		IReader *chunk				= g_chunk->open_chunk(i);

		// create volume object
		CFogVolume vol;

		// fill ltx
		chunk->r_stringZ(vol.m_name);
		vol.m_color = ini.r_fcolor(vol.m_name, "color");
		vol.fMaxDensity = ini.r_float(vol.m_name, "max_density");

		chunk->r(&vol.m_bbox, 0x18);
		chunk->r(&vol.m_xform, 0x40);

		vol.m_inv_xform.invert(vol.m_xform);

/*		Msg("_11, _12, _13, _14: %f,%f,%f,%f", vol.m_xform._11, vol.m_xform._12, vol.m_xform._13, vol.m_xform._14);
		Msg("_21, _22, _23, _24: %f,%f,%f,%f", vol.m_xform._21, vol.m_xform._22, vol.m_xform._23, vol.m_xform._24);
		Msg("_31, _32, _33, _34: %f,%f,%f,%f", vol.m_xform._31, vol.m_xform._32, vol.m_xform._33, vol.m_xform._34);
		Msg("_41, _42, _43, _44: %f,%f,%f,%f", vol.m_xform._41, vol.m_xform._42, vol.m_xform._43, vol.m_xform._44);

		Msg("_11, _12, _13, _14: %f,%f,%f,%f", vol.m_inv_xform._11, vol.m_inv_xform._12, vol.m_inv_xform._13, vol.m_inv_xform._14);
		Msg("_21, _22, _23, _24: %f,%f,%f,%f", vol.m_inv_xform._21, vol.m_inv_xform._22, vol.m_inv_xform._23, vol.m_inv_xform._24);
		Msg("_31, _32, _33, _34: %f,%f,%f,%f", vol.m_inv_xform._31, vol.m_inv_xform._32, vol.m_inv_xform._33, vol.m_inv_xform._34);
		Msg("_41, _42, _43, _44: %f,%f,%f,%f", vol.m_inv_xform._41, vol.m_inv_xform._42, vol.m_inv_xform._43, vol.m_inv_xform._44);*/

		u32	dwUsage			= D3DUSAGE_WRITEONLY;

		// create and fill vertex buffer
		{
			vol.num_vertices	= chunk->r_u32();
			BYTE*	pData		= 0;
			R_CHK				(HW.pDevice->CreateVertexBuffer(vol.num_vertices*12,dwUsage,0,D3DPOOL_MANAGED,&vol.g_vb,0));
			R_CHK				(vol.g_vb->Lock(0,0,(void**)&pData,0));
			chunk->r			(pData,vol.num_vertices*12);
			vol.g_vb->Unlock	();
		}

		// create and fill index buffer
		{
			vol.num_indices		= chunk->r_u32();
			BYTE*	pData		= 0;
			R_CHK				(HW.pDevice->CreateIndexBuffer(vol.num_indices*2,dwUsage,D3DFMT_INDEX16,D3DPOOL_MANAGED,&vol.g_ib,0));
			R_CHK				(vol.g_ib->Lock(0,0,(void**)&pData,0));
			chunk->r			(pData,vol.num_indices*2);
			vol.g_ib->Unlock	();
		}
		chunk->close();
		vol.hGeom.create				(vFormat, vol.g_vb, vol.g_ib);
		m_volumes.push_back(vol);
	}
	g_chunk->close();
	FS.r_close(fs);
	m_bLoaded = true;
}

void CLevelFog::Unload()
{
	if (!m_bLoaded)		return;

	for (u16 i = 0; i < m_volumes.size(); ++i)
	{
		m_volumes[i].hGeom.destroy();
		_RELEASE(m_volumes[i].g_vb);
		_RELEASE(m_volumes[i].g_ib);
	}
	m_volumes.clear();
	m_bLoaded = false;
}