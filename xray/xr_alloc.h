#pragma once
#include <math.h>
#include "_type_traits.h"

class __declspec(dllimport) xrMemory
{
public:
	void* mem_alloc(unsigned int);
	void mem_free(void*);
};

extern __declspec(dllimport) xrMemory Memory;

template<class T>
class __declspec(dllimport) xalloc
{
public:
	//    typedefs
	typedef T value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	template<typename U>
	struct rebind {
		typedef xalloc<U> other;
	};
	T * allocate(unsigned int n, const void *p) { return (T*)Memory.mem_alloc(sizeof(T) * n); };
	void construct(T *p, T *_Val) { if (p) {*p = *_Val;} }
};


template <class T>
__forceinline	T*		xr_new		()
{
	T* ptr	= (T*)Memory.mem_alloc(sizeof(T), typeid(T).name());
	return new (ptr) T();
}

template <bool _is_pm, typename T>
struct xr_special_free
{
	__forceinline void operator()(T* &ptr)
	{
		void*	_real_ptr	= dynamic_cast<void*>(ptr);
		ptr->~T			();
		Memory.mem_free	(_real_ptr);
	}
};

template <typename T>
struct xr_special_free<false,T>
{
	__forceinline void operator()(T* &ptr)
	{
		ptr->~T			();
		Memory.mem_free	(ptr);
	}
};
template <class T>
__forceinline	void	xr_delete	(T* &ptr)
{
	if (ptr) 
	{
		xr_special_free<is_polymorphic<T>::result,T>()(ptr);
		ptr = NULL;
	}
}
