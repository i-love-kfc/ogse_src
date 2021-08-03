#include "CameraManager.h"
#include "base_types.h"
#include "Game_Persistent.h"
#include "game_level.h"
#include "car.h"

#define _USE_MATH_DEFINES // for C++
#include <math.h>

void extract_euler_xzy(const Fmatrix &m, float &ax, float &az, float &ay);
const _vector3<float> pos = _vector3<float>().set(0,0,0);
const _vector3<float> vertical = _vector3<float>().set(0,1,0);
const _vector3<float> hor = _vector3<float>().set(1,0,0);
void CCameraManager::Update3(const CCameraBase* C)
{
	//return;
	//
	//Log("CCameraManager::Update3");
	//Msg("clmp_Yaw = %d, pitch = %f, yaw = %f, roll = %f", (int)C->bClampYaw, C->pitch, C->yaw, C->roll);
	CCameraBase* C1 = const_cast<CCameraBase*>(C);
	//_vector3<float> r1, v1, a;
	//r1.crossproduct(C1->vDirection, vertical).normalize(); // нормаль к направлению в горизонтальной плоскости
	//v1.crossproduct(r1, C1->vDirection); // правильная вертикаль камеры
	//a.crossproduct(v1, C1->vNormal); // вектор, соосный направлению с длиной, равной синусу угла доворота
	//float aaa = a.dotproduct(C1->vDirection);
	//float angle = asin(acos(aaa));
	//C1->roll = 0.5;
	//Msg("roll = %f", 180*angle/3.141);
	//const _vector3<float> &d =  C->vDirection;
	//const _vector3<float> &n =  C->vNormal;
	//Msg("dir: %5.3f, %5.3f, %5.3f", d.x, d.y, d.z);
	//Msg("nrm: %5.3f, %5.3f, %5.3f", n.x, n.y, n.z);
	//_vector3<float> r;
	//r.crossproduct(n, d);
	//float da = asin(r.y);
	//Msg("angle: %5.3f", da);
	//C1->roll -= da;

	//---------------------------------
	const Fmatrix& obj_XFORM = *(const Fmatrix*)((char*)C->parent + 80);
	float rx,ry,rz;
	extract_euler_xzy(obj_XFORM, rx, rz, ry);
	//Msg("rx = %5.3f, rz = %5.3f, ry = %5.3f", rx, rz, ry);
	//Msg("pos: %5.1f, %5.1f, %5.1f", obj_XFORM.c.x, obj_XFORM.c.y, obj_XFORM.c.z);
	C1->roll = rz;
	return;
/*	Update(
		//C->vPosition,
		pos,
		//C->vDirection,
		hor,
		//C->vNormal,
		vertical,
		C->f_fov*0.6f,
		C->f_aspect,
		g_pGamePersistent->Environment().CurrentEnv.far_plane,
		C->m_Flags);*/
}


void extract_euler_xzy(const Fmatrix &m, float &ax, float &az, float &ay)
{
	if (m.i.y < 1)
	{
		if (m.i.y > -1)
		{
			az = asin(-m.i.y);
			ax = atan2(m.k.y, m.j.y);
			ay = atan2(m.i.z, m.i.x);
			return;
		}
		else
		{
			az = M_PI_2;
			ax = -atan2(-m.k.x, m.k.z);
			ay = 0;
			return;
		}
	}
	else
	{
		az = -M_PI_2;
		ax = atan2(-m.k.x, m.k.z);
		ay = 0;
		return;
	}
}


void print_pp_effect(CCameraManager* cm)
{
	Msg("effectors: %d", cm->m_EffectorsPP.size());
	for (size_t i=0; i<cm->m_EffectorsPP.size(); i++)
	{
	}
}

void CCar::cam_Update(float dt, float fov)
{
	char* holder = (char*)this;
	//Msg("custom camera: %f", dt);
	Fvector P, Da;
	Da.set(0,0,0);

	Fmatrix & XFORM = *(Fmatrix*)(holder - 127*4);
	//Msg("1");
	Fvector & m_camera_position = *(Fvector*)(holder + 736);
	//Msg("2");
	//Msg("pos: %5.1f %5.1f %5.1f", m_camera_position.x, m_camera_position.y, m_camera_position.z);
	CCameraBase *active_camera = *(CCameraBase**)(holder + 732);
	//Msg("3");
	Fvector new_base = m_camera_position;
	new_base.y -= 0.6;
	XFORM.transform_tiny(P, new_base /*m_camera_position*/);
	P.y += 0.6;
	//Msg("4");

	if (active_camera->tag == 0)
	{
		//Msg("tag");
		char *owner_actor = *(char**)(holder + 8);
		if (owner_actor)
		{
		*(float*)(owner_actor + 1248) = -active_camera->yaw;
		*(float*)(owner_actor + 1252) = -active_camera->pitch;
		}
		//Msg("yaw = %5.2f, pitch = %5.2f", active_camera->yaw, active_camera->pitch);
	}
	//Msg("5");
	active_camera->f_fov = fov;
	active_camera->vfptr->Update(active_camera, P, Da);
	//Msg("6");
	g_pGameLevel->m_pCameras->Update(active_camera);
	//Msg("exit");
}
