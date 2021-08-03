#pragma once

#include "DLL_Pure.h"
#include "InputReceiver.h"
#include "pureRender.h"
#include "pureFrame.h"
#include "event.h"
#include "sound.h"
#include "object_list.h"
#include "feel.h"
#include "object_space.h"

class CCameraManager;
class CInifile;
class CObject;
class ISpatial;
struct CCustomHUD;

struct CCustomHUDVtbl
{
	char dummy[0xC];
	void (__thiscall *Render_First)(CCustomHUD *_this);
	void (__thiscall *Render_Last)(CCustomHUD *_this);
};

struct CCustomHUD // actually this class inherited from DLL_Pure and IEventReceiver, but i need table of virtual functions
{
	CCustomHUDVtbl *vfptr;

	void Render_First()
	{
		return vfptr->Render_First(this);
	};
	void Render_Last()
	{
		return vfptr->Render_Last(this);
	};
};

class IGame_Level : public DLL_Pure, public IInputReceiver, public pureRender, public pureFrame, public IEventReceiver
{
public:
  CObject *pCurrentEntity;
  CObject *pCurrentViewEntity;
  xr_vector<ref_sound> Sounds_Random;
  unsigned int Sounds_Random_dwNextTime;
  int Sounds_Random_Enabled;
  CCameraManager *m_pCameras;
  xr_vector<ISpatial *,xalloc<ISpatial *> > snd_ER;
  CObjectList Objects;
  CObjectSpace ObjectSpace;
  int bReady;
  CInifile *pLevel;
  CCustomHUD *pHUD;
	struct	_esound_delegate	{
		Feel::Sound*			dest	;
		resptr_core<ref_sound_data,resptr_base<ref_sound_data> >		source	;
		float					power	;
	};
  xr_vector<IGame_Level::_esound_delegate> snd_Events;
public:
	CObject* CurrentViewEntity(void) const
	{
		return pCurrentViewEntity;
	}
	CObject* CurrentEntity(void) const
	{
		return pCurrentEntity;
	}
};

enum fsL_Chunks			{
	fsL_HEADER			=1,		//*
	fsL_SHADERS			=2,		//*
	fsL_VISUALS			=3,		//*
	fsL_PORTALS			=4,		//*		- Portal polygons
	fsL_LIGHT_DYNAMIC	=6,		//*
	fsL_GLOWS			=7,		//*		- All glows inside level
	fsL_SECTORS			=8,		//*		- All sectors on level
	fsL_VB				=9,		//*		- Static geometry
	fsL_IB				=10,	//*
	fsL_SWIS			=11,	//*		- collapse info, usually for trees
    fsL_forcedword		= 0xFFFFFFFF
};
enum fsSLS_Chunks		{
	fsSLS_Description	= 1,	// Name of level
	fsSLS_ServerState,
	fsSLS_forcedword = u32(-1)
};

extern __declspec(dllimport) IGame_Level* g_pGameLevel;
