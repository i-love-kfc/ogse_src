#include "Environment.h"
#include "RenderDevice.h"
#include "xrCommon.h"

template<typename Value1, typename Value2>
inline std::pair<Value1, Value2> make_pair(Value1 first, Value2 second)
{
	return (std::pair<Value1, Value2>(first, second));
};

extern __declspec(dllimport) float psVisDistance;

#define	CHECK_VECTOR(C)	if (C.x<0 || C.x>2 || C.y<0 || C.y>2 || C.z<0 || C.z>2)	{ Msg("! Invalid '%s' in env-section '%s'",#C,S);}

CEnvDescriptor& CEnvDescriptor::operator=(class CEnvDescriptor const & __that)
{
	//Log("CEnvDescriptor& CEnvDescriptor::operator=");
	exec_time = __that.exec_time;
	exec_time_loaded = __that.exec_time_loaded;
	sky_texture_name = __that.sky_texture_name;
	sky_texture_env_name = __that.sky_texture_env_name;
	clouds_texture_name = __that.clouds_texture_name;
	
	sky_texture = __that.sky_texture;
	sky_texture_env = __that.sky_texture_env;
	clouds_texture = __that.clouds_texture;
	
	clouds_color = __that.clouds_color;
	
	sky_color = __that.sky_color;
	
	sky_rotation = __that.sky_rotation;
	
	far_plane = __that.far_plane;
	
	fog_color = __that.fog_color;
	
	fog_density = __that.fog_density;
	fog_distance = __that.fog_distance;
	
	rain_density = __that.rain_density;
	
	//-------------------
	//rain_color = __that.rain_color;
	params_ex = (SEnvParamsEx*)Memory.mem_alloc(sizeof(SEnvParamsEx));
	params_ex->rain_color = __that.params_ex->rain_color;
	params_ex->rain_max_drop_angle = __that.params_ex->rain_max_drop_angle;
	params_ex->sun_shafts = __that.params_ex->sun_shafts;
	params_ex->sun_shafts_length = __that.params_ex->sun_shafts_length;
	params_ex->rain_increase_speed = __that.params_ex->rain_increase_speed;
	params_ex->moon_road_intensity = __that.params_ex->moon_road_intensity;
	//-------------------
	
	bolt_period = __that.bolt_period;
	bolt_duration = __that.bolt_duration;
	wind_velocity = __that.wind_velocity;
	wind_direction = __that.wind_direction;
	
	ambient = __that.ambient;
	
	hemi_color = __that.hemi_color;
	
	sun_color = __that.sun_color;
	
	sun_dir = __that.sun_dir;
	
	lens_flare_id = __that.lens_flare_id;
	tb_id = __that.tb_id;
	env_ambient = __that.env_ambient;
	return *this;
}
CEnvDescriptor::CEnvDescriptor(void)
{
	//Log("CEnvDescriptor::CEnvDescriptor");
	exec_time			= 0.0f;
	exec_time_loaded	= 0.0f;
	
	clouds_color.set	(1,1,1,1);
	sky_color.set		(1,1,1);
	sky_rotation		= 0.0f;

	far_plane			= 400.0f;;

	fog_color.set		(1,1,1);
	fog_density			= 0.0f;
	fog_distance		= 400.0f;

	rain_density		= 0.0f;
	//----------------
	//params_ex = xr_new<SParamsEx>();
	params_ex = (SEnvParamsEx*)Memory.mem_alloc(sizeof(SEnvParamsEx));
	params_ex->rain_color.set		(0,0,0);
	params_ex->rain_max_drop_angle = 0;
	params_ex->sun_shafts = 0;
	params_ex->sun_shafts_length = 0;
	params_ex->rain_increase_speed = 1.0;
	params_ex->moon_road_intensity = 0.f;
	//---------------
	bolt_period			= 0.0f;
	bolt_duration		= 0.0f;

    wind_velocity		= 0.0f;
    wind_direction		= 0.0f;
    
	ambient.set			(0,0,0);
	hemi_color.set		(1,1,1,1);
	sun_color.set		(1,1,1);
	sun_dir.set			(0,-1,0);

    lens_flare_id		= -1;
	tb_id				= -1;
    
	env_ambient			= 0;
	//return this;
}
CEnvDescriptor::~CEnvDescriptor()
{
	//Log("CEnvDescriptor::~CEnvDescriptor");
	Memory.mem_free(params_ex);
}
typedef char string_path[520];
void CEnvDescriptor::load(LPCSTR exec_tm, LPCSTR S, CEnvironment* parent)
{
	//Msg("CEnvDescriptor::load: exec_tm = %s, S = %s", exec_tm, S);
	int hours, minutes, secons;
	sscanf					(exec_tm,"%d:%d:%d",&hours, &minutes, &secons);
	//R_ASSERT3				((tm.x>=0)&&(tm.x<24)&&(tm.y>=0)&&(tm.y<60)&&(tm.z>=0)&&(tm.z<60),"Incorrect weather time",S);
	exec_time				= hours*3600.f+minutes*60.f+secons;
	exec_time_loaded		= exec_time;
	string_path				st,st_env;
	strcpy_s(st, pSettings->r_string(S,"sky_texture"));

	strconcat				(sizeof(st_env),st_env,st,"#small");
	sky_texture_name		= st;
	sky_texture_env_name	= st_env;
	clouds_texture_name		= pSettings->r_string	(S,"clouds_texture");
	LPCSTR	cldclr			= pSettings->r_string	(S,"clouds_color");
	float	multiplier		= 0, save=0;
	sscanf					(cldclr,"%f,%f,%f,%f,%f",&clouds_color.x,&clouds_color.y,&clouds_color.z,&clouds_color.w,&multiplier);
	save=clouds_color.w;	
	clouds_color.mul		(.5f*multiplier);		
	clouds_color.w	= save; 
	sky_color				= pSettings->r_fvector3	(S,"sky_color");		
	sky_color.mul(.5f);
	if (pSettings->line_exist(S,"sky_rotation"))	sky_rotation	= DEG2RAD(pSettings->r_float(S,"sky_rotation"));
	else											sky_rotation	= 0;
	far_plane				= pSettings->r_float	(S,"far_plane");
	fog_color				= pSettings->r_fvector3	(S,"fog_color");
	fog_density				= pSettings->r_float	(S,"fog_density");
	fog_distance			= pSettings->r_float	(S,"fog_distance");
	rain_density			= pSettings->r_float	(S,"rain_density");
	clamp(rain_density,0.f,1.f);
	//----------------
	//rain_color				= pSettings->r_fvector3	(S,"rain_color");            
	params_ex->rain_color = pSettings->r_fvector3	(S,"rain_color");
	if (pSettings->line_exist(S,"rain_max_drop_angle"))	params_ex->rain_max_drop_angle = DEG2RAD(pSettings->r_float(S,"rain_max_drop_angle"));
	else												params_ex->rain_max_drop_angle	= DEG2RAD(10);
	if (pSettings->line_exist(S,"sun_shafts"))			params_ex->sun_shafts = pSettings->r_float(S,"sun_shafts");
	else												params_ex->sun_shafts = 0.0;
	if (pSettings->line_exist(S,"sun_shafts_length"))	params_ex->sun_shafts_length = pSettings->r_float(S,"sun_shafts_length");
	else												params_ex->sun_shafts_length	= 0.0;
	if (pSettings->line_exist(S,"rain_increase_speed"))	params_ex->rain_increase_speed = pSettings->r_float(S,"rain_increase_speed");
	else												params_ex->rain_increase_speed	= 1.0;
	if (pSettings->line_exist(S,"moon_road_intensity"))	params_ex->moon_road_intensity = pSettings->r_float(S,"moon_road_intensity");
	else												params_ex->moon_road_intensity	= 0.0;
	//---------------
	wind_velocity			= pSettings->r_float	(S,"wind_velocity");
	wind_direction			= DEG2RAD(pSettings->r_float(S,"wind_direction"));
	ambient					= pSettings->r_fvector3	(S,"ambient");
	hemi_color				= pSettings->r_fvector4	(S,"hemi_color");
	sun_color				= pSettings->r_fvector3	(S,"sun_color");
	_vector2<float> sun_dir_ = pSettings->r_fvector2	(S,"sun_dir");	
	sun_dir.setHP	(DEG2RAD(sun_dir_.y), DEG2RAD(sun_dir_.x));
	//VERIFY2					(sun_dir.y<0,"Invalid sun direction settings while loading");

	lens_flare_id			= parent->eff_LensFlare->AppendDef(pSettings,pSettings->r_string(S,"flares"));
	tb_id					= parent->eff_Thunderbolt->AppendDef(pSettings,pSettings->r_string(S,"thunderbolt"));
	bolt_period				= (tb_id>=0)?pSettings->r_float	(S,"bolt_period"):0.f;
	bolt_duration			= (tb_id>=0)?pSettings->r_float	(S,"bolt_duration"):0.f;
	env_ambient = 0;
	if (pSettings->line_exist(S,"env_ambient"))
	{
		env_ambient = parent->AppendEnvAmb(pSettings->r_string(S,"env_ambient"));
	}

	CHECK_VECTOR					(clouds_color);
	CHECK_VECTOR					(sky_color	);
	CHECK_VECTOR					(fog_color	);
	CHECK_VECTOR					(params_ex->rain_color	);
	CHECK_VECTOR					(ambient	);
	CHECK_VECTOR					(hemi_color	);
	CHECK_VECTOR					(sun_color	);

	on_device_create();
}
void CEnvDescriptorMixer::lerp(CEnvironment* , CEnvDescriptor& A, CEnvDescriptor& B, float f, CEnvModifier& M, float m_power)
{
	//Log("CEnvDescriptorMixer::lerp");
	float	_power			=	1.f/(m_power+1);	// the environment itself
	float	fi				=	1-f;

	sky_r_textures.clear();
	sky_r_textures.push_back(make_pair(0,A.sky_texture));
	sky_r_textures.push_back(make_pair(1,B.sky_texture));

	sky_r_textures_env.clear();
	sky_r_textures_env.push_back(make_pair(0,A.sky_texture_env));
	sky_r_textures_env.push_back(make_pair(1,B.sky_texture_env));

	clouds_r_textures.clear();
	clouds_r_textures.push_back	(make_pair(0,A.clouds_texture));
	clouds_r_textures.push_back	(make_pair(1,B.clouds_texture));

	weight					=	f;

	clouds_color.lerp(A.clouds_color,B.clouds_color,f);
	sky_rotation			=	(fi*A.sky_rotation + f*B.sky_rotation);
	far_plane				=	(fi*A.far_plane + f*B.far_plane + M.far_plane)*psVisDistance*_power;
	fog_color.lerp(A.fog_color,B.fog_color,f).add(M.fog_color).mul(_power);
	fog_density				=	(fi*A.fog_density + f*B.fog_density + M.fog_density)*_power;
	fog_distance			=	(fi*A.fog_distance + f*B.fog_distance);
	fog_near				=	(1.0f - fog_density) *0.85f * fog_distance;
	fog_far					=	0.99f * fog_distance;
	rain_density			=	fi*A.rain_density + f*B.rain_density;
	//------------------------------
	params_ex->rain_color.lerp(A.params_ex->rain_color,B.params_ex->rain_color,f);
	params_ex->rain_max_drop_angle = fi*A.params_ex->rain_max_drop_angle + f*B.params_ex->rain_max_drop_angle;
	params_ex->sun_shafts = fi*A.params_ex->sun_shafts + f*B.params_ex->sun_shafts;
	params_ex->sun_shafts_length = fi*A.params_ex->sun_shafts_length + f*B.params_ex->sun_shafts_length;
	params_ex->rain_increase_speed = fi*A.params_ex->rain_increase_speed + f*B.params_ex->rain_increase_speed;
	params_ex->moon_road_intensity = fi*A.params_ex->moon_road_intensity + f*B.params_ex->moon_road_intensity;
	//------------------------------
	bolt_period				=	fi*A.bolt_period + f*B.bolt_period;
	bolt_duration			=	fi*A.bolt_duration + f*B.bolt_duration;
	// wind
	wind_velocity			=	fi*A.wind_velocity + f*B.wind_velocity;
	wind_direction			=	fi*A.wind_direction + f*B.wind_direction;

	// colors
	sky_color.lerp			(A.sky_color,B.sky_color,f).add(M.sky_color).mul(_power);
	ambient.lerp			(A.ambient,B.ambient,f).add(M.ambient).mul(_power);
	hemi_color.lerp			(A.hemi_color,B.hemi_color,f);
	hemi_color.x			+= M.hemi_color.x;
	hemi_color.y			+= M.hemi_color.y; 
	hemi_color.z			+= M.hemi_color.z;
	hemi_color.x			*= _power;
	hemi_color.y			*= _power;
	hemi_color.z			*= _power;
	sun_color.lerp			(A.sun_color,B.sun_color,f);
	sun_dir.lerp			(A.sun_dir,B.sun_dir,f).normalize();
	//VERIFY2					(sun_dir.y<0,"Invalid sun direction settings while lerp");
	//Log("done");

}

void CEnvironment::ReselectEnvs(void)
{
	SelectEnvs(CurrentWeather, Current[0], Current[1], fGameTime);
}
void CEnvironment::load()
{
	/*tonemap					= Device.Resources->_CreateTexture("$user$tonemap");	//. hack
	if (!eff_Rain)    		eff_Rain 		= xr_new<CEffect_Rain>();
	if (!eff_LensFlare)		eff_LensFlare 	= xr_new<CLensFlare>();
	if (!eff_Thunderbolt)	eff_Thunderbolt	= xr_new<CEffect_Thunderbolt>();
	// load weathers
	if (WeatherCycles.empty()){
		LPCSTR first_weather=0;
		int weather_count	= pSettings->line_count("weathers");
		for (int w_idx=0; w_idx<weather_count; w_idx++){
			LPCSTR weather, sect_w;
			if (pSettings->r_line("weathers",w_idx,&weather,&sect_w)){
				if (0==first_weather) first_weather=weather;
				int env_count	= pSettings->line_count(sect_w);
				LPCSTR exec_tm, sect_e;
				for (int env_idx=0; env_idx<env_count; env_idx++){
					if (pSettings->r_line(sect_w,env_idx,&exec_tm,&sect_e)){
						CEnvDescriptor*		D=xr_new<CEnvDescriptor>();
						D->load				(exec_tm,sect_e,this);
						WeatherCycles[weather].push_back	(D);
					}
				}
			}
		}
		// sorting weather envs
		EnvsMapIt _I=WeatherCycles.begin();
		EnvsMapIt _E=WeatherCycles.end();
		for (; _I!=_E; _I++){
			//R_ASSERT3	(_I->second.size()>1,"Environment in weather must >=2",*_I->first);
			std::sort(_I->second.begin(),_I->second.end(),sort_env_etl_pred);
		}
		//R_ASSERT2	(!WeatherCycles.empty(),"Empty weathers.");
		SetWeather	(first_weather);
	}
	// load weather effects
	if (WeatherFXs.empty()){
		int line_count	= pSettings->line_count("weather_effects");
		for (int w_idx=0; w_idx<line_count; w_idx++){
			LPCSTR weather, sect_w;
			if (pSettings->r_line("weather_effects",w_idx,&weather,&sect_w)){
				EnvVec& env		= WeatherFXs[weather];
				env.push_back	(xr_new<CEnvDescriptor>()); env.back()->exec_time_loaded = 0;
				env.push_back	(xr_new<CEnvDescriptor>()); env.back()->exec_time_loaded = 0;
				int env_count	= pSettings->line_count(sect_w);
				LPCSTR exec_tm, sect_e;
				for (int env_idx=0; env_idx<env_count; env_idx++){
					if (pSettings->r_line(sect_w,env_idx,&exec_tm,&sect_e)){
						CEnvDescriptor*	D=xr_new<CEnvDescriptor>();
						D->load			(exec_tm,sect_e,this);
						env.push_back	(D);

					}
				}
				env.push_back	(xr_new<CEnvDescriptor>()); env.back()->exec_time_loaded = DAY_LENGTH;
			}
		}
		// sorting weather envs
		EnvsMapIt _I=WeatherFXs.begin();
		EnvsMapIt _E=WeatherFXs.end();
		for (; _I!=_E; _I++){
			//R_ASSERT3	(_I->second.size()>1,"Environment in weather must >=2",*_I->first);
			std::sort(_I->second.begin(),_I->second.end(),sort_env_etl_pred);
		}
	}*/
	
}

void CEnvironment::Invalidate(void)
{
//	Log("CEnvironment::Invalidate");
	bWFX					= false;
	Current[0]				= 0;
	Current[1]				= 0;
	if (eff_LensFlare)		eff_LensFlare->Invalidate();
}

/*
void CEnvironment::SetWeather(shared_str name, bool forced)
{
	Msg("CEnvironment::SetWeather: %s", name.c_str());
//.	static BOOL bAlready = FALSE;
//.	if(bAlready)	return;
	if (name.size())	{
//.		bAlready = TRUE;
        EnvsMapIt it		= WeatherCycles.find(name);
		if (it == WeatherCycles.end())
		{
			Msg("! Invalid weather name: %s", name.c_str());
			return;
		}
        //R_ASSERT3			(it!=WeatherCycles.end(),"Invalid weather name.",*name);
		CurrentCycleName	= it->first;
		if (forced)			{Invalidate();			}
		if (!bWFX){
			CurrentWeather		= &it->second;
			CurrentWeatherName	= it->first;
		}
		if (forced)			{SelectEnvs(fGameTime);	}
    }
	else
	{
		//FATAL				("! Empty weather name");
    }
}

*/