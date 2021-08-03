#pragma once
#include "xrCommon.h"
/*
class /*__declspec(dllimport)*//* CInifile {
	//void Load(class IReader *,char const *);
public:
	CInifile(char const *,int,int,int);
	CInifile(CInifile const &);
	virtual ~CInifile(void);
	//CInifile(class IReader *,char const *);
	struct Item {
		Item::~Item(void);
		Item::Item(struct Item const &);
		Item::Item(void);
		Item & operator=(Item const &);
	};
	struct Sect {
		Sect::~Sect(void);
		Sect::Sect(Sect const &);
		Sect::Sect(void);
		int line_exist(char const *,char const * *);
	};
	bool save_as(char const *);
	char const * fname(void);
	char const * r_string(char const *,char const *) ;
	char const * r_string(shared_str const &,char const *) ;
	CInifile & operator=(CInifile const &)  ;
	shared_str r_string_wb(char const *,char const *) ;
	shared_str r_string_wb(shared_str const &,char const *) ;
	float r_float(char const *,char const *) ;
	float r_float(shared_str const &,char const *) ;
	int line_exist(char const *,char const *) ;
	int line_exist(shared_str const &, shared_str const &) ;
	int r_bool(char const *,char const *) ;
	int r_bool(shared_str const &,char const *) ;
	int r_line(char const *,int,char const * *,char const * *) ;
	int r_line(shared_str const &,int,char const * *,char const * *) ;
	int r_s32(char const *,char const *) ;
	int r_s32(shared_str const &,char const *) ;
	int r_token(char const *,char const *,struct xr_token const *) ;
	int section_exist(char const *) ;
	int section_exist(shared_str const &) ;
	short r_s16(char const *,char const *) ;
	short r_s16(shared_str const &,char const *) ;
	signed char r_s8(char const *,char const *) ;
	signed char r_s8(shared_str const &,char const *) ;
	static class CInifile * Create(char const *,int);
	static int IsBOOL(char const *);
	static void Destroy(CInifile *);
	//struct _color<float> r_fcolor(char const *,char const *);
	//struct _color<float> r_fcolor(class shared_str const &,char const *)
	_vector2<float> r_fvector2(char const *,char const *) ;
	_vector2<float> r_fvector2(class shared_str const &,char const *) ;
	//struct _vector2<int> r_ivector2(char const *,char const *);
	//struct _vector2<int> r_ivector2(class shared_str const &,char const *);
	_vector3<float> r_fvector3(char const *,char const *) ;
	//struct _vector3<float> r_fvector3(class shared_str const &,char const *);
	//struct _vector3<int> r_ivector3(char const *,char const *);
	//struct _vector3<int> r_ivector3(class shared_str const &,char const *);
	_vector4<float> r_fvector4(char const *,char const *) ;
	_vector4<float> r_fvector4(class shared_str const &,char const *) ;
	//struct _vector4<int> r_ivector4(char const *,char const *);
	//struct _vector4<int> r_ivector4(class shared_str const &,char const *);
	Sect & r_section(char const *) ;
	Sect & r_section(shared_str const &) ;
	unsigned __int64 r_clsid(char const *,char const *) ;
	unsigned __int64 r_clsid(shared_str const &,char const *) ;
	unsigned char r_u8(char const *,char const *) ;
	unsigned char r_u8(shared_str const &,char const *) ;
	unsigned int line_count(char const *) ;
	unsigned int line_count(shared_str const &) ;
	unsigned int r_color(char const *,char const *) ;
	unsigned int r_color(shared_str const &,char const *) ;
	unsigned int r_u32(char const *,char const *) ;
	unsigned int r_u32(shared_str const &,char const *) ;
	unsigned short r_u16(char const *,char const *) ;
	unsigned short r_u16(shared_str const &,char const *) ;
	void remove_line(char const *,char const *);
	void w_bool(char const *,char const *,int,char const *);
	void w_color(char const *,char const *,unsigned int,char const *);
	//void w_fcolor(char const *,char const *,struct _color<float> const &,char const *);
	void w_float(char const *,char const *,float,char const *);
	//void w_fvector2(char const *,char const *,struct _vector2<float> const &,char const *);
	//void w_fvector3(char const *,char const *,struct _vector3<float> const &,char const *);
	//void w_fvector4(char const *,char const *,struct _vector4<float> const &,char const *);
	//void w_ivector2(char const *,char const *,struct _vector2<int> const &,char const *);
	//void w_ivector3(char const *,char const *,struct _vector3<int> const &,char const *);
	//void w_ivector4(char const *,char const *,struct _vector4<int> const &,char const *);
	void w_s16(char const *,char const *,short,char const *);
	void w_s32(char const *,char const *,int,char const *);
	void w_s8(char const *,char const *,signed char,char const *);
	void w_string(char const *,char const *,char const *,char const *);
	void w_u16(char const *,char const *,unsigned short,char const *);
	void w_u32(char const *,char const *,unsigned int,char const *);
	void w_u8(char const *,char const *,unsigned char,char const *);
	//xr_vector<struct Sect *,class xalloc<struct Sect *> > & sections(void);
};

extern __declspec(dllimport) CInifile * pSettings;

extern __declspec(dllimport) int		    	_GetItemCount			( LPCSTR , char separator=',');
extern __declspec(dllimport) LPSTR	    	_GetItem				( LPCSTR, int, LPSTR, char separator=',', LPCSTR ="", bool trim=true );
extern __declspec(dllimport) LPSTR	    	_GetItems				( LPCSTR, int, int, LPSTR, char separator=',');
*/