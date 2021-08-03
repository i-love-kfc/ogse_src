#include "ResourceManager.h"
#include "Backend.h"
#include "ETextureParams.h"

float					r__dtex_range	= 50;
void fix_texture_thm_name(LPSTR fn)
{
	LPSTR _ext = strext(fn);
	if(  _ext					&&
	  (0==stricmp(_ext,".tga")	||
		0==stricmp(_ext,".thm")	||
		0==stricmp(_ext,".dds")	||
		0==stricmp(_ext,".bmp")	||
		0==stricmp(_ext,".ogm")	) )
		*_ext = 0;
}
void __cdecl CTextureDescrMngr::LoadLTX()
{
	string_path				fname;		
	FS.update_path			(fname,"$game_textures$","textures.ltx");

	if (FS.exist(fname))
	{
		CInifile			ini(fname);
		if (ini.section_exist("association"))
		{
			CInifile::Sect& data	= ini.r_section("association");
			CInifile::SectCIt I		= data.Data.begin();
			CInifile::SectCIt E		= data.Data.end();
			for ( ; I!=E; ++I)	
			{
				const CInifile::Item& item	= *I;

				texture_desc& desc		= m_texture_details[item.first];
				desc.m_assoc			= xr_new<texture_assoc>();

				string_path				T;
				float					s;

				int res = sscanf					(*item.second,"%[^,],%f",T,&s);
				R_ASSERT(res==2);
				desc.m_assoc->detail_name = T;
				desc.m_assoc->cs		= xr_new<cl_dt_scaler>(s);
				desc.m_assoc->usage		= 0;
				if(strstr(item.second.c_str(),"usage[diffuse_or_bump]"))
					desc.m_assoc->usage	= (1<<0)|(1<<1);
				else
				if(strstr(item.second.c_str(),"usage[bump]"))
					desc.m_assoc->usage	= (1<<1);
				else
				if(strstr(item.second.c_str(),"usage[diffuse]"))
					desc.m_assoc->usage	= (1<<0);
			}
		}//"association"

		if (ini.section_exist("specification"))
		{
			CInifile::Sect& 	sect = ini.r_section("specification");
			for (CInifile::SectCIt I2=sect.Data.begin(); I2!=sect.Data.end(); ++I2)	
			{
				const CInifile::Item& item	= *I2;

				texture_desc& desc		= m_texture_details[item.first];
				desc.m_spec				= xr_new<texture_spec>();

				string_path				bmode, bparallax;
				int res = sscanf		(item.second.c_str(),"bump_mode[%[^]]], material[%f], parallax[%[^]]]",bmode,&desc.m_spec->m_material,bparallax);
//				Msg("reading %s", item.second.c_str());
				R_ASSERT(res>=2);
				if ((bmode[0]=='u')&&(bmode[1]=='s')&&(bmode[2]=='e')&&(bmode[3]==':'))
				{
					// bump-map specified
					desc.m_spec->m_bump_name	=	bmode+4;
				}
				if (res == 3)
				{
					if ((bparallax[0]=='y')&&(bparallax[1]=='e')&&(bparallax[2]=='s'))
					{
						// parallax
						desc.m_spec->bParallax = TRUE;
					} else {
						desc.m_spec->bParallax = FALSE;
					}
				} else {
					desc.m_spec->bParallax = FALSE;
				}
			}
		}//"specification"
	}//file-exist
}
void __cdecl CTextureDescrMngr::LoadTHM()
{
	FS_FileSet				flist;
	FS.file_list			(flist,"$game_textures$",FS_ListFiles,"*.thm");
	Msg						("count of .thm files=%d", flist.size());
	FS_FileSetIt It			= flist.begin();
	FS_FileSetIt It_e		= flist.end();
	STextureParams			tp;
	string_path				fn;
	for(;It!=It_e;++It)
	{
		
		FS.update_path		(fn,"$game_textures$", (*It).name.c_str());
		IReader* F			= FS.r_open(fn);
		strcpy_s				(fn,(*It).name.c_str());
		fix_texture_thm_name(fn);

		R_ASSERT			(F->find_chunk(THM_CHUNK_TYPE));
		F->r_u32			();
		tp.Clear			();
		tp.Load				(*F);
		FS.r_close			(F);
		if (STextureParams::ttImage		== tp.fmt ||
			STextureParams::ttTerrain	== tp.fmt ||
			STextureParams::ttNormalMap	== tp.fmt	)
		{

			texture_desc& desc		 = m_texture_details[fn];

			if( tp.detail_name.size() &&
				tp.flags.is_any(STextureParams::flDiffuseDetail|STextureParams::flBumpDetail) )
			{
				if(desc.m_assoc)
					xr_delete				(desc.m_assoc);

				desc.m_assoc				= xr_new<texture_assoc>();
				desc.m_assoc->detail_name	= tp.detail_name;
				desc.m_assoc->cs			= xr_new<cl_dt_scaler>(tp.detail_scale);
				desc.m_assoc->usage			= 0;
					
				if( tp.flags.is(STextureParams::flDiffuseDetail) )
					desc.m_assoc->usage		|= (1<<0);
					
				if( tp.flags.is(STextureParams::flBumpDetail) )
					desc.m_assoc->usage		|= (1<<1);

			}
			if(desc.m_spec)
				xr_delete				(desc.m_spec);

			desc.m_spec					= xr_new<texture_spec>();
			desc.m_spec->m_material		= tp.material+tp.material_weight;
				
			if(tp.bump_mode==STextureParams::tbmUse)
				desc.m_spec->m_bump_name	= tp.bump_name;
		}
	}
}