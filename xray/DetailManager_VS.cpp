#include "r2.h"
#include "detailmanager.h"
#include "Backend.h"
#include "Game_Persistent.h"
#include "xr_render_console.h"

const int			quant	= 16384;
const int			c_hdr	= 10;
//const int			c_size	= 4;
const int			c_size	= 2;

static D3DVERTEXELEMENT9 dwDecl[] =
{
	{ 0, 0,  D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_POSITION,	0 },	// pos
	{ 0, 12, D3DDECLTYPE_SHORT4,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_TEXCOORD,	0 },	// uv
#ifdef HWI
	{ 1, 0,   D3DDECLTYPE_FLOAT4,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_TEXCOORD,	1 },	// matrix 0
	{ 1, 16,  D3DDECLTYPE_FLOAT4,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_TEXCOORD,	2 },	// matrix 1
//	{ 1, 32,  D3DDECLTYPE_FLOAT4,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_TEXCOORD,	3 },	// matrix 2
//	{ 1, 48,  D3DDECLTYPE_FLOAT2,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_TEXCOORD,	4 },	// sun + hemi
#endif
	D3DDECL_END()
};

#pragma pack(push,1)
struct	vertHW
{
	float		x,y,z;
	short		u,v,t,mid;
};
struct	vertHW_instance
{
	float		cosa,sina,s,h;
	float		x,y,z,scale;
};
#pragma pack(pop)

short QC (float v)
{
	int t=iFloor(v*float(quant)); clamp(t,-32768,32767);
	return short(t&0xffff);
}

#ifdef HWI
void CDetailManager::hw_Load	()
{
	// Analyze batch-size
	hw_BatchSize	= (u32(HW.Caps.geometry.dwRegisters)-c_hdr)/c_size;
	clamp			(hw_BatchSize,(u32)0,(u32)64);
	Msg				("* [DETAILS] VertexConsts(%d), Batch(%d)",u32(HW.Caps.geometry.dwRegisters),hw_BatchSize);

	// Pre-process objects
	u32			dwVerts		= 0;
	u32			dwIndices	= 0;
	for (u32 o=0; o<objects.size(); o++)
	{
		CDetail& D	=	*objects[o];
		dwVerts		+=	D.number_vertices;
		dwIndices	+=	D.number_indices;
	}
	u32			vSize		= sizeof(vertHW);
	Msg("* [DETAILS] %d v(%d), %d p",dwVerts,vSize,dwIndices/3);

	// Determine POOL & USAGE
	u32 dwUsage		=	D3DUSAGE_WRITEONLY;

	// Create VB/IB
	R_CHK			(HW.pDevice->CreateVertexBuffer(dwVerts*vSize,dwUsage,0,D3DPOOL_MANAGED,&hw_VB,0));
	R_CHK			(HW.pDevice->CreateIndexBuffer(dwIndices*2,dwUsage,D3DFMT_INDEX16,D3DPOOL_MANAGED,&hw_IB,0));
	Msg("* [DETAILS] VB(%dK), IB(%dK)",(dwVerts*vSize)/1024, (dwIndices*2)/1024);

	// added by KD
	R_CHK			(HW.pDevice->CreateVertexBuffer(dm_current_cache_line*(dm_current_cache_line/2)*4*sizeof(vertHW_instance),D3DUSAGE_DYNAMIC,0,D3DPOOL_DEFAULT,&HI_instance_data,0));

	// Fill VB
	{
		vertHW*			pV;
		R_CHK			(hw_VB->Lock(0,0,(void**)&pV,0));
		for (u32 o=0; o<objects.size(); o++)
		{
			CDetail& D		=	*objects[o];
			for (u32 v=0; v<D.number_vertices; v++)
			{
				Fvector&	vP = D.vertices[v].P;
				pV->x	=	vP.x;
				pV->y	=	vP.y;
				pV->z	=	vP.z;
				pV->u	=	QC(D.vertices[v].u);
				pV->v	=	QC(D.vertices[v].v);
				pV->t	=	QC(vP.y/(D.bv_bb.max.y-D.bv_bb.min.y));
				pV->mid	=	1;
				pV++;
			}
		}
		R_CHK			(hw_VB->Unlock());
	}

	// Fill IB
	{
		u16*			pI;
		R_CHK			(hw_IB->Lock(0,0,(void**)(&pI),0));
		for (u32 o=0; o<objects.size(); o++)
		{
			CDetail& D		=	*objects[o];
			for (u32 i=0; i<u32(D.number_indices); i++)
				*pI++	=	u16(D.indices[i]);
		}
		R_CHK			(hw_IB->Unlock());
	}

	// Create shader to access constant storage
	ref_shader		S;	S.create("details\\set");
	R_constant_table&	T0	= *(S->E[0]->passes[0]->constants);
	R_constant_table&	T1	= *(S->E[1]->passes[0]->constants);
	hwc_consts			= T0.get("consts");
	hwc_wave			= T0.get("wave");
	hwc_wind			= T0.get("dir2D");
	hwc_array			= T0.get("array");
	hwc_s_consts		= T1.get("consts");
	hwc_s_xform			= T1.get("xform");
	hwc_s_array			= T1.get("array");

	// Declare geometry
	hw_Geom.create		(dwDecl, hw_VB, hw_IB);
}
#else
void CDetailManager::hw_Load	()
{
	// Analyze batch-size
	hw_BatchSize	= (u32(HW.Caps.geometry.dwRegisters)-c_hdr)/c_size;
	clamp			(hw_BatchSize,(u32)0,(u32)80);
	Msg				("* [DETAILS] VertexConsts(%d), Batch(%d)",u32(HW.Caps.geometry.dwRegisters),hw_BatchSize);

	// Pre-process objects
	u32			dwVerts		= 0;
	u32			dwIndices	= 0;
	for (u32 o=0; o<objects.size(); o++)
	{
		CDetail& D	=	*objects[o];
		dwVerts		+=	D.number_vertices*hw_BatchSize;
		dwIndices	+=	D.number_indices*hw_BatchSize;
	}
	u32			vSize		= sizeof(vertHW);
	Msg("* [DETAILS] %d v(%d), %d p",dwVerts,vSize,dwIndices/3);

	// Determine POOL & USAGE
	u32 dwUsage		=	D3DUSAGE_WRITEONLY;

	int lod_verts = objects.size()*hw_BatchSize*4*1.4;
	int lod_indices = objects.size()*hw_BatchSize*6*1.4;

	// Create VB/IB
	R_CHK			(HW.pDevice->CreateVertexBuffer((dwVerts*vSize/* + lod_verts*vSize*/),dwUsage,0,D3DPOOL_MANAGED,&hw_VB,0));
	R_CHK			(HW.pDevice->CreateIndexBuffer((dwIndices*2/* + lod_indices*2*/),dwUsage,D3DFMT_INDEX16,D3DPOOL_MANAGED,&hw_IB,0));
	R_CHK			(HW.pDevice->CreateVertexBuffer(lod_verts*vSize,dwUsage,0,D3DPOOL_MANAGED,&hw_lod_VB,0));
	R_CHK			(HW.pDevice->CreateIndexBuffer(lod_indices*2,dwUsage,D3DFMT_INDEX16,D3DPOOL_MANAGED,&hw_lod_IB,0));
	Msg("* [DETAILS] Batch(%d), VB(%dK), IB(%dK)",hw_BatchSize,(dwVerts*vSize)/1024, (dwIndices*2)/1024);

	// Create tc minmax vector
	xr_vector<Fvector4> tc_vec;
	Fvector4 temp = {1, 1, 0, 0};
	tc_vec.resize(objects.size(), temp);

	// Fill VB
	{
		vertHW*			pV;
		R_CHK			(hw_VB->Lock(0,0,(void**)&pV,0));
		for (u32 o=0; o<objects.size(); o++)
		{
			Fvector4& tc = tc_vec[o];
			CDetail& D		=	*objects[o];
			for (u32 batch=0; batch<hw_BatchSize; batch++)
			{
				u32 mid	=	batch*c_size;
				for (u32 v=0; v<D.number_vertices; v++)
				{
					Fvector&	vP = D.vertices[v].P;
					pV->x	=	vP.x;
					pV->y	=	vP.y;
					pV->z	=	vP.z;
					pV->u	=	QC(D.vertices[v].u);
					pV->v	=	QC(D.vertices[v].v);
					pV->t	=	QC(vP.y/(D.bv_bb.max.y-D.bv_bb.min.y));
					pV->mid	=	short(mid);
					pV++;

					tc.x = _min(tc.x, D.vertices[v].u);
					tc.y = _min(tc.y, D.vertices[v].v);
					tc.z = _max(tc.z, D.vertices[v].u);
					tc.w = _max(tc.w, D.vertices[v].v);
				}
			}
		}
		R_CHK			(hw_VB->Unlock());
	}

	// Fill lod VB
	{
		vertHW*			pV;
		R_CHK			(hw_lod_VB->Lock(0,0,(void**)&pV,0));
		for (u32 o=0; o<objects.size(); o++)
		{
			CDetail& D		=	*objects[o];
			u32 batchsize = hw_BatchSize*1.4;
			for (u32 batch=0; batch<batchsize; batch++)
			{
				u32 mid	=	batch*c_size;

					Fvector&	max = D.bv_bb.max;
					Fvector&	min = D.bv_bb.min;
					pV->x	=	min.x;
					pV->y	=	max.y;
					pV->z	=	0;
					pV->u	=	QC(tc_vec[o].x);
					pV->v	=	QC(tc_vec[o].w);
					pV->t	=	QC(max.y/(max.y-min.y));
					pV->mid	=	short(mid);
					pV++;

					pV->x	=	min.x;
					pV->y	=	min.y;
					pV->z	=	0;
					pV->u	=	QC(tc_vec[o].x);
					pV->v	=	QC(tc_vec[o].y);
					pV->t	=	QC(min.y/(max.y-min.y));
					pV->mid	=	short(mid);
					pV++;

					pV->x	=	max.x;
					pV->y	=	max.y;
					pV->z	=	0;
					pV->u	=	QC(tc_vec[o].z);
					pV->v	=	QC(tc_vec[o].w);
					pV->t	=	QC(max.y/(max.y-min.y));
					pV->mid	=	short(mid);
					pV++;

					pV->x	=	max.x;
					pV->y	=	min.y;
					pV->z	=	0;
					pV->u	=	QC(tc_vec[o].z);
					pV->v	=	QC(tc_vec[o].y);
					pV->t	=	QC(min.y/(max.y-min.y));
					pV->mid	=	short(mid);
					pV++;
			}
		}
		R_CHK			(hw_lod_VB->Unlock());
	}
	tc_vec.clear_and_free();

	// Fill IB
	{
		u16*			pI;
		R_CHK			(hw_IB->Lock(0,0,(void**)(&pI),0));
		for (u32 o=0; o<objects.size(); o++)
		{
			CDetail& D		=	*objects[o];
			u16		offset	=	0;
			for (u32 batch=0; batch<hw_BatchSize; batch++)
			{
				for (u32 i=0; i<u32(D.number_indices); i++)
					*pI++	=	u16(u16(D.indices[i]) + u16(offset));
				offset		=	u16(offset+u16(D.number_vertices));
			}
		}
		R_CHK			(hw_IB->Unlock());
	}

	// Fill lod IB
	{
		u16*			pI;
		R_CHK			(hw_lod_IB->Lock(0,0,(void**)(&pI),0));
		for (u32 o=0; o<objects.size(); o++)
		{
			CDetail& D		=	*objects[o];
			u32 batchsize = hw_BatchSize*1.4;
			u16		offset	=	0;
			for (u32 batch=0; batch<batchsize; batch++)
			{
				*pI++	=	u16(u16(0) + u16(offset));
				*pI++	=	u16(u16(1) + u16(offset));
				*pI++	=	u16(u16(2) + u16(offset));
				*pI++	=	u16(u16(3) + u16(offset));
				*pI++	=	u16(u16(2) + u16(offset));
				*pI++	=	u16(u16(1) + u16(offset));
				offset		=	u16(offset+4);
			}
		}
		R_CHK			(hw_lod_IB->Unlock());
	}

	// Create shader to access constant storage
	ref_shader		S;	S.create("details\\set");
	R_constant_table&	T0	= *(S->E[0]->passes[0]->constants);
	R_constant_table&	T1	= *(S->E[1]->passes[0]->constants);
	hwc_consts			= T0.get("consts");
	hwc_wave			= T0.get("wave");
	hwc_wind			= T0.get("dir2D");
	hwc_array			= T0.get("array");
	hwc_s_consts		= T1.get("consts");
	hwc_s_xform			= T1.get("xform");
	hwc_s_array			= T1.get("array");

	// Declare geometry
	hw_Geom.create		(dwDecl, hw_VB, hw_IB);

	// Declare lod geometry
	hw_lod_Geom.create		(dwDecl, hw_lod_VB, hw_lod_IB);
}
#endif

void CDetailManager::hw_Unload()
{
	// Destroy VS/VB/IB
	hw_Geom.destroy				();
	_RELEASE					(hw_IB);
	_RELEASE					(hw_VB);
#ifdef HWI
	_RELEASE					(HI_instance_data);
#endif

	hw_lod_Geom.destroy				();
	_RELEASE					(hw_lod_IB);
	_RELEASE					(hw_lod_VB);
}

void CDetailManager::hw_Render()
{
	// Render-prepare
	Fvector4	dir1,dir2;
	float		tm_rot1		= (PI_MUL_2*Device.fTimeGlobal/swing_current.rot1);
	float		tm_rot2		= (PI_MUL_2*Device.fTimeGlobal/swing_current.rot2);
	dir1.set				(_sin(tm_rot1),0,_cos(tm_rot1),0).normalize().mul(swing_current.amp1);
	dir2.set				(_sin(tm_rot2),0,_cos(tm_rot2),0).normalize().mul(swing_current.amp2);

	// Setup geometry and DMA
	RCache.set_Geometry		(hw_Geom);

	// Wave0
	float		scale			=	1.f/float(quant);
	Fvector4	wave;
	wave.set				(1.f/5.f,		1.f/7.f,	1.f/3.f,	Device.fTimeGlobal*swing_current.speed);
	RCache.set_c			(&*hwc_consts,	scale,		scale,		ps_r__Detail_l_aniso,	ps_r__Detail_l_ambient);				// consts
	RCache.set_c			(&*hwc_wave,	wave.div(PI_MUL_2));	// wave
	RCache.set_c			(&*hwc_wind,	dir1);																					// wind-dir
	hw_Render_dump			(&*hwc_array,	1, 0, c_hdr );

	// Wave1
	wave.set				(1.f/3.f,		1.f/7.f,	1.f/5.f,	Device.fTimeGlobal*swing_current.speed);
	RCache.set_c			(&*hwc_wave,	wave.div(PI_MUL_2));	// wave
	RCache.set_c			(&*hwc_wind,	dir2);																					// wind-dir
	hw_Render_dump			(&*hwc_array,	2, 0, c_hdr );

	// Still
	RCache.set_c			(&*hwc_s_consts,scale,		scale,		scale,				1.f);
	RCache.set_c			(&*hwc_s_xform,	Device.mFullTransform);
	hw_Render_dump			(&*hwc_s_array,	0, 1, c_hdr );

	RCache.set_Geometry		(hw_lod_Geom);
	hw_Render_dump_lod		(&*hwc_s_array,	c_hdr );
}

#ifdef HWI
void CDetailManager::hw_Render_dump		(ref_constant x_array, u32 var_id, u32 lod_id, u32 c_offset)
{
	Device.Statistic->RenderDUMP_DT_Count	= 0;

	// Matrices and offsets
//	u32		vOffset	=	0;
//	u32		iOffset	=	0;

	vis_list& list	=	m_visibles	[var_id];

	RCache.set_Element				(objects[0]->shader->E[lod_id]);
	RImplementation.apply_lmaterial	();

	vertHW_instance*			pV;
	R_CHK			(HI_instance_data->Lock(0,0,(void**)&pV,D3DLOCK_DISCARD));
	u32 num_instances = 0;


	// Iterate
	for (u32 O=0; O<objects.size(); O++){
		CDetail&	Object				= *objects	[O];
		xr_vector <SlotItemVec* >& vis	= list		[O];
		if (!vis.empty()){
	//		u32 num_instances = 0;
			// Setup matrices + colors (and flush it as nesessary)
	//		RCache.set_Element				(Object.shader->E[lod_id]);
	//		RImplementation.apply_lmaterial	();

//			vertHW_instance*			pV;
//			R_CHK			(HI_instance_data->Lock(0,0,(void**)&pV,D3DLOCK_DISCARD));

			xr_vector <SlotItemVec* >::iterator _vI = vis.begin();
			xr_vector <SlotItemVec* >::iterator _vE = vis.end();
			for (; _vI!=_vE; _vI++){
				SlotItemVec*	items		= *_vI;
				SlotItemVecIt _iI			= items->begin();
				SlotItemVecIt _iE			= items->end();
				for (; _iI!=_iE; _iI++){
					if (!(*_iI))
					{
#pragma todo		("Don't forget to remove this in release!")
						Msg("[DETAILS MANAGER RENDER]: SlotItem ptr is broken! _iI=%d, *_iI=%d", (u32)(&(*_iI)), (u32)(*(&(*_iI))));
						continue;
					}
					SlotItem&	Instance	= **_iI;

					// Build matrix ( 3x4 matrix, last row - color )
					float		scale		= Instance.scale_calculated;
					Fmatrix&	M			= Instance.mRotY;

					// Build color
					pV->cosa = M._11*scale;
					pV->sina = M._31*scale;
					pV->s = Instance.c_sun;
					pV->h = Instance.c_hemi;
					pV->x = M._41;
					pV->y = M._42;
					pV->z = M._43;
					pV->scale = scale;
					pV++;
					num_instances++;
				}
			}
//			R_CHK			(HI_instance_data->Unlock());
//			Msg("object # %d, instances %d, vOffset %d, iOffset %d", O, num_instances, vOffset, iOffset);
//			HW.pDevice->SetStreamSourceFreq(0, (D3DSTREAMSOURCE_INDEXEDDATA | num_instances));
//			HW.pDevice->SetStreamSource(0, hw_VB, 0, D3DXGetDeclVertexSize( dwDecl, 0 ));

//			HW.pDevice->SetStreamSourceFreq(1, (D3DSTREAMSOURCE_INSTANCEDATA | 1));
//			HW.pDevice->SetStreamSource(1, HI_instance_data, 0, D3DXGetDeclVertexSize( dwDecl, 1 ));

//			RCache.Render( D3DPT_TRIANGLELIST, vOffset, 0, Object.number_vertices, iOffset, Object.number_indices/3 );

//			HW.pDevice->SetStreamSourceFreq(0, 1);
//			HW.pDevice->SetStreamSourceFreq(1, 1);
		}
//		vOffset += Object.number_vertices;
//		iOffset += Object.number_indices;
	}
	R_CHK			(HI_instance_data->Unlock());
//			Msg("object # %d, instances %d, vOffset %d, iOffset %d", O, num_instances, vOffset, iOffset);
	HW.pDevice->SetStreamSourceFreq(0, (D3DSTREAMSOURCE_INDEXEDDATA | num_instances));
	HW.pDevice->SetStreamSource(0, hw_VB, 0, D3DXGetDeclVertexSize( dwDecl, 0 ));

	HW.pDevice->SetStreamSourceFreq(1, (D3DSTREAMSOURCE_INSTANCEDATA | 1));
	HW.pDevice->SetStreamSource(1, HI_instance_data, 0, D3DXGetDeclVertexSize( dwDecl, 1 ));

	RCache.Render( D3DPT_TRIANGLELIST, 0, 0, objects[0]->number_vertices, 0, objects[0]->number_indices/3 );

	HW.pDevice->SetStreamSourceFreq(0, 1);
	HW.pDevice->SetStreamSourceFreq(1, 1);
}
#else
#define DETAILS_OPT
void CDetailManager::hw_Render_dump		(ref_constant x_array, u32 var_id, u32 lod_id, u32 c_offset)
{
	Device.Statistic->RenderDUMP_DT_Count	= 0;

	// Matrices and offsets
	u32		vOffset	=	0;
	u32		iOffset	=	0;

	vis_list& list	=	m_visibles	[var_id];
/*
	CEnvDescriptor&	desc = g_pGamePersistent->Environment().CurrentEnv;
	Fvector					c_sun,c_ambient,c_hemi;
	c_sun.set				(desc.sun_color.x,	desc.sun_color.y,	desc.sun_color.z);	c_sun.mul(.5f);
	c_ambient.set			(desc.ambient.x,	desc.ambient.y,		desc.ambient.z);
	c_hemi.set				(desc.hemi_color.x,	desc.hemi_color.y,	desc.hemi_color.z);
*/
	// Iterate
	u32 counter = 0;
	u32 vert_counter = 0;
	for (u32 O=0; O<objects.size(); O++){
		CDetail&	Object				= *objects	[O];
/*		if ((1==lod_id) && !Object.m_Flags.is(DO_NO_WAVING)) 
		{
			vOffset		+=	hw_BatchSize * Object.number_vertices;
			iOffset		+=	hw_BatchSize * Object.number_indices;
			continue;
		}*/
		xr_vector <SlotItemVec* >& vis	= list		[O];
		if (!vis.empty()){
			// Setup matrices + colors (and flush it as nesessary)
			RCache.set_Element				(Object.shader->E[lod_id]);
			RImplementation.apply_lmaterial	();
			u32			c_base				= x_array->vs.index;
			Fvector4*	c_storage			= RCache.get_ConstantCache_Vertex().get_array_f().access(c_base);

			u32 dwBatch	= 0;

			xr_vector <SlotItemVec* >::iterator _vI = vis.begin();
			xr_vector <SlotItemVec* >::iterator _vE = vis.end();

			for (; _vI!=_vE; _vI++){
				SlotItemVec*	items		= *_vI;
				SlotItemVecIt _iI			= items->begin();
				SlotItemVecIt _iE			= items->end();
				for (; _iI!=_iE; _iI++){
					if (!(*_iI))
					{
/*	#pragma todo		("Don't forget to remove this in release!")
						Msg("[DETAILS MANAGER RENDER]: SlotItem ptr is broken! _iI=%d, *_iI=%d", (u32)(&(*_iI)), (u32)(*(&(*_iI))));*/
						continue;
					}

					float dist = Device.vCameraPosition.distance_to((*_iI)->mRotY.c);
					if ((1!=lod_id) && (dist < ps_r2_ndr)) continue;
#ifdef DETAILS_OPT
					if (dist > ps_r2_details_opt.z)
					{
						if (!(*_iI)->need_to_render_anyway[2])
							continue;
					} else if (dist > ps_r2_details_opt.y)
					{
						if (!(*_iI)->need_to_render_anyway[1])
							continue;
					} else if (dist > ps_r2_details_opt.x)
					{
						if (!(*_iI)->need_to_render_anyway[0])
							continue;
					}
#endif
					SlotItem&	Instance	= **_iI;
					u32			base		= dwBatch*2;

					// Build matrix ( 3x4 matrix, last row - color )
					float		scale		= Instance.scale_calculated;
					Fmatrix&	M			= Instance.mRotY;
					float		h			= Instance.c_hemi;
					float		s			= Instance.c_sun;
/*					c_storage[base+0].set	(M._11*scale,	M._21*scale,	M._31*scale,	M._41	);
					c_storage[base+1].set	(M._12*scale,	M._22*scale,	M._32*scale,	M._42	);
					c_storage[base+2].set	(M._13*scale,	M._23*scale,	M._33*scale,	M._43	);
					c_storage[base+3].set	(s,				s,				s,				h		);*/

					c_storage[base+0].set	(M._11*scale,	M._31*scale,	s,		h	);
					c_storage[base+1].set	(M._41,			M._42,		M._43,	scale	);

					dwBatch	++;
					if (dwBatch == hw_BatchSize)	{
						// flush
						Device.Statistic->RenderDUMP_DT_Count					+=	dwBatch;
						u32 dwCNT_verts;
						u32 dwCNT_prims;
						dwCNT_verts			= dwBatch * Object.number_vertices;
						dwCNT_prims			= (dwBatch * Object.number_indices)/3;
						RCache.get_ConstantCache_Vertex().b_dirty				=	TRUE;
						RCache.get_ConstantCache_Vertex().get_array_f().dirty	(c_base,c_base+dwBatch*2);
						RCache.Render			(D3DPT_TRIANGLELIST,vOffset, 0, dwCNT_verts,iOffset,dwCNT_prims);
						RCache.stat.r.s_details.add	(dwCNT_verts);

						// restart
						dwBatch					= 0;
						counter++;
						vert_counter += dwCNT_verts;
					}
				}
			}
			// flush if nessecary
			if (dwBatch)
			{
				Device.Statistic->RenderDUMP_DT_Count	+= dwBatch;
				u32 dwCNT_verts;
				u32 dwCNT_prims;
				dwCNT_verts			= dwBatch * Object.number_vertices;
				dwCNT_prims			= (dwBatch * Object.number_indices)/3;
				RCache.get_ConstantCache_Vertex().b_dirty				=	TRUE;
				RCache.get_ConstantCache_Vertex().get_array_f().dirty	(c_base,c_base+dwBatch*2);
				RCache.Render				(D3DPT_TRIANGLELIST,vOffset,0,dwCNT_verts,iOffset,dwCNT_prims);
				RCache.stat.r.s_details.add	(dwCNT_verts);

				// restart
				dwBatch					= 0;
				counter++;
				vert_counter += dwCNT_verts;
			}
			// Clean up
			if ((ps_r2_ls_flags.test(R2FLAG_SUN_DETAILS) && (RImplementation.PHASE_SMAP == RImplementation.phase))										// phase smap with shadows
			|| (ps_r2_ls_flags.test(R2FLAG_SUN_DETAILS) && (RImplementation.PHASE_NORMAL == RImplementation.phase) && (!RImplementation.is_sun()))		// phase normal with shadows without sun
			|| (!ps_r2_ls_flags.test(R2FLAG_SUN_DETAILS) && (RImplementation.PHASE_NORMAL == RImplementation.phase)))	
				vis.clear_not_free			();
		}
		vOffset		+=	hw_BatchSize * Object.number_vertices;
		iOffset		+=	hw_BatchSize * Object.number_indices;
	}
//	Msg("hw_render_dump var_id %d, DIP count %d, VERT count %d", var_id, counter, vert_counter);
}
void CDetailManager::hw_Render_dump_lod		(ref_constant x_array, u32 c_offset)
{
	Device.Statistic->RenderDUMP_DT_Count	= 0;

	// Matrices and offsets
	u32		vOffset	=	0;
	u32		iOffset	=	0;

	u32		batchsize = 1.4 * hw_BatchSize;

	vis_list& list	=	m_visibles	[0];
/*
	CEnvDescriptor&	desc = g_pGamePersistent->Environment().CurrentEnv;
	Fvector					c_sun,c_ambient,c_hemi;
	c_sun.set				(desc.sun_color.x,	desc.sun_color.y,	desc.sun_color.z);	c_sun.mul(.5f);
	c_ambient.set			(desc.ambient.x,	desc.ambient.y,		desc.ambient.z);
	c_hemi.set				(desc.hemi_color.x,	desc.hemi_color.y,	desc.hemi_color.z);
*/
	// Iterate
	u32 counter = 0;
	u32 vert_counter = 0;
	RCache.set_Element				(objects[0]->shader->E[1]);
	RImplementation.apply_lmaterial	();
	for (u32 O=0; O<objects.size(); O++){
		CDetail&	Object				= *objects	[O];
/*		if (Object.m_Flags.is(DO_NO_WAVING))
		{
			vOffset		+=	batchsize * 4;
			iOffset		+=	batchsize * 6;
			continue;
		}*/
		xr_vector <SlotItemVec* >& vis	= list		[O];
		if (!vis.empty()){
			// Setup matrices + colors (and flush it as nesessary)
			u32			c_base				= x_array->vs.index;
			Fvector4*	c_storage			= RCache.get_ConstantCache_Vertex().get_array_f().access(c_base);

			u32 dwBatch	= 0;

			xr_vector <SlotItemVec* >::iterator _vI = vis.begin();
			xr_vector <SlotItemVec* >::iterator _vE = vis.end();
			for (; _vI!=_vE; _vI++){
				SlotItemVec*	items		= *_vI;
				SlotItemVecIt _iI			= items->begin();
				SlotItemVecIt _iE			= items->end();
				for (; _iI!=_iE; _iI++){
					if (!(*_iI))
					{
/*#pragma todo		("Don't forget to remove this in release!")
						Msg("[DETAILS MANAGER RENDER]: SlotItem ptr is broken! _iI=%d, *_iI=%d", (u32)(&(*_iI)), (u32)(*(&(*_iI))));*/
						continue;
					}
					
#ifdef DETAILS_OPT
					float dist = Device.vCameraPosition.distance_to((*_iI)->mRotY.c);
					if (dist > ps_r2_details_opt.z)
					{
						if (!(*_iI)->need_to_render_anyway[2])
							continue;
					} else if (dist > ps_r2_details_opt.y)
					{
						if (!(*_iI)->need_to_render_anyway[1])
							continue;
					} else if (dist > ps_r2_details_opt.x)
					{
						if (!(*_iI)->need_to_render_anyway[0])
							continue;
					}
#endif
					SlotItem&	Instance	= **_iI;
					u32			base		= dwBatch*2;

					// Build matrix ( 3x4 matrix, last row - color )
					float		scale		= Instance.scale_calculated;
					Fmatrix&	M			= Instance.mRotY;
					float		h			= Instance.c_hemi;
					float		s			= Instance.c_sun;
/*					c_storage[base+0].set	(M._11*scale,	M._21*scale,	M._31*scale,	M._41	);
					c_storage[base+1].set	(M._12*scale,	M._22*scale,	M._32*scale,	M._42	);
					c_storage[base+2].set	(M._13*scale,	M._23*scale,	M._33*scale,	M._43	);
					c_storage[base+3].set	(s,				s,				s,				h		);*/

					c_storage[base+0].set	(M._11*scale,	M._31*scale,	s,		h	);
					c_storage[base+1].set	(M._41,			M._42,		M._43,	scale	);

					dwBatch	++;
					if (dwBatch == batchsize)	{
						// flush
						Device.Statistic->RenderDUMP_DT_Count					+=	dwBatch;
						u32 dwCNT_verts;
						u32 dwCNT_prims;
						dwCNT_verts			= dwBatch * 4;
						dwCNT_prims			= (dwBatch * 6)/3;
						RCache.get_ConstantCache_Vertex().b_dirty				=	TRUE;
						RCache.get_ConstantCache_Vertex().get_array_f().dirty	(c_base,c_base+dwBatch*2);
						RCache.Render			(D3DPT_TRIANGLELIST,vOffset, 0, dwCNT_verts,iOffset,dwCNT_prims);
						RCache.stat.r.s_details.add	(dwCNT_verts);

						// restart
						dwBatch					= 0;
						counter++;
						vert_counter += dwCNT_verts;
					}
				}
			}
			// flush if nessecary
			if (dwBatch)
			{
				Device.Statistic->RenderDUMP_DT_Count	+= dwBatch;
				u32 dwCNT_verts;
				u32 dwCNT_prims;
				dwCNT_verts			= dwBatch * 4;
				dwCNT_prims			= (dwBatch * 6)/3;
				RCache.get_ConstantCache_Vertex().b_dirty				=	TRUE;
				RCache.get_ConstantCache_Vertex().get_array_f().dirty	(c_base,c_base+dwBatch*2);
				RCache.Render				(D3DPT_TRIANGLELIST,vOffset,0,dwCNT_verts,iOffset,dwCNT_prims);
				RCache.stat.r.s_details.add	(dwCNT_verts);

				// restart
				dwBatch					= 0;
				counter++;
				vert_counter += dwCNT_verts;
			}
			// Clean up
			if ((ps_r2_ls_flags.test(R2FLAG_SUN_DETAILS) && (RImplementation.PHASE_SMAP == RImplementation.phase))										// phase smap with shadows
			|| (ps_r2_ls_flags.test(R2FLAG_SUN_DETAILS) && (RImplementation.PHASE_NORMAL == RImplementation.phase) && (!RImplementation.is_sun()))		// phase normal with shadows without sun
			|| (!ps_r2_ls_flags.test(R2FLAG_SUN_DETAILS) && (RImplementation.PHASE_NORMAL == RImplementation.phase)))	
				vis.clear_not_free			();
		}
		vOffset		+=	batchsize * 4;
		iOffset		+=	batchsize * 6;
	}
//	Msg("hw_render_dump_lod DIP count %d, VERT count %d", counter, vert_counter);
}
#endif