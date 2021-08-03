#pragma once
/*
#include "vfw.h"
#include "mmsystem.h"
*/
#include "base_types.h"

struct CAviPlayerCustom
{
/*  CAviPlayerCustom *alpha;
  AVIINDEXENTRY *m_pMovieIndex;
  char *m_pMovieData;
  HIC__ *m_aviIC;
  char *m_pDecompressedBuf;
  tagBITMAPINFOHEADER m_biOutFormat;
  tagBITMAPINFOHEADER m_biInFormat;
  float m_fRate;
  float m_fCurrentRate;
  unsigned int m_dwFrameTotal;
  unsigned int m_dwFrameCurrent;
  unsigned int m_dwFirstFrameOffset;
  unsigned int m_dwWidth;
  unsigned int m_dwHeight;*/
};

class CTheoraStream;

class CTheoraSurface
{
	CTheoraStream*		m_rgb;
	CTheoraStream*		m_alpha;

	u32					tm_start;
	u32					tm_play;
	u32					tm_total;
	BOOL				ready;
	BOOL				bShaderYUV2RGB;
public:
	BOOL				playing;
	BOOL				looped;
};