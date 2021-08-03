#pragma once
#include "xr_alloc.h"
#include "xrCommon.h"
#include "shared_str.h"
/*
struct xr_resource
{
  unsigned int dwReference;
};
struct xr_resource_flagged //: public xr_resource
{
	xr_resource baseclass_0;
	unsigned int dwFlags;
};

template<class T>
struct resptr_base
{
	inline resptr_base(T*p) : p_(p) {
		Log("copy_constr");
		++p_->baseclass_4.baseclass_0.baseclass_0.dwReference;
		Msg("copy_constr: %d", p_->baseclass_4.baseclass_0.baseclass_0.dwReference);
	}
	resptr_base() : p_(0) {
		Log1("eqv_oper")
	}
	void operator=(T*);
	T *p_;
	inline ~resptr_base()
	{
		Log("destr");
		if ( p_ )
		{
			//++p_->baseclass_4.baseclass_0.baseclass_0.dwReference;
			//++p_->baseclass_4.baseclass_0.baseclass_0.dwReference;
			if ( p_->baseclass_4.baseclass_0.baseclass_0.dwReference-- == 1 )
			{
				p_->vfptr->__vecDelDtor(p_, 0);
				Memory.mem_free(__RTCastToVoid(p_));
			}
		}
		Msg("destr: %d", p_->baseclass_4.baseclass_0.baseclass_0.dwReference);
	}
};
template<class T1, class T2>
class resptr_core : public T2
{
public:
	//resptr_core(const resptr_core&);
	//void operator=(const resptr_core&);
	~resptr_core();
};
struct xr_resource_named //: xr_resource_flagged
{
	xr_resource_flagged baseclass_0;
	shared_str cName;
};*/

class	/*__declspec(dllimport)*/ 	xr_resource	
{
public:
	enum			{RF_REGISTERED=1<<0 };
public:
	unsigned int				dwReference;
	xr_resource()			: dwReference(0)				{ }
};

class	/*__declspec(dllimport)*/ 	xr_resource_flagged	:	public xr_resource			
{
public:
	enum			{RF_REGISTERED=1<<0 };
public:
	unsigned int				dwFlags;
	xr_resource_flagged()	: dwFlags(0)					{ }
};

class	/*__declspec(dllimport)*/ 	xr_resource_named	:	public xr_resource_flagged	
{
public:
	shared_str		cName;

	const char *	set_name			( const char * name)	
	{
		cName		= name;
		return		*cName;
	}
	xr_resource_named()	: cName(0)		{ }
	~xr_resource_named()				{ }
};

// resptr_BASE
template <class T>
class resptr_base
{
public:
	T *					p_;
protected:
	// ref-counting
	void				_inc	()									{	if (0==p_) return;	p_->dwReference++;														}
	void				_dec	()									{	if (0==p_) return;	p_->dwReference--; if (0==p_->dwReference) xr_delete(p_);				}
public:
	__forceinline 		void		_set	(T * rhs) 							{	if (0!=rhs) rhs->dwReference++;	_dec(); p_ = rhs;											}
	__forceinline 		void		_set	(resptr_base<T> const & rhs)		{	T* prhs = rhs._get(); _set(prhs);															}
	__forceinline 		T *			_get	() const							{	return p_;																					}
	//void				_clear	()									{	p_ = 0;																						}
};

// resptr_CORE
template <class T, typename C>
class resptr_core		: public C
{
protected:
	typedef resptr_core			this_type;
	typedef resptr_core<T,C>	self;
public:
						// construction
						resptr_core		()												{	p_ = 0;						}
						resptr_core		(T * p, bool add_ref = true) 					{	p_ = p;	if(add_ref)	_inc(); }
						resptr_core		(const self & rhs)								{	p_ = rhs.p_; _inc();		}
						~resptr_core	()												{	_dec();						}

						// assignment
	self &				operator=		(const self & rhs)								{	_set(rhs);	return (self&)*this;	}

						// accessors
	T &					operator*() const												{	return *p_;	}
	T *					operator->() const												{	return p_;	}

						// unspecified bool type
						typedef T * (resptr_core::*unspecified_bool_type) () const;
						operator unspecified_bool_type () const							{	return p_ == 0? 0: &resptr_core::_get;	}
						bool operator!	() const										{	return p_ == 0;	}

						// fast swapping
	void				swap			(self & rhs)									{	T * tmp = p_; p_ = rhs.p_; rhs.p_ = tmp;	}
};

// res_ptr == res_ptr
// res_ptr != res_ptr
// const res_ptr == ptr
// const res_ptr != ptr
// ptr == const res_ptr
// ptr != const res_ptr
// res_ptr < res_ptr
// res_ptr > res_ptr
template<class T, class U, typename D>	inline bool operator	==	(resptr_core<T,D> const & a, resptr_core<U,D> const & b)		{ return a._get() == b._get();						}
template<class T, class U, typename D>	inline bool operator	!=	(resptr_core<T,D> const & a, resptr_core<U,D> const & b)		{ return a._get() != b._get();						}
template<class T, typename D>			inline bool operator	==	(resptr_core<T,D> const & a, T * b)								{ return a._get() == b;								}
template<class T, typename D>			inline bool operator	!=	(resptr_core<T,D> const & a, T * b)								{ return a._get() != b;								}
template<class T, typename D>			inline bool operator	==	(T * a, resptr_core<T,D> const & b)								{ return a == b._get();								}
template<class T, typename D>			inline bool operator	!=	(T * a, resptr_core<T,D> const & b)								{ return a != b._get();								}
template<class T, typename D>			inline bool operator	<	(resptr_core<T,D> const & a, resptr_core<T,D> const & b)		{ return std::less<T *>()(a._get(), b._get());		}
template<class T, typename D>			inline bool operator	>	(resptr_core<T,D> const & a, resptr_core<T,D> const & b)		{ return std::greater<T *>()(a._get(), b._get());	}

// externally visible swap
//template<class T, typename D> void swap	(resptr_core<T,D> & lhs, resptr_core<T,D> & rhs)									{ lhs.swap(rhs);	}

// mem_fn support
//template<class T, typename D> T * get_pointer(resptr_core<T,D> const & p)													{ return p.get();	}

// casting
//template<class T, class U, typename D> resptr_core<T,D> static_pointer_cast(resptr_core<U,D> const & p)						{ return static_cast<T *>(p.get());				}
//template<class T, class U, typename D> resptr_core<T,D> dynamic_pointer_cast(resptr_core<U,D> const & p)					{ return dynamic_cast<T *>(p.get());			}


