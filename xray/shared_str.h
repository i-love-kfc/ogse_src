#pragma once
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#include "windows.h"
#include "base_types.h"
#include "CriticalSection.h"

struct __declspec(dllimport) str_value
{
	DWORD dwReference;
	DWORD dwLength;
	DWORD dwCRC;
	char value[];
	str_value & operator=(struct str_value const &);
};

struct __declspec(dllimport) str_value_cmp;
/*{ // less
	IC bool		operator ()	(const str_value* A, const str_value* B) const	{ return A->dwCRC<B->dwCRC;	};
};*/

class __declspec(dllimport) str_container
{
private:
public:
	str_value * dock(char const *);
	void dump();
};

extern __declspec(dllimport) str_container * g_pStringContainer;

/*
class shared_str
{
public:
	str_value *p_;
	shared_str() : p_(0) {}
	void set(const char* str)
	{
		str_value * v10 = g_pStringContainer->dock(str);
		if ( v10 )
			++*(DWORD *)v10;
		if ( p_ )
		{
			--p_->dwReference;
			if ( !p_->dwReference )
				p_ = 0;
		}
		p_ = v10;
	}

	shared_str(const char* s) {p_ = 0; set(s);}
	~shared_str() {--p_->dwReference;}
	const char * operator*() const
	{
		str_value *v1 = this->p_;
		return v1 ? (const char *)&v1[1] : 0;
	}

	const char* p_str()
	{
		const static char* stub = "";

		if (p_->dwLength > 0)
		{
			return (const char *)&p_[1];
		}
		else
		{
			return stub;
		}
	}
};

*/


typedef const char*		str_c;



//////////////////////////////////////////////////////////////////////////
class					shared_str
{
private:
	str_value*			p_;
protected:
	// ref-counting
	void				_dec		()								{	if (0==p_) return;	p_->dwReference--; 	if (0==p_->dwReference)	p_=0;						}
public:
	void				_set		(str_c rhs) 					
	{	
		str_value* v = g_pStringContainer->dock(rhs); 
		if (0!=v) 
			v->dwReference++; 
		_dec(); p_ = v;	
	}
	void				_set		(shared_str const &rhs)			{	str_value* v = rhs.p_; if (0!=v) v->dwReference++; _dec(); p_ = v;							}
	const str_value*	_get		()	const						{	return p_;																					}
public:
	// construction
						shared_str	()								{	p_ = 0;											}
						shared_str	(str_c rhs) 					{	p_ = 0;	_set(rhs);								}
						shared_str	(shared_str const &rhs)			{	p_ = 0;	_set(rhs);								}
						~shared_str	()								{	_dec();											}

	// assignment & accessors
	shared_str&			operator=	(str_c rhs)						{	_set(rhs);	return (shared_str&)*this;			}
	shared_str&			operator=	(shared_str const &rhs)			{	_set(rhs);	return (shared_str&)*this;			}
	str_c				operator*	() const						{	return p_?p_->value:0;							}
	bool				operator!	() const						{	return p_ == 0;									}
	char				operator[]	(size_t id)						{	return p_->value[id];							}
	str_c				c_str		() const						{	return p_?p_->value:0;							}

	// misc func
	u32					size		()						const	{	if (0==p_) return 0; else return p_->dwLength;	}
	void				swap		(shared_str & rhs)				{	str_value* tmp = p_; p_ = rhs.p_; rhs.p_ = tmp;	}
	bool				equal		(const shared_str & rhs) const	{	return (p_ == rhs.p_);							}
    /*shared_str& __cdecl	sprintf		(const char* format, ...)		{
		string4096 	buf;
		va_list		p;
		va_start	(p,format);
		int vs_sz	= _vsnprintf(buf,sizeof(buf)-1,format,p); buf[sizeof(buf)-1]=0;
		va_end		(p);
		if (vs_sz)	_set(buf);	
		return 		(shared_str&)*this;
	}*/
};

// res_ptr == res_ptr
// res_ptr != res_ptr
// const res_ptr == ptr
// const res_ptr != ptr
// ptr == const res_ptr
// ptr != const res_ptr
// res_ptr < res_ptr
// res_ptr > res_ptr
inline bool operator	==	(shared_str const & a, shared_str const & b)		{ return a._get() == b._get();					}
inline bool operator	!=	(shared_str const & a, shared_str const & b)		{ return a._get() != b._get();					}
inline bool operator	<	(shared_str const & a, shared_str const & b)		{ return a._get() <  b._get();					}
inline bool operator	>	(shared_str const & a, shared_str const & b)		{ return a._get() >  b._get();					}

// externally visible standart functionality
inline void swap			(shared_str & lhs, shared_str & rhs)				{ lhs.swap(rhs);		}
//inline u32	xr_strlen		(shared_str & a)									{ return a.size();		}
//inline int	xr_strcmp		(const shared_str & a, const char* b)				{ return xr_strcmp(*a,b);	}
//inline int	xr_strcmp		(const char* a, const shared_str & b)				{ return xr_strcmp(a,*b);	}
/*inline int	xr_strcmp		(const shared_str & a, const shared_str & b)		{ 
	if (a.equal(b))		return 0;
	else				return xr_strcmp(*a,*b);
}
*/
//inline void	xr_strlwr		(xr_string& src)									{ for(xr_string::iterator it=src.begin(); it!=src.end(); it++) *it=xr_string::value_type(tolower(*it));}
//inline void	xr_strlwr		(shared_str& src)									{ if (*src){LPSTR lp=xr_strdup(*src); xr_strlwr(lp); src=lp; xr_free(lp);} }

//#pragma pack(pop)
