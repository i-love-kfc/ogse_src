#pragma once
#include "IniFile.h"

class __declspec(dllimport) CEffect_Thunderbolt
{
public:                     
	CEffect_Thunderbolt	(); 
	~CEffect_Thunderbolt();

	void OnFrame(int id, float period, float duration);
	void Render();

	int AppendDef(CInifile* pIni, const char* sect);
};
