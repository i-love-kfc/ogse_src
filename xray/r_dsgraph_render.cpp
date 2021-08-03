#include "r2.h"
#include "Backend.h"

using namespace		R_dsgraph;

extern __declspec(dllimport) void __fastcall sorted_L1		(mapSorted_Node *N);

void	R_dsgraph_structure::r_dsgraph_render_distort	()
{
	// Sorted (back to front)
	mapDistort.traverseRL	(sorted_L1);
	mapDistort.clear		();
}
//////////////////////////////////////////////////////////////////////////
// strict-sorted render
void	R_dsgraph_structure::r_dsgraph_render_sorted	()
{
	// Sorted (back to front)
	mapSorted.traverseRL	(sorted_L1);
	mapSorted.clear			();
}
//////////////////////////////////////////////////////////////////////////
// strict-sorted render
void	R_dsgraph_structure::r_dsgraph_render_emissive	()
{
	// Sorted (back to front)
	mapEmissive.traverseLR	(sorted_L1);
	mapEmissive.clear		();
}
/*
ICF float calcLOD	(float ssa/*fDistSq*//*, float R)
{
	return			_sqrt(clampr((ssa - r_ssaGLOD_end)/(r_ssaGLOD_start-r_ssaGLOD_end),0.f,1.f));
}

// NORMAL
IC	bool	cmp_normal_items		(const _NormalItem& N1, const _NormalItem& N2)
{	return (N1.ssa > N2.ssa);		}

void __fastcall mapNormal_Render	(mapNormalItems& N)
{
	// *** DIRECT ***
	std::sort				(N.begin(),N.end(),cmp_normal_items);
	_NormalItem				*I=&*N.begin(), *E = &*N.end();
	for (; I!=E; I++)		{
		_NormalItem&		Ni	= *I;
		Ni.pVisual->Render	(calcLOD(Ni.ssa,Ni.pVisual->vis.sphere.R));
	}
}
void __declspec(dllimport) __fastcall mapMatrix_Render	(mapMatrixItems& N);
/*
// Matrix
IC	bool	cmp_matrix_items		(const _MatrixItem& N1, const _MatrixItem& N2)
{	return (N1.ssa > N2.ssa);		}

void __fastcall mapMatrix_Render	(mapMatrixItems& N)
{
	// *** DIRECT ***
	std::sort				(N.begin(),N.end(),cmp_matrix_items);
	_MatrixItem				*I=&*N.begin(), *E = &*N.end();
	for (; I!=E; I++)		{
		_MatrixItem&	Ni				= *I;
		RCache.set_xform_world			(Ni.Matrix);
		RImplementation.apply_object	(Ni.pObject);
		RImplementation.apply_lmaterial	();
		Ni.pVisual->Render				(calcLOD(Ni.ssa,Ni.pVisual->vis.sphere.R));
	}
	N.clear	();
}

IC	bool	cmp_vs_nrm			(mapNormalVS::TNode* N1, mapNormalVS::TNode* N2)			{	return (N1->val.ssa > N2->val.ssa);		}
IC	bool	cmp_vs_mat			(mapMatrixVS::TNode* N1, mapMatrixVS::TNode* N2)			{	return (N1->val.ssa > N2->val.ssa);		}

IC	bool	cmp_ps_nrm			(mapNormalPS::TNode* N1, mapNormalPS::TNode* N2)			{	return (N1->val.ssa > N2->val.ssa);		}
IC	bool	cmp_ps_mat			(mapMatrixPS::TNode* N1, mapMatrixPS::TNode* N2)			{	return (N1->val.ssa > N2->val.ssa);		}

IC	bool	cmp_cs_nrm			(mapNormalCS::TNode* N1, mapNormalCS::TNode* N2)			{	return (N1->val.ssa > N2->val.ssa);		}
IC	bool	cmp_cs_mat			(mapMatrixCS::TNode* N1, mapMatrixCS::TNode* N2)			{	return (N1->val.ssa > N2->val.ssa);		}

IC	bool	cmp_states_nrm		(mapNormalStates::TNode* N1, mapNormalStates::TNode* N2)	{	return (N1->val.ssa > N2->val.ssa);		}
IC	bool	cmp_states_mat		(mapMatrixStates::TNode* N1, mapMatrixStates::TNode* N2)	{	return (N1->val.ssa > N2->val.ssa);		}

IC	bool	cmp_textures_lex2_nrm	(mapNormalTextures::TNode* N1, mapNormalTextures::TNode* N2){	
	STextureList*	t1			= N1->key;
	STextureList*	t2			= N2->key;
	if ((*t1)[0] < (*t2)[0])	return true;
	if ((*t1)[0] > (*t2)[0])	return false;
	if ((*t1)[1] < (*t2)[1])	return true;
	else						return false;
}
IC	bool	cmp_textures_lex2_mat	(mapMatrixTextures::TNode* N1, mapMatrixTextures::TNode* N2){	
	STextureList*	t1			= N1->key;
	STextureList*	t2			= N2->key;
	if ((*t1)[0] < (*t2)[0])	return true;
	if ((*t1)[0] > (*t2)[0])	return false;
	if ((*t1)[1] < (*t2)[1])	return true;
	else						return false;
}
IC	bool	cmp_textures_lex3_nrm	(mapNormalTextures::TNode* N1, mapNormalTextures::TNode* N2){	
	STextureList*	t1			= N1->key;
	STextureList*	t2			= N2->key;
	if ((*t1)[0] < (*t2)[0])	return true;
	if ((*t1)[0] > (*t2)[0])	return false;
	if ((*t1)[1] < (*t2)[1])	return true;
	if ((*t1)[1] > (*t2)[1])	return false;
	if ((*t1)[2] < (*t2)[2])	return true;
	else						return false;
}
IC	bool	cmp_textures_lex3_mat	(mapMatrixTextures::TNode* N1, mapMatrixTextures::TNode* N2){	
	STextureList*	t1			= N1->key;
	STextureList*	t2			= N2->key;
	if ((*t1)[0] < (*t2)[0])	return true;
	if ((*t1)[0] > (*t2)[0])	return false;
	if ((*t1)[1] < (*t2)[1])	return true;
	if ((*t1)[1] > (*t2)[1])	return false;
	if ((*t1)[2] < (*t2)[2])	return true;
	else						return false;
}
IC	bool	cmp_textures_lexN_nrm	(mapNormalTextures::TNode* N1, mapNormalTextures::TNode* N2){	
	STextureList*	t1			= N1->key;
	STextureList*	t2			= N2->key;
	return std::lexicographical_compare(t1->begin(),t1->end(),t2->begin(),t2->end());
}
IC	bool	cmp_textures_lexN_mat	(mapMatrixTextures::TNode* N1, mapMatrixTextures::TNode* N2){	
	STextureList*	t1			= N1->key;
	STextureList*	t2			= N2->key;
	return std::lexicographical_compare(t1->begin(),t1->end(),t2->begin(),t2->end());
}
IC	bool	cmp_textures_ssa_nrm	(mapNormalTextures::TNode* N1, mapNormalTextures::TNode* N2){	
	return (N1->val.ssa > N2->val.ssa);		
}
IC	bool	cmp_textures_ssa_mat	(mapMatrixTextures::TNode* N1, mapMatrixTextures::TNode* N2){	
	return (N1->val.ssa > N2->val.ssa);		
}

void		sort_tlist_nrm			
(
 xr_vector<mapNormalTextures::TNode*,render_alloc<mapNormalTextures::TNode*> >& lst, 
 xr_vector<mapNormalTextures::TNode*,render_alloc<mapNormalTextures::TNode*> >& temp, 
 mapNormalTextures&					textures, 
 BOOL	bSSA
 )
{
	int amount			= textures.begin()->key->size();
	if (bSSA)	
	{
		if (amount<=1)
		{
			// Just sort by SSA
			textures.getANY_P			(lst);
			std::sort					(lst.begin(), lst.end(), cmp_textures_ssa_nrm);
		} 
		else 
		{
			// Split into 2 parts
			mapNormalTextures::TNode* _it	= textures.begin	();
			mapNormalTextures::TNode* _end	= textures.end		();
			for (; _it!=_end; _it++)	{
				if (_it->val.ssa > r_ssaHZBvsTEX)	lst.push_back	(_it);
				else								temp.push_back	(_it);
			}

			// 1st - part - SSA, 2nd - lexicographically
			std::sort					(lst.begin(),	lst.end(),	cmp_textures_ssa_nrm);
			if (2==amount)				std::sort	(temp.begin(),	temp.end(),	cmp_textures_lex2_nrm);
			else if (3==amount)			std::sort	(temp.begin(),	temp.end(),	cmp_textures_lex3_nrm);
			else						std::sort	(temp.begin(),	temp.end(),	cmp_textures_lexN_nrm);

			// merge lists
			lst.insert					(lst.end(),temp.begin(),temp.end());
		}
	}
	else 
	{
		textures.getANY_P			(lst);
		if (2==amount)				std::sort	(lst.begin(),	lst.end(),	cmp_textures_lex2_nrm);
		else if (3==amount)			std::sort	(lst.begin(),	lst.end(),	cmp_textures_lex3_nrm);
		else						std::sort	(lst.begin(),	lst.end(),	cmp_textures_lexN_nrm);
	}
}

void		sort_tlist_mat			
(
 xr_vector<mapMatrixTextures::TNode*,render_alloc<mapMatrixTextures::TNode*> >& lst,
 xr_vector<mapMatrixTextures::TNode*,render_alloc<mapMatrixTextures::TNode*> >& temp,
 mapMatrixTextures&					textures,
 BOOL	bSSA
 )
{
	int amount			= textures.begin()->key->size();
	if (bSSA)	
	{
		if (amount<=1)
		{
			// Just sort by SSA
			textures.getANY_P			(lst);
			std::sort					(lst.begin(), lst.end(), cmp_textures_ssa_mat);
		} 
		else 
		{
			// Split into 2 parts
			mapMatrixTextures::TNode* _it	= textures.begin	();
			mapMatrixTextures::TNode* _end	= textures.end		();
			for (; _it!=_end; _it++)	{
				if (_it->val.ssa > r_ssaHZBvsTEX)	lst.push_back	(_it);
				else								temp.push_back	(_it);
			}

			// 1st - part - SSA, 2nd - lexicographically
			std::sort					(lst.begin(),	lst.end(),	cmp_textures_ssa_mat);
			if (2==amount)				std::sort	(temp.begin(),	temp.end(),	cmp_textures_lex2_mat);
			else if (3==amount)			std::sort	(temp.begin(),	temp.end(),	cmp_textures_lex3_mat);
			else						std::sort	(temp.begin(),	temp.end(),	cmp_textures_lexN_mat);

			// merge lists
			lst.insert					(lst.end(),temp.begin(),temp.end());
		}
	}
	else 
	{
		textures.getANY_P			(lst);
		if (2==amount)				std::sort	(lst.begin(),	lst.end(),	cmp_textures_lex2_mat);
		else if (3==amount)			std::sort	(lst.begin(),	lst.end(),	cmp_textures_lex3_mat);
		else						std::sort	(lst.begin(),	lst.end(),	cmp_textures_lexN_mat);
	}
}

void R_dsgraph_structure::r_dsgraph_render_graph	(u32	_priority, bool _clear)
{
	Device.Statistic->RenderDUMP.Begin		();

	// **************************************************** NORMAL
	// Perform sorting based on ScreenSpaceArea
	// Sorting by SSA and changes minimizations
	{
		RCache.set_xform_world			(Fidentity);
		mapNormalVS&	vs				= mapNormal	[_priority];
		vs.getANY_P						(nrmVS);
		std::sort						(nrmVS.begin(), nrmVS.end(), cmp_vs_nrm);
		for (u32 vs_id=0; vs_id<nrmVS.size(); vs_id++)
		{
			mapNormalVS::TNode*	Nvs			= nrmVS[vs_id];
			RCache.set_VS					(Nvs->key);	

			mapNormalPS&		ps			= Nvs->val;		ps.ssa	= 0;
			ps.getANY_P						(nrmPS);
			std::sort						(nrmPS.begin(), nrmPS.end(), cmp_ps_nrm);
			for (u32 ps_id=0; ps_id<nrmPS.size(); ps_id++)
			{
				mapNormalPS::TNode*	Nps			= nrmPS[ps_id];
				RCache.set_PS					(Nps->key);	

				mapNormalCS&		cs			= Nps->val;		cs.ssa	= 0;
				cs.getANY_P						(nrmCS);
				std::sort						(nrmCS.begin(), nrmCS.end(), cmp_cs_nrm);
				for (u32 cs_id=0; cs_id<nrmCS.size(); cs_id++)
				{
					mapNormalCS::TNode*	Ncs			= nrmCS[cs_id];
					RCache.set_Constants			(Ncs->key);

					mapNormalStates&	states		= Ncs->val;		states.ssa	= 0;
					states.getANY_P					(nrmStates);
					std::sort						(nrmStates.begin(), nrmStates.end(), cmp_states_nrm);
					for (u32 state_id=0; state_id<nrmStates.size(); state_id++)
					{
						mapNormalStates::TNode*	Nstate		= nrmStates[state_id];
						RCache.set_States					(Nstate->key);

						mapNormalTextures&		tex			= Nstate->val;	tex.ssa =	0;
						sort_tlist_nrm						(nrmTextures,nrmTexturesTemp,tex,true);
						for (u32 tex_id=0; tex_id<nrmTextures.size(); tex_id++)
						{
							mapNormalTextures::TNode*	Ntex	= nrmTextures[tex_id];
							RCache.set_Textures					(Ntex->key);
							RImplementation.apply_lmaterial		();

							mapNormalItems&				items	= Ntex->val;		items.ssa	= 0;
							mapNormal_Render					(items);
							if (_clear)				items.clear	();
						}
						nrmTextures.clear		();
						nrmTexturesTemp.clear	();
						if(_clear) tex.clear	();
					}
					nrmStates.clear			();
					if(_clear) states.clear	();
				}
				nrmCS.clear				();
				if(_clear) cs.clear		();
			}
			nrmPS.clear				();
			if(_clear) ps.clear		();
		}
		nrmVS.clear				();
		if(_clear) vs.clear		();
	}

	// **************************************************** MATRIX
	// Perform sorting based on ScreenSpaceArea
	// Sorting by SSA and changes minimizations
	{
		mapMatrixVS&	vs				= mapMatrix	[_priority];
		vs.getANY_P						(matVS);
		std::sort						(matVS.begin(), matVS.end(), cmp_vs_mat);
		for (u32 vs_id=0; vs_id<matVS.size(); vs_id++)	{
			mapMatrixVS::TNode*	Nvs			= matVS[vs_id];
			RCache.set_VS					(Nvs->key);	

			mapMatrixPS&		ps			= Nvs->val;		ps.ssa	= 0;
			ps.getANY_P						(matPS);
			std::sort						(matPS.begin(), matPS.end(), cmp_ps_mat);
			for (u32 ps_id=0; ps_id<matPS.size(); ps_id++)
			{
				mapMatrixPS::TNode*	Nps			= matPS[ps_id];
				RCache.set_PS					(Nps->key);	

				mapMatrixCS&		cs			= Nps->val;		cs.ssa	= 0;
				cs.getANY_P						(matCS);
				std::sort						(matCS.begin(), matCS.end(), cmp_cs_mat);
				for (u32 cs_id=0; cs_id<matCS.size(); cs_id++)
				{
					mapMatrixCS::TNode*	Ncs			= matCS[cs_id];
					RCache.set_Constants			(Ncs->key);

					mapMatrixStates&	states		= Ncs->val;		states.ssa	= 0;
					states.getANY_P					(matStates);
					std::sort						(matStates.begin(), matStates.end(), cmp_states_mat);
					for (u32 state_id=0; state_id<matStates.size(); state_id++)
					{
						mapMatrixStates::TNode*	Nstate		= matStates[state_id];
						RCache.set_States					(Nstate->key);

						mapMatrixTextures&		tex			= Nstate->val;	tex.ssa =	0;
						sort_tlist_mat						(matTextures,matTexturesTemp,tex,true);
						for (u32 tex_id=0; tex_id<matTextures.size(); tex_id++)
						{
							mapMatrixTextures::TNode*	Ntex	= matTextures[tex_id];
							RCache.set_Textures					(Ntex->key);
							RImplementation.apply_lmaterial		();

							mapMatrixItems&				items	= Ntex->val;		items.ssa	= 0;
							mapMatrix_Render					(items);
						}
						matTextures.clear		();
						matTexturesTemp.clear	();
						if(_clear) tex.clear	();
					}
					matStates.clear			();
					if(_clear) states.clear	();
				}
				matCS.clear				();
				if(_clear) cs.clear		();
			}
			matPS.clear				();
			if(_clear) ps.clear		();
		}
		matVS.clear				();
		if(_clear) vs.clear		();
	}

	Device.Statistic->RenderDUMP.End	();
}

//////////////////////////////////////////////////////////////////////////
// sub-space rendering - shortcut to render with frustum extracted from matrix
/*
void	R_dsgraph_structure::r_dsgraph_render_subspace	(IRender_Sector* _sector, Fmatrix& mCombined, Fvector& _cop, BOOL _dynamic, BOOL _precise_portals)
{
	CFrustum	temp;
	temp.CreateFromMatrix			(mCombined,	FRUSTUM_P_ALL);
	r_dsgraph_render_subspace		(_sector,&temp,mCombined,_cop,_dynamic,_precise_portals);
}*/