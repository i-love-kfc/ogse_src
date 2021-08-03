#pragma once
#include "base_types.h"
/*
typedef void	( * LogCallback)	(LPCSTR string);

class xrCore 
{
public:
	string64	ApplicationName;
	string_path	ApplicationPath;
	string_path	WorkingPath;
	string64	UserName;
	string64	CompName;
	string512	Params;

public:
	void		_initialize	(LPCSTR ApplicationName, LogCallback cb=0, BOOL init_fs=TRUE, LPCSTR fs_fname=0);
	void		_destroy	();
};
extern __declspec(dllimport) xrCore	Core;

class CLocatorAPI  
{
//	friend class FS_Path;
public:
	struct	file
	{
		LPCSTR					name;			// low-case name
		u32						vfs;			// 0xffffffff - standart file
		u32						crc;			// contents CRC
		u32						ptr;			// pointer inside vfs
		u32						size_real;		// 
		u32						size_compressed;// if (size_real==size_compressed) - uncompressed
        u32						modif;			// for editor
	};
/*private:
	struct	file_pred: public 	std::binary_function<file&, file&, bool> 
	{	
		IC bool operator()	(const file& x, const file& y) const
		{	return xr_strcmp(x.name,y.name)<0;	}
	};
	struct	archive
	{
		shared_str				path;
		void					*hSrcFile, *hSrcMap;
		u32						size;
	};
	DEFINE_MAP_PRED				(LPCSTR,FS_Path*,PathMap,PathPairIt,pred_str);
	PathMap						pathes;

	DEFINE_SET_PRED				(file,files_set,files_it,file_pred);
    DEFINE_VECTOR				(archive,archives_vec,archives_it);

	DEFINE_VECTOR				(_finddata_t,FFVec,FFIt);
	FFVec						rec_files;

    int							m_iLockRescan	; 

	files_set					files			;
    archives_vec				archives		;
	BOOL						bNoRecurse		;

	xrCriticalSection			m_auth_lock		;
	u64							m_auth_code		;
public:
	enum{
		flNeedRescan			= (1<<0),
		flBuildCopy				= (1<<1),
		flReady					= (1<<2),
		flEBuildCopy			= (1<<3),
		flEventNotificator      = (1<<4),
		flTargetFolderOnly		= (1<<5),
		flCacheFiles			= (1<<6),
		flScanAppRoot			= (1<<7),
		flNeedCheck				= (1<<8),
		flDumpFileActivity		= (1<<9),
	};    
	Flags32						m_Flags			;
	u32							dwAllocGranularity;
	u32							dwOpenCounter;*//*
public:
	LPCSTR						update_path		(string_path& dest, LPCSTR initial, LPCSTR src);
	IWriter*					w_open			(LPCSTR initial, LPCSTR N);
	void						w_close			(IWriter* &S);

	CStreamReader*				rs_open			(LPCSTR initial, LPCSTR N);
	IReader*					r_open			(LPCSTR N);
	void						r_close			(IReader* &S);
	void						r_close			(CStreamReader* &fs);

	const file*					exist			(LPCSTR path, LPCSTR name);
};

extern __declspec(dllimport)	CLocatorAPI*					xr_FS;
#define FS (*xr_FS)*/