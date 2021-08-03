#include "DetailManager.h"
#include "r2_rendertarget.h"
#include "r2.h"
#include "Game_Persistent.h"
#include "game_level.h"
#include "xr_3da.h"
#include "xrCore.h"
#include "FMesh.h"
#include "effect_rain.h"
#include "uber_deffer.h"
#include "WallmarksEngine.h"

CLevelFog *FV;
CPuddles *Puddles;
rain_timer_params* rain_timers_raycheck;
rain_timer_params* rain_timers;

typedef xr_vector<shared_str>	sh_list;
void	_ParseList(sh_list& dest, LPCSTR names)
{
	if (0==names) 		names 	= "$null";

	ZeroMemory			(&dest, sizeof(dest));
	char*	P			= (char*) names;
	svector<char,128>	N;

	while (*P)
	{
		if (*P == ',') {
			// flush
			N.push_back	(0);
			strlwr		(N.begin());

			fix_texture_name( N.begin() );
//. andy			if (strext(N.begin())) *strext(N.begin())=0;
			dest.push_back(N.begin());
			N.clear		();
		} else {
			N.push_back	(*P);
		}
		P++;
	}
	if (N.size())
	{
		// flush
		N.push_back	(0);
		strlwr		(N.begin());

		fix_texture_name( N.begin() );
//. andy		if (strext(N.begin())) *strext(N.begin())=0;
		dest.push_back(N.begin());
	}
}
shared_str CTextureDescrMngr::GetBumpName(const shared_str& tex_name) const
{
	map_TD::const_iterator I = m_texture_details.find	(tex_name);
	if (I!=m_texture_details.end())
	{
		if(I->second.m_spec)
		{
			return I->second.m_spec->m_bump_name;
		}	
	}
	return "";
}

void CRender::level_Load(IReader* fs)
{
	R_ASSERT						(0!=g_pGameLevel);
	R_ASSERT						(!b_loaded);

	/* get base address of modules*/
	// deprecated code
/*	ListProcessModules(GetCurrentProcessId());

	Msg("Base address fo xrRender_R2.dll is %x", BASE_ADDRESS_XRRENDER_R2);
	Msg("Base address fo xrGame.dll is %x", BASE_ADDRESS_XRGAME);*/

	// Begin
	pApp->LoadBegin					();
	Device.Resources->DeferredLoad	(TRUE);
	IReader*						chunk;

	// Shaders
	g_pGamePersistent->LoadTitle		("st_loading_shaders");
	{
		chunk = fs->open_chunk		(fsL_SHADERS);
		R_ASSERT2					(chunk,"Level doesn't builded correctly.");
		u32 count = chunk->r_u32	();
		Shaders.resize				(count);
//		Msg("SHADER CHUNK TEXTURES LOADED");
		for(u32 i=0; i<count; i++)	// skip first shader as "reserved" one
		{
			string512				n_sh,n_tlist;
			LPCSTR			n		= LPCSTR(chunk->pointer());
			chunk->skip_stringZ		();
			if (0==n[0])			continue;
			strcpy					(n_sh,n);
			LPSTR			delim	= strchr(n_sh,'/');
			*delim					= 0;
			strcpy					(n_tlist,delim+1);
			Shaders[i]				= Device.Resources->Create(n_sh,n_tlist);
/*			sh_list lst;
			_ParseList			(lst,	n_tlist	);
			Msg("%s (shader %s)", n_tlist, n_sh);
			shared_str bn = Device.Resources->m_textures_description.GetBumpName(lst[0]);
			if (bn.c_str() != "")
			{
				Msg("%s", bn.c_str());
				Msg("%s#", bn.c_str());
			}*/
		}
		chunk->close();
	}

	// Components
	Wallmarks					= xr_new<CWallmarksEngine>	();
	Details						= xr_new<CDetailManager>	();

	if	(!g_dedicated_server)	{
		// VB,IB,SWI
		g_pGamePersistent->LoadTitle("st_loading_geometry");
		{
			CStreamReader			*geom = FS.rs_open("$level$","level.geom");
			R_ASSERT2				(geom, "level.geom");
			w_LoadBuffers			(geom,FALSE);
			w_LoadSWIs				(geom);
			FS.r_close				(geom);
		}

		//...and alternate/fast geometry
		{
			CStreamReader			*geom = FS.rs_open("$level$","level.geomx");
			R_ASSERT2				(geom, "level.geomX");
			w_LoadBuffers				(geom,TRUE);
			FS.r_close				(geom);
		}

		// Visuals
		g_pGamePersistent->LoadTitle("st_loading_spatial_db");
		chunk						= fs->open_chunk(fsL_VISUALS);		
		w_LoadVisuals				(chunk);
		chunk->close				();


		// Details
		g_pGamePersistent->LoadTitle("st_loading_details");
		Details->Load				();
	}

	// Sectors
	g_pGamePersistent->LoadTitle("st_loading_sectors_portals");
	w_LoadSectors					(fs);

	// Fog Volumes
	FV = xr_new<CLevelFog>();
	FV->Load();

	// Puddles
	Puddles = xr_new<CPuddles>();
	Puddles->Load();

	// Rain stuff
	rain_timers = xr_new<rain_timer_params>();
	rain_timers_raycheck = xr_new<rain_timer_params>();

	// HOM
	HOM.w_Load					();

	// Lights
	// pApp->LoadTitle			("Loading lights...");
	LoadLights					(fs);

	// End
	pApp->LoadEnd				();

	// sanity-clear
	lstLODs.clear				();
	lstLODgroups.clear			();
	mapLOD.clear				();

	// signal loaded
	b_loaded					= TRUE	;
}

void CRender::level_Unload()
{
	if (0==g_pGameLevel)		return;
	if (!b_loaded)				return;

	u32 I;
	
	// Puddles
	Puddles->Unload			();

	// Fog volumes
	FV->Unload				();

	// HOM
	HOM.w_Unload			();

	//*** Details
	Details->Unload			();

	//*** Sectors
	// 1.
	xr_delete				(rmPortals);
	pLastSector				= 0;
	vLastCameraPos.set		(0,0,0);
	// 2.
	for (I=0; I<Sectors.size(); I++)
		xr_delete(Sectors[I]);
	Sectors.clear			();
	// 3.
	Portals.clear			();

	//*** Lights
	// Glows.Unload			();
	Lights.Unload			();

	//*** Visuals
	for (I=0; I<Visuals.size(); I++)
	{
		Visuals[I]->Release();
		xr_delete(Visuals[I]);
	}
	Visuals.clear			();

	//*** VB/IB
	for (I=0; I<nVB.size(); I++)	_RELEASE(nVB[I]);
	for (I=0; I<xVB.size(); I++)	_RELEASE(xVB[I]);
	nVB.clear(); xVB.clear();
	for (I=0; I<nIB.size(); I++)	_RELEASE(nIB[I]);
	for (I=0; I<xIB.size(); I++)	_RELEASE(xIB[I]);
	nIB.clear(); xIB.clear();
	nDC.clear(); xDC.clear();

	//*** Components
	xr_delete					(FV);
	xr_delete					(rain_timers);
	xr_delete					(rain_timers_raycheck);
	xr_delete					(Details);
	xr_delete					(Wallmarks);

	//*** Shaders
	Shaders.clear_and_free		();
	b_loaded					= FALSE;
}


/*void CRender::LoadBuffers		(CStreamReader *base_fs,	BOOL _alternative)
{
	R_ASSERT2					(base_fs,"Could not load geometry. File not found.");
	Device.Resources->Evict		();
	u32	dwUsage					= D3DUSAGE_WRITEONLY;

	xr_vector<VertexDeclarator>				&_DC	= _alternative?xDC:nDC;
	xr_vector<IDirect3DVertexBuffer9*>		&_VB	= _alternative?xVB:nVB;
	xr_vector<IDirect3DIndexBuffer9*>		&_IB	= _alternative?xIB:nIB;

	// Vertex buffers
	{
		// Use DX9-style declarators
		CStreamReader			*fs	= base_fs->open_chunk(fsL_VB);
		R_ASSERT2				(fs,"Could not load geometry. File 'level.geom?' corrupted.");
		u32 count				= fs->r_u32();
		_DC.resize				(count);
		_VB.resize				(count);
		for (u32 i=0; i<count; i++)
		{
			// decl
//			D3DVERTEXELEMENT9*	dcl		= (D3DVERTEXELEMENT9*) fs().pointer();
			u32					buffer_size = (MAXD3DDECLLENGTH+1)*sizeof(D3DVERTEXELEMENT9);
			D3DVERTEXELEMENT9	*dcl = (D3DVERTEXELEMENT9*)_alloca(buffer_size);
			fs->r				(dcl,buffer_size);
			fs->advance			(-(int)buffer_size);

			u32 dcl_len			= D3DXGetDeclLength		(dcl)+1;
			_DC[i].resize		(dcl_len);
			fs->r				(_DC[i].begin(),dcl_len*sizeof(D3DVERTEXELEMENT9));

			// count, size
			u32 vCount			= fs->r_u32	();
			u32 vSize			= D3DXGetDeclVertexSize	(dcl,0);
			Msg	("* [Loading VB] %d verts, %d Kb",vCount,(vCount*vSize)/1024);

			// Create and fill
			BYTE*	pData		= 0;
			R_CHK				(HW.pDevice->CreateVertexBuffer(vCount*vSize,dwUsage,0,D3DPOOL_MANAGED,&_VB[i],0));
			R_CHK				(_VB[i]->Lock(0,0,(void**)&pData,0));
//			CopyMemory			(pData,fs().pointer(),vCount*vSize);
			fs->r				(pData,vCount*vSize);
			_VB[i]->Unlock		();

//			fs->advance			(vCount*vSize);
		}
		fs->close				();
	}

	// Index buffers
	{
		CStreamReader			*fs	= base_fs->open_chunk(fsL_IB);
		u32 count				= fs->r_u32();
		_IB.resize				(count);
		for (u32 i=0; i<count; i++)
		{
			u32 iCount			= fs->r_u32	();
			Msg("* [Loading IB] %d indices, %d Kb",iCount,(iCount*2)/1024);

			// Create and fill
			BYTE*	pData		= 0;
			R_CHK				(HW.pDevice->CreateIndexBuffer(iCount*2,dwUsage,D3DFMT_INDEX16,D3DPOOL_MANAGED,&_IB[i],0));
			R_CHK				(_IB[i]->Lock(0,0,(void**)&pData,0));
//			CopyMemory			(pData,fs().pointer(),iCount*2);
			fs->r				(pData,iCount*2);
			_IB[i]->Unlock		();

//			fs().advance		(iCount*2);
		}
		fs->close				();
	}
}

void CRender::LoadVisuals(IReader *fs)
{
	IReader*		chunk	= 0;
	u32			index	= 0;
	IRender_Visual*		V		= 0;
	ogf_header		H;

	while ((chunk=fs->open_chunk(index))!=0)
	{
		chunk->r_chunk_safe			(OGF_HEADER,&H,sizeof(H));
		V = Models->Instance_Create	(H.type);
		V->Load(0,chunk,0);
		Visuals.push_back(V);

		chunk->close();
		index++;
	}
}
*/
void CRender::LoadLights(IReader *fs)
{
	// lights
	Lights.Load	(fs);
}

/*struct b_portal
{
	u16				sector_front;
	u16				sector_back;
	svector<Fvector,6>	vertices;
};

void CRender::LoadSectors(IReader* fs)
{
	// allocate memory for portals
	u32 size = fs->find_chunk(fsL_PORTALS); 
	R_ASSERT(0==size%sizeof(b_portal));
	u32 count = size/sizeof(b_portal);
	Portals.resize	(count);
	for (u32 c=0; c<count; c++)
		Portals[c]	= xr_new<CPortal> ();

	// load sectors
	IReader* S = fs->open_chunk(fsL_SECTORS);
	for (u32 i=0; ; i++)
	{
		IReader* P = S->open_chunk(i);
		if (0==P) break;

		CSector* __S		= xr_new<CSector> ();
		__S->load			(*P);
		Sectors.push_back	(__S);

		P->close();
	}
	S->close();

	// load portals
	if (count) 
	{
		CDB::Collector	CL;
		fs->find_chunk	(fsL_PORTALS);
		for (u32 i=0; i<count; i++)
		{
			b_portal	P;
			fs->r		(&P,sizeof(P));
			CPortal*	__P	= (CPortal*)Portals[i];
			__P->Setup	(P.vertices.begin(),P.vertices.size(),
				(CSector*)getSector(P.sector_front),
				(CSector*)getSector(P.sector_back));
			for (u32 j=2; j<P.vertices.size(); j++)
				CL.add_face_packed_D(
				P.vertices[0],P.vertices[j-1],P.vertices[j],
				u32(i)
				);
		}
		if (CL.getTS()<2)
		{
			Fvector					v1,v2,v3;
			v1.set					(-20000.f,-20000.f,-20000.f);
			v2.set					(-20001.f,-20001.f,-20001.f);
			v3.set					(-20002.f,-20002.f,-20002.f);
			CL.add_face_packed_D	(v1,v2,v3,0);
		}

		// build portal model
		rmPortals = xr_new<CDB::MODEL> ();
		rmPortals->build	(CL.getV(),int(CL.getVS()),CL.getT(),int(CL.getTS()));
	} else {
		rmPortals = 0;
	}

	// debug
	//	for (int d=0; d<Sectors.size(); d++)
	//		Sectors[d]->DebugDump	();

	pLastSector = 0;
}

void CRender::LoadSWIs(CStreamReader* base_fs)
{
	// allocate memory for portals
	if (base_fs->find_chunk(fsL_SWIS)){
		CStreamReader		*fs	= base_fs->open_chunk(fsL_SWIS);
		u32 item_count		= fs->r_u32();

		xr_vector<FSlideWindowItem>::iterator it	= SWIs.begin();
		xr_vector<FSlideWindowItem>::iterator it_e	= SWIs.end();

		for(;it!=it_e;++it)
			xr_free( (*it).sw );

		SWIs.clear_not_free();

		SWIs.resize			(item_count);
		for (u32 c=0; c<item_count; c++){
			FSlideWindowItem& swi = SWIs[c];
			swi.reserved[0]	= fs->r_u32();	
			swi.reserved[1]	= fs->r_u32();	
			swi.reserved[2]	= fs->r_u32();	
			swi.reserved[3]	= fs->r_u32();	
			swi.count		= fs->r_u32();
			VERIFY			(NULL==swi.sw);
			swi.sw			= xr_alloc<FSlideWindow> (swi.count);
			fs->r			(swi.sw,sizeof(FSlideWindow)*swi.count);
		}
		fs->close			();
	}
}
*/