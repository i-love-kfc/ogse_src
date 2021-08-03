#pragma once

#include "xrCommon.h"
#include "texture.h"
#include "shader.h"
#include "LensFlare.h"
#include "Effect_Thunderbolt.h"

const float DAY_LENGTH = 86400.0;

class CEnvAmbient;
class CEnvironment;

class /*__declspec(dllimport)*/ CEnvModifier
{
public:
	_vector3<float> position;
	float radius;
	float power;
	float far_plane;
	_vector3<float> fog_color;
	float fog_density;
	_vector3<float> ambient;
	_vector3<float> sky_color;
	_vector3<float> hemi_color;
};
class /*__declspec(dllimport)*/ CEnvDescriptor
{
public:
	struct SEnvParamsEx
	{
		_vector3<float> rain_color;
		float sun_shafts;
		float sun_shafts_length;
		float rain_max_drop_angle;
		float rain_increase_speed;
		float moon_road_intensity;
	};
	float exec_time;
	float exec_time_loaded;
	shared_str sky_texture_name;
	shared_str sky_texture_env_name;
	shared_str clouds_texture_name;
	ref_texture sky_texture;
	ref_texture sky_texture_env;
	ref_texture clouds_texture;
	_vector4<float> clouds_color;
	_vector3<float> sky_color;
	float sky_rotation;
	float far_plane;
	_vector3<float> fog_color;
	float fog_density;
	float fog_distance;
	float rain_density;
	SEnvParamsEx *params_ex;
	int stab1;
	int stab2;
	//_vector3<float> rain_color;
	float bolt_period;
	float bolt_duration;
	float wind_velocity;
	float wind_direction;
	_vector3<float> ambient;
	_vector4<float> hemi_color;
	_vector3<float> sun_color;
	_vector3<float> sun_dir;
	int lens_flare_id;
	int tb_id;
	CEnvAmbient *env_ambient;
	//__declspec(dllexport)
	CEnvDescriptor();
	//__declspec(dllexport)
	~CEnvDescriptor();
	//__declspec(dllexport)
	CEnvDescriptor& operator=(class CEnvDescriptor const &);
	//CEnvDescriptor* CEnvDescriptor_repl(void);
	void load(const char* exec_tm, const char* sect, CEnvironment* parent);
	void on_device_create();
	void on_device_destroy();
};
class CEnvDescriptorMixer : public CEnvDescriptor
{
public:
	STextureList sky_r_textures;
	STextureList sky_r_textures_env;
	STextureList clouds_r_textures;
	float weight;
	float fog_near;
	float fog_far;
	//__declspec(dllexport)
	void lerp(CEnvironment* , CEnvDescriptor& A, CEnvDescriptor& B, float f, CEnvModifier& M, float m_power);
		//
};
class CPerlinNoise1D;
//class Shader;
struct SGeometry;
class CEffect_Rain;
class CLensFlare;
class CEffect_Thunderbolt;

class /*__declspec(dllimport)*/ CEnvironment {
public:
	template class std::map<shared_str,xr_vector<CEnvDescriptor *,xalloc<CEnvDescriptor *> >,std::less<shared_str>,xalloc<std::pair<shared_str,xr_vector<CEnvDescriptor *,xalloc<CEnvDescriptor *> > > > >;

	typedef std::binary_function<shared_str,shared_str,bool> str_pred;
	xr_vector<_vector3<float> > CloudsVerts; // 0
	xr_vector<unsigned short> CloudsIndices; // 16
	//CBlender_skybox m_b_skybox; //32 (324) компилятор даёт 328
	char m_b_skybox[324]; //32 (324) компилятор даёт 328
	CPerlinNoise1D *PerlinNoise1D; // 356
	float fGameTime;
	float wind_strength_factor;
	float wind_gust_factor;
	CEnvDescriptorMixer CurrentEnv;
	CEnvDescriptor *Current[2];
	bool bWFX;
	float wfx_time;
	CEnvDescriptor *WFX_end_desc[2];
	xr_vector<CEnvDescriptor *> *CurrentWeather;
	shared_str CurrentWeatherName;
	shared_str CurrentCycleName;
	xr_map<shared_str,xr_vector<CEnvDescriptor *> > WeatherCycles; // 12 байт
	xr_map<shared_str,xr_vector<CEnvDescriptor *> > WeatherFXs; // 12 байт
	xr_vector<CEnvModifier> Modifiers;
	xr_vector<CEnvAmbient*> Ambients;
	resptr_core<Shader,resptrcode_shader> sh_2sky;
	resptr_core<SGeometry,resptrcode_geom> sh_2geom;
	resptr_core<Shader,resptrcode_shader> clouds_sh;
	resptr_core<SGeometry,resptrcode_geom> clouds_geom;
	CEffect_Rain *eff_Rain;
	CLensFlare *eff_LensFlare;
	CEffect_Thunderbolt *eff_Thunderbolt;
	float fTimeFactor;
	resptr_core<CTexture,resptrcode_texture> tonemap;
	resptr_core<CTexture,resptrcode_texture> tsky0;
	resptr_core<CTexture,resptrcode_texture> tsky1;
public:
	//__declspec(dllexport)
	void load(void);
	void unload();
	CEnvAmbient* AppendEnvAmb(const shared_str & sect);
	//__declspec(dllexport)
	void Invalidate(void);
	//__declspec(dllexport)
	void SetWeather(shared_str name, bool forced);
private:
	void SelectEnvs(xr_vector<CEnvDescriptor *>* envs, CEnvDescriptor*& e0, CEnvDescriptor*& e1, float gt);
public:
	void ReselectEnvs(void);
	void					RenderSky			();
	void					RenderClouds		();
	void					RenderFlares		();
	void					RenderLast			();
};
