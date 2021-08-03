/*#pragma once

#include "xr_alloc.h"
#include <vector>

// vector


class dumb
{
	unsigned int dummy;
};
template	<typename T, typename allocator = xalloc<T> >
class xr_vector : public dumb, public std::vector<T,allocator> {
private:
	typedef std::vector<T,allocator>	inherited;

public:
	typedef allocator					allocator_type;

public:
			xr_vector			()									: inherited	()					{}
			xr_vector			(size_t _count, const T& _value)	: inherited	(_count,_value)		{}
	explicit xr_vector			(size_t _count)						: inherited (_count)			{}
	unsigned int		size				() const							{ return (unsigned int)inherited::size();} 

	void	clear_and_free		()									{ inherited::clear();			}
	void	clear_not_free		()									{ erase(begin(),end());			}
	void	clear_and_reserve	()									{ if ( capacity() <= (size()+size()/4) ) clear_not_free(); else { unsigned int old=size(); clear_and_free(); reserve(old); } }
	void	clear				()									{ clear_and_free	();			}

	const_reference operator[]	(size_type _Pos) const				{ {VERIFY(_Pos<size());} return (*(begin() + _Pos)); }
	reference operator[]		(size_type _Pos)					{ {VERIFY(_Pos<size());} return (*(begin() + _Pos)); }
};*/

#pragma once

#include "xr_alloc.h"
#include <vector>
/*
namespace std {
template <class T, class alloc = xalloc<T> >
class vector
{
public:
	typedef T value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type* iterator;
	typedef const value_type* const_iterator;

	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef size_t size_type;

protected:
	int stub;
	T *_Myfirst;
	T *_Mylast;
	T *_Myend;

private:
	template <class _Tp>
	inline void _Copy_Construct(_Tp* __p, const _Tp& __val) {
	memset((char*)__p, _STLP_SHRED_BYTE, sizeof(_Tp));
}

public:
	iterator		erase(iterator __first, iterator __last);
	void			clear() {erase(begin(), end());}
	size_t			size() const {return (_Mylast - _Myfirst)/sizeof(T*);};
	bool			empty() const {return (this->_Mylast == this->_Myfirst);};
	size_t			capacity() const {return (_Myend - _Myfirst)/sizeof(T*);};
	iterator		begin() {return _Myfirst;};
	iterator		end() {return _Mylast;};
	const_iterator	begin() const {return _Myfirst;};
	const_iterator	end() const {return _Mylast;};
	void			push_back(const T& val);
	void			pop_back();
};
};*/
template	<typename T, typename allocator = xalloc<T> >
class xr_vector : public std::vector<T,allocator> {
private:
	typedef std::vector<T,allocator>	inherited;

public:
	typedef allocator					allocator_type;

public:
			xr_vector			()									: inherited	()					{}
			xr_vector			(size_t _count, const T& _value)	: inherited	(_count,_value)		{}
	explicit xr_vector			(size_t _count)						: inherited (_count)			{}
	unsigned int		size				() const							{ return (unsigned int)inherited::size();} 

//	void	clear_and_free		()									{ inherited::clear();			}
	void	clear_not_free		();//									{ erase(begin(),end());			}
//	void	clear_and_reserve	()									{ if ( capacity() <= (size()+size()/4) ) clear_not_free(); else { unsigned int old=size(); clear_and_free(); reserve(old); } }
//	void	clear				()									{ clear_and_free	();			}

	const_reference operator[]	(size_type _Pos) const				{ {VERIFY(_Pos<size());} return (*(begin() + _Pos)); }
	reference operator[]		(size_type _Pos)					{ {VERIFY(_Pos<size());} return (*(begin() + _Pos)); }
};
