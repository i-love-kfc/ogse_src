//для статических библиотек

#pragma comment(lib, "user32.lib")

#pragma comment(lib, "xr_3da.lib")
#pragma comment(lib, "xrCore.lib")
#pragma comment(lib, "xrCDB.lib")
#pragma comment(lib, "xrLua_GSC.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "xrRender_R2.lib")
#pragma comment(lib, "d3dcompiler_47.lib")

#pragma comment(linker, "/export:luaopen_ogse=_luaopen_ogse")

#pragma comment(linker, "/export:CEffectorZoomInertion__Process=?Process@CEffectorZoomInertion@@QAEHAAU?$_vector3@M@@00AAM11@Z")
#pragma comment(linker, "/export:CCameraManager__Update3=?Update3@CCameraManager@@QAEXPBVCCameraBase@@@Z")
#pragma comment(linker, "/export:CEnvironment__load=?load@CEnvironment@@QAEXXZ")
#pragma comment(linker, "/export:CEnvDescriptor__CEnvDescriptor=??0CEnvDescriptor@@QAE@XZ")
#pragma comment(linker, "/export:CEnvDescriptor___CEnvDescriptor=??1CEnvDescriptor@@QAE@XZ")
#pragma comment(linker, "/export:CEnvDescriptor__load=?load@CEnvDescriptor@@QAEXPBD0PAVCEnvironment@@@Z")
#pragma comment(linker, "/export:CEnvDescriptor__operator_=??4CEnvDescriptor@@QAEAAV0@ABV0@@Z")
#pragma comment(linker, "/export:CEnvDescriptorMixer__lerp=?lerp@CEnvDescriptorMixer@@QAEXPAVCEnvironment@@AAVCEnvDescriptor@@1MAAVCEnvModifier@@M@Z")
#pragma comment(linker, "/export:CEffect_Rain__Born=?Born@CEffect_Rain@@AAEXAAUItem@1@M@Z")
#pragma comment(linker, "/export:CLensFlareDescriptor__load=?load@CLensFlareDescriptor@@QAEXPAVCInifile@@PBD@Z")
#pragma comment(linker, "/export:CLensFlare__Render=?Render@CLensFlare@@QAIXHHH@Z")
#pragma comment(linker, "/export:CLensFlare__OnFrame=?OnFrame@CLensFlare@@QAEXH@Z")
#pragma comment(linker, "/export:CEnvironment__Invalidate=?Invalidate@CEnvironment@@QAEXXZ")
		// CRenderTarget override
#pragma comment(linker, "/export:CRenderTarget__phase_combine=?phase_combine@CRenderTarget@@QAEXXZ")
#pragma comment(linker, "/export:CRenderTarget__CRenderTarget=??0CRenderTarget@@QAE@XZ")
#pragma comment(linker, "/export:CRenderTarget___CRenderTarget=??1CRenderTarget@@UAE@XZ")
#pragma comment(linker, "/export:CRender__create=?create@CRender@@QAEXXZ")
#pragma comment(linker, "/export:CRender__reset_begin=?reset_begin@CRender@@QAEXXZ")
#pragma comment(linker, "/export:CRender__reset_end=?reset_end@CRender@@QAEXXZ")
		// defines for shaders
#pragma comment(linker, "/export:CRender__shader_compile=?shader_compile@CRender@@QAEJPBD0IPAX100K111@Z")
		// console commands
#pragma comment(linker, "/export:xrRender_initconsole=?xrRender_initconsole@@YAXXZ")
		// detail radius
#pragma comment(linker, "/export:CRender__level_Load=?level_Load@CRender@@QAEXPAVIReader@@@Z")
#pragma comment(linker, "/export:CRender__level_Unload=?level_Unload@CRender@@QAEXXZ")
#pragma comment(linker, "/export:CDetailManager__MT_CALC=?MT_CALC@CDetailManager@@QAGXXZ")
#pragma comment(linker, "/export:CDetailManager__Render=?Render@CDetailManager@@QAEXXZ")
		// global uniforms for shadrs
#pragma comment(linker, "/export:CBlender_Compile__SetMapping=?SetMapping@CBlender_Compile@@QAEXXZ")
		// bloodmarks
#pragma comment(linker, "/export:CRender__Add_SkeletonWallmark=?add_SkeletonWallmark@CRender@@QAEXPBU?$_matrix@M@@PAVCKinematics@@AAV?$resptr_core@UShader@@Uresptrcode_shader@@@@ABU?$_vector3@M@@3M@Z")
#pragma comment(linker, "/export:CRender__Add_SkeletonWallmark_ptr=?add_SkeletonWallmark@CRender@@QAEXV?$intrusive_ptr@VCSkeletonWallmark@@Uintrusive_base@@@@@Z")
		// timers for ikvision
#pragma comment(linker, "/export:CObject__UpdateCL=?UpdateCL@CObject@@QAEXXZ")
		// correct sun shadows
#pragma comment(linker, "/export:CLight_DB__Update=?Update@CLight_DB@@QAEXXZ")
		// additional samplers for detail bump
#pragma comment(linker, "/export:uber_deffer=?uber_deffer@@YAXAAVCBlender_Compile@@_NPBD2H21@Z")
		// clear target fix
#pragma comment(linker, "/export:CRenderTarget__phase_scene_prepare=?phase_scene_prepare@CRenderTarget@@QAEXXZ")
		// rain render overload
#pragma comment(linker, "/export:CEffect_Rain__Render=?Render@CEffect_Rain@@QAEXXZ")
		// blender creating
#pragma comment(linker, "/export:CRender__blender_create=?blender_create@CRender@@QAEPAVIBlender@@_K@Z")
		// blender destroying
#pragma comment(linker, "/export:CRender__blender_destroy=?blender_destroy@CRender@@QAEXAAPAVIBlender@@@Z")
		// spot lights rendering
#pragma comment(linker, "/export:CRenderTarget__accum_spot=?accum_spot@CRenderTarget@@QAEXPAVlight@@@Z")
		// point lights rendering
#pragma comment(linker, "/export:CRenderTarget__accum_point=?accum_point@CRenderTarget@@QAEXPAVlight@@@Z")
		// dump pp effector
#pragma comment(linker, "/export:print_pp_effect=?print_pp_effect@@YAXPAVCCameraManager@@@Z")
		// gi restoring
//#pragma comment(linker, "/export:light__export=?export@light@@QAEXAAVlight_Package@@@Z")
//#pragma comment(linker, "/export:CRender__light_create=?light_create@CRender@@QAEPAVIRender_Light@@XZ")
#pragma comment(linker, "/export:CRender__render_indirect=?render_indirect@CRender@@QAEXPAVlight@@@Z")
		//
#pragma comment(linker, "/export:CCar__cam_Update=?cam_Update@CCar@@AAEXMM@Z")
		// cool sunshafts :)
#pragma comment(linker, "/export:CRenderTarget__accum_direct=?accum_direct@CRenderTarget@@QAEXI@Z")
		// overloading textures description loading
#pragma comment(linker, "/export:CTextureDescrMngr__LoadLTX=?LoadLTX@CTextureDescrMngr@@AAAXXZ")
#pragma comment(linker, "/export:CTextureDescrMngr__LoadTHM=?LoadTHM@CTextureDescrMngr@@AAAXXZ")
		// pcss
#pragma comment(linker, "/export:CRenderTarget__phase_smap_spot=?phase_smap_spot@CRenderTarget@@QAEXPAVlight@@@Z")
#pragma comment(linker, "/export:CRenderTarget__phase_smap_direct=?phase_smap_direct@CRenderTarget@@QAEXPAVlight@@I@Z")
#pragma comment(linker, "/export:CRenderTarget__phase_accumulator=?phase_accumulator@CRenderTarget@@QAEXXZ")
#pragma comment(linker, "/export:CRenderTarget__accum_volumetric_spot=?accum_volumetric_spot@CRenderTarget@@QAEXPAVlight@@@Z")
//#pragma comment(linker, "/export:R_dsgraph_structure__r_dsgraph_render_lods=?r_dsgraph_render_lods@R_dsgraph_structure@@QAEX_N0@Z")
//#pragma comment(linker, "/export:FProgressive__Render=?Render@FProgressive@@QAEXM@Z")
