#pragma once

#include "base_types.h"

struct ref_sound_data;

struct ref_sound_dataVtbl
{
  void *(__thiscall *__vecDelDtor)(ref_sound_data *_this, unsigned int);
};

struct ref_sound_data : public xr_resource
{
  ref_sound_dataVtbl *vfptr;
  /*shared_str nm;
  CSound_source *handle;
  CSound_emitter *feedback;
  esound_type s_type;
  int g_type;
  CObject *g_object;
  resptr_core<CSound_UserData,resptr_base<CSound_UserData> > g_userdata;
  */
};

struct ref_sound
{
  resptr_core<ref_sound_data,resptr_base<ref_sound_data> > _p;
};