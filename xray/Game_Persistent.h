#pragma once
#include "Environment.h"

struct IMainMenu;
struct IMainMenuVtbl
{
  void *(__thiscall *__vecDelDtor)(IMainMenu *_this, unsigned int);
  void (__thiscall *Activate)(IMainMenu *_this, bool);
  bool (__thiscall *IsActive)(IMainMenu *_this);
  void (__thiscall *DestroyInternal)(IMainMenu *_this, bool);
};
struct IMainMenu
{
  IMainMenuVtbl *vfptr;

	bool IsActive()
	{
		return vfptr->IsActive(this);
	}
};

extern IC bool IsMainMenuActive();

class IGame_Persistent;
class IRender_Visual;
struct IGame_PersistentVtbl
{
	char dummy [0x14];
	bool (__thiscall *OnRenderPPUI_query)(IGame_Persistent *_this);
	void (__thiscall *OnRenderPPUI_main)(IGame_Persistent *_this);
	void (__thiscall *OnRenderPPUI_PP)(IGame_Persistent *_this);
	void (__thiscall *OnGameStart)(IGame_Persistent *_this);
	void (__thiscall *OnGameEnd)(IGame_Persistent *_this);
	void (__thiscall *UpdateGameType)(IGame_Persistent *_this);
	void (__thiscall *RegisterModel)(IGame_Persistent *_this, IRender_Visual* V);
	float (__thiscall *MtlTransparent)(IGame_Persistent *_this, unsigned int);
	u32 dummy2;
	void (__thiscall *LoadTitle)(IGame_Persistent *_this, const char *str);
};
class IGame_Persistent
{
	IGame_PersistentVtbl *vfptr; 
	char dummy[0x464];
public:
	IMainMenu *m_pMainMenu;
//	CEnvironment *pEnvironment;
	CEnvironment& Environment(void) ;
	float MtlTransparent(unsigned int mtl_idx)
	{
		return vfptr->MtlTransparent(this, mtl_idx);
	}
	bool OnRenderPPUI_query(void)
	{
		return vfptr->OnRenderPPUI_query(this);
	}
	void OnRenderPPUI_main(void)
	{
		return vfptr->OnRenderPPUI_main(this);
	}
	void OnRenderPPUI_PP(void)
	{
		return vfptr->OnRenderPPUI_PP(this);
	}
	void LoadTitle(const char *str)
	{
		return vfptr->LoadTitle(this, str);
	}
};
extern __declspec(dllimport) IGame_Persistent * g_pGamePersistent;

class CApplication
{
public:
	struct					sLevelInfo
	{
		char*				folder;
		char*				name;
	};
	char dummy[0x134];
	xr_vector<sLevelInfo>	Levels;
	u32						Level_Current;
public:
	void					LoadBegin			();
	void					LoadEnd				();
};
extern __declspec(dllimport) CApplication *pApp;
extern __declspec(dllimport) bool g_dedicated_server;