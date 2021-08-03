#pragma once

#include "xr_alloc.h"
//#include "xr_algorithm.h"
#include <functional>

template <typename K, class V, class P=std::less<K>, typename allocator = xalloc<std::pair<K,V> > >
class xr_map
{
  int stub;
  void *_Myhead;
  unsigned int _Mysize;
};

template <class key, class value>
struct xr_multimap
{
  int stub;
  void *_Myhead;
  unsigned int _Mysize;
};

#define DEFINE_MAP(K,T,N)			typedef xr_map< K , T > N;	//	typedef N::iterator I;
#define DEFINE_MAP_PRED(K,T,N,P)	typedef xr_map< K, T, P > N;//	typedef N::iterator I;