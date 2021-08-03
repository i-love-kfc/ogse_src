#pragma once
/*

//#include <vector>
//#include <map>
#include <string.h>
#include <windows.h>*/
#include "base_types.h"/*

#define fdSGN	0x080000000		// mask for sign bit
const	float		PI_DIV_2	= 1.5707963267948966192313216916398f;
const	float		PI_MUL_2	= 6.2831853071795864769252867665590f;

#define IC inline
#define ICF __forceinline

inline BOOL negative(const float &f)	{ return (*((unsigned*)(&f))&fdSGN);	}

/*template<typename T>
inline T*xr_new(void)
{
T* p = Memory.mem_alloc(sizeof(T));
p->p();
}*/
#define DEG2RAD(deg) (M_PI * deg / 180.f)
#define MUL05(v) (v * 0.5f)
template <typename T>
inline T saturate(T v)
{
	clamp(v, (T)0, (T)1);
	return v;
}

template<class T>
inline T lerp(T a1, T a2, T v)
{
	return (a1 + (a2 - a1) * v);
}
/*
void __cdecl Log(const char*);
void __cdecl Msg(const char*, ...);

//#define xr_vector std::vector
//#define xr_map std::map

template <typename T>
inline void clamp(T &v, T a, T b)
{
	if (v < a)
	{
		v = a;
	}
	else if (v > b)
	{
		v = b;
	}
}

template <typename T>
inline T clampr	( const T& val, const T& _low, const T& _high ){
	if		( val<_low	)	return _low; 
	else if	( val>_high )	return _high;
	else					return val;
};
inline void strconcat(size_t buf_size, char*buf, const char* s1, const char* s2)
{
	strcpy_s(buf, buf_size, s1);
	strcat_s(buf, buf_size - strlen(s1), s2);
}
/*
struct xalloc<CControlAnimation::SAnimationEvent> {
DWORD size_type; 	// offset: 00, len=4
int difference_type; 	// offset: 00, len=4
CControlAnimation::SAnimationEvent* pointer; 	// offset: 00, len=4
CControlAnimation::SAnimationEvent* const_pointer; 	// offset: 00, len=4
CControlAnimation::SAnimationEvent* reference; 	// offset: 00, len=4
CControlAnimation::SAnimationEvent* const_reference; 	// offset: 00, len=4
invalid value_type; 	// offset: 00, len=12
CControlAnimation::SAnimationEvent* address(CControlAnimation::SAnimationEvent*, );
CControlAnimation::SAnimationEvent* address(CControlAnimation::SAnimationEvent*, );
void xalloc<CControlAnimation::SAnimationEvent>(xalloc<CControlAnimation::SAnimationEvent>*, );
void xalloc<CControlAnimation::SAnimationEvent>(xalloc<CControlAnimation::SAnimationEvent>*, );
void xalloc<CControlAnimation::SAnimationEvent>();
CControlAnimation::SAnimationEvent* allocate(DWORD, void*, );
char* _Charalloc(DWORD, );
void deallocate(void*, DWORD, );
void deallocate(CControlAnimation::SAnimationEvent*, DWORD, );
void xalloc<CControlAnimation::SAnimationEvent>::construct(CControlAnimation::SAnimationEvent*, CControlAnimation::SAnimationEvent*, ); 	// offset: 00, len=22
void destroy(CControlAnimation::SAnimationEvent*, );
DWORD max_size();
invalid ?$rebind@USAnimationEvent@CControlAnimation@@; 	// offset: 00, len=1
};*/
/*
template<class T> 
struct /*__declspec(dllimport)*/ /*_vector2
{
	T x,y;
	_vector2& set(float _u, float _v)				{ x=T(_u); y=T(_v);				return *this;	}
	_vector2& set(double _u, double _v)			{ x=T(_u); y=T(_v);				return *this;	}
	_vector2& set(int _u, int _v)					{ x=T(_u); y=T(_v);				return *this;	}
	_vector2& set(const _vector2 &p)					{ x=p.x; y=p.y;					return *this;	}
	_vector2& div(const T s)						{ x/=s; y/=s;					return *this;	}
	_vector2& add(T p)								{ x+=p; y+=p;					return *this;	}
	_vector2& add(const _vector2 &p)					{ x+=p.x; y+=p.y;				return *this;	}
	_vector2& add(const _vector2 &p1, const _vector2 &p2)	{ x=p1.x+p2.x; y=p1.y+p2.y;		return *this;	}
	_vector2& add(const _vector2 &p, float d)			{ x=p.x+d; y=p.y+d;				return *this;	}
};

typedef _vector2<float> Fvector2;
typedef _vector2<int> Ivector2;

template<class T> 
struct _vector3
{
public:
	typedef T			TYPE;
	typedef _vector3<T>	Self;
	typedef Self&		SelfRef;
	typedef const Self&	SelfCRef;

	T x,y,z;
//	_vector3(void):x(0), y(0), z(0) {};
//	_vector3(T x_, T y_, T z_):x(x_), y(y_), z(z_) {};
	_vector3& set(T x_, T y_, T z_)
	{
		x = x_; y = y_; z = z_;
		return *this;
	};
	_vector3& set(const _vector3 &v)
	{
		*this = v;
		return *this;
	}
	_vector3& setHP(T h, T p)
	{
		float cos_p = cosf(p);
		x = -cos_p * sinf(h);
		y = sinf(p);
		z = cosf(h) * cos_p;
		return *this;
	};

	ICF	SelfRef	sub(const Self &v)						{ x-=v.x;	y-=v.y;		z-=v.z;			return *this;	};
	ICF SelfRef	sub(T s)								{ x-=s;		y-=s;		z-=s;			return *this;	};
	ICF	SelfRef	sub(const Self &a, const Self &v)		{ x=a.x-v.x;y=a.y-v.y;	z=a.z-v.z;		return *this;	};
	ICF SelfRef	sub(const Self &a, T s)					{ x=a.x-s;  y=a.y-s;	z=a.z-s;		return *this;	};

	ICF	SelfRef	add(const Self &v)						{ x+=v.x;	y+=v.y;		z+=v.z;			return *this;	};
	ICF SelfRef	add(T s)								{ x+=s;		y+=s;		z+=s;			return *this;	};
	ICF SelfRef	add(const Self &a, const Self &v)		{ x=a.x+v.x;y=a.y+v.y;	z=a.z+v.z;		return *this;	};
	ICF SelfRef	add(const Self &a, T s)					{ x=a.x+s;  y=a.y+s;	z=a.z+s;		return *this;	};

	ICF	SelfRef	div(const Self &v)						{ x/=v.x;	y/=v.y;  z/=v.z;			return *this;	};
	ICF SelfRef	div(T s)								{ x/=s;		y/=s;    z/=s;				return *this;	};
	ICF	SelfRef	div(const Self &a, const Self &v)		{ x=a.x/v.x;y=a.y/v.y;	z=a.z/v.z;		return *this;	};
	ICF SelfRef	div(const Self &a, T s)					{ x=a.x/s;  y=a.y/s;	z=a.z/s;		return *this;	};

	IC	SelfRef	min(const Self &v1,const Self &v2)		{ x = _min(v1.x,v2.x); y = _min(v1.y,v2.y); z = _min(v1.z,v2.z);	return *this;	};
	IC	SelfRef	min(const Self &v)						{ x = _min(x,v.x);	y = _min(y,v.y);	z = _min(z,v.z);			return *this;	};
	IC	SelfRef	max(const Self &v1,const Self &v2)		{ x = _max(v1.x,v2.x); y = _max(v1.y,v2.y);	z = _max(v1.z,v2.z);	return *this;	};
	IC	SelfRef	max(const Self &v)						{ x = _max(x,v.x);	y = _max(y,v.y);	z = _max(z,v.z);			return *this;	};

	_vector3& mul(T a)
	{
		x *= a; y *= a; z *= a;
		return *this;
	}
	_vector3& mul(const _vector3 &v, T a)
	{
		*this = v;
		mul(a);
		return *this;
	}
	_vector3& lerp(const _vector3 &p1, const _vector3 &p2, T t)
	{
		::lerp(x, p1.x, p2.x, t);
		::lerp(y, p1.y, p2.y, t);
		::lerp(z, p1.z, p2.z, t);
		return *this;
	}
	_vector3& mad(const _vector3 &d, T m)
	{
		x += d.x * m;
		y += d.y * m;
		z += d.z * m;
		return *this;
	}
	_vector3& mad(const _vector3 &p, const _vector3 &d, T m)
	{
		*this = p;
		return mad(d, m);
	}
	_vector3& mad(const _vector3& d, const _vector3& s)
	{
		x += d.x * s.x;
		y += d.y * s.y;
		z += d.z * s.z;
		return *this;
	}
	_vector3& mad(const _vector3 &p, const _vector3 &d, const _vector3 &s)
	{
		*this = p;
		return mad(d, s);
	}

	bool similar(const _vector3 &v, T EPS = EPS_L) const
	{
		return abs(x-v.x) < EPS && abs(y-v.y) < EPS && abs(z-v.z) < EPS;
	}
	_vector3& crossproduct(const _vector3 &v1, const _vector3 &v2)
	{
		x = v1.y*v2.z - v1.z*v2.y;
		y = v1.z*v2.x - v1.x*v2.z;
		z = v1.x*v2.y - v1.y*v2.x;
		return *this;
	}
	_vector3& crossproduct(const _vector3 &v)
	{
		return crossproduct(*this, v);
	}
	T magnitude()
	{
		return sqrt(x*x + y*y + z*z);
	}
	_vector3& normalize()
	{
		T L = magnitude();
		x /= L;
		y /= L;
		z /= L;
		return *this;
	}
	_vector3& normalize(const _vector3 &v) 
	{
		T mag= sqrt(1/(v.x*v.x + v.y*v.y + v.z*v.z));
		x = v.x * mag;
		y = v.y * mag;
		z = v.z * mag;
		return *this;	
	}

	T dotproduct(const _vector3 &v) const
	{
		return x*v.x + y*v.y + z*v.z;
	}
	_vector3&	random_dir		(CRandom& R = ::Random)
	{
		z	= cos(R.randF(0, M_PI));
		T a = R.randF(0, M_PI * 2.f);
		T r = sqrt(1 - z * z);
		T sa= sin(a);
		T ca= cos(a);
		x	= r * ca;
		y	= r * sa;
		return *this;	
	};
	_vector3&	random_dir		(const _vector3& ConeAxis, float ConeAngle, CRandom& R = ::Random)
	{
		_vector3 rnd;
		rnd.random_dir(R);
		mad(ConeAxis, rnd, R.randF(0, tanf(ConeAngle)));
		normalize();
		return *this;	
	}	
	// Distance calculation
	ICF	T		distance_to_sqr(const Self &v) const
	{	return (x-v.x)*(x-v.x) + (y-v.y)*(y-v.y) + (z-v.z)*(z-v.z);	}

	// Distance calculation
	ICF	T		distance_to(const Self &v) const 
	{	return _sqrt(distance_to_sqr(v));	}
	IC SelfRef	mknormal_non_normalized	(const Self &p0, const Self & p1, const Self &p2 )
	{
    	_vector3<T> v01,v12;
    	v01.sub( p1, p0 );
    	v12.sub( p2, p1 );
    	crossproduct( v01, v12 );
		return *this;	
    };
};
typedef _vector3<float> Fvector;
typedef _vector3<unsigned int> Ivector;

template<class T> 
struct _vector4
{
	typedef T			TYPE;
	typedef _vector4<T>	Self;
	typedef Self&		SelfRef;
	typedef const Self&	SelfCRef;
public:
	_vector4& set(T x_, T y_, T z_, T w_)
	{
		x = x_; y = y_; z = z_; w = w_;
		return *this;
	};
	T x,y,z,w;
	_vector4& mul(T a)
	{
		x *= a; y *= a; z *= a; w *= a;
		return *this;
	}
	_vector4& mul(const _vector4 &v, T a)
	{
		*this = v;
		mul(a);
		return *this;
	}
	_vector4 & lerp(const _vector4 &v1, const _vector4 &v2, T a)
	{
		::lerp(x, v1.x, v2.x, a);
		::lerp(y, v1.y, v2.y, a);
		::lerp(z, v1.z, v2.z, a);
		::lerp(w, v1.w, v2.w, a);
		return *this;
	}
	_vector4 & add(const _vector4 &v)						{ x+=v.x;	y+=v.y;		z+=v.z;		w+=v.w;		return *this; }
	_vector4 & add(T s)										{ x+=s;		y+=s;		z+=s;		w+=s;		return *this; }
	_vector4 & add(const _vector4 &a, const _vector4 &v)	{ x=a.x+v.x;y=a.y+v.y;	z=a.z+v.z;	w=a.w+v.w;	return *this; }
	_vector4 & add(const _vector4 &a, T s)					{ x=a.x+s;  y=a.y+s;	z=a.z+s;	w=a.w+s;	return *this; }

	IC	SelfRef		div(const Self &v)					{ x/=v.x;	y/=v.y;		z/=v.z;		w/=v.w;		return *this; }
	IC  SelfRef		div(T s)							{ x/=s;		y/=s;		z/=s;		w/=s;		return *this; }
	IC	SelfRef		div(const Self &a, const Self &v)	{ x=a.x/v.x;y=a.y/v.y;	z=a.z/v.z;	w=a.w/v.w;	return *this; }
	IC  SelfRef		div(const Self &a, T s)				{ x=a.x/s;  y=a.y/s;	z=a.z/s;	w=a.w/s;	return *this; }

	IC	T			magnitude_sqr ()					{ return x*x + y*y + z*z + w*w;		}
	IC	T			magnitude()							{ return _sqrt(magnitude_sqr());	}
	IC	SelfRef		normalize()							{ return mul(1/magnitude());		}
};

typedef _vector4<float> Fvector4;
typedef _vector4<s32> Ivector4;

inline int iFloor (float x)
{
	int a			= *(const int*)(&x);
	int exponent	= (127 + 31) - ((a >> 23) & 0xFF);
	int r			= (((u32)(a) << 8) | (1U << 31)) >> exponent;
	exponent		+= 31-127;
	{
		int imask	=	(!(((( (1<<(exponent)))-1)>>8)&a));
		exponent	-=	(31-127)+32;
		exponent	>>=	31;
		a			>>=	31;
		r			-=	(imask&a);
		r			&=	exponent;
		r			^=	a;
	}
	return r;
}
// maps unsigned 8 bits/channel to D3DCOLOR
inline u32	color_argb	(u32 a, u32 r, u32 g, u32 b)	{	return ((a&0xff)<<24)|((r&0xff)<<16)|((g&0xff)<<8)|(b&0xff);	}
inline u32	color_rgba	(u32 r, u32 g, u32 b, u32 a)	{	return color_argb(a,r,g,b);		}
inline u32	color_argb_f(f32 a, f32 r, f32 g, f32 b)	
{
	s32	 _r = clampr(iFloor(r*255.f),0,255);
	s32	 _g = clampr(iFloor(g*255.f),0,255);
	s32	 _b = clampr(iFloor(b*255.f),0,255);
	s32	 _a = clampr(iFloor(a*255.f),0,255);
	return color_argb(_a,_r,_g,_b);
}
inline u32	color_rgba_f(f32 r, f32 g, f32 b, f32 a)	{	return color_argb_f(a,r,g,b);	}
inline u32	color_xrgb	(u32 r, u32 g, u32 b)			{	return color_argb(0xff,r,g,b);	}
inline u32	color_get_R	(u32 rgba)						{	return (((rgba) >> 16) & 0xff);	}
inline u32	color_get_G	(u32 rgba)						{	return (((rgba) >> 8) & 0xff);	}
inline u32	color_get_B	(u32 rgba)						{	return ((rgba) & 0xff);			}
inline u32 color_get_A (u32 rgba)						{	return ((rgba) >> 24);			}
inline u32 subst_alpha	(u32 rgba, u32 a)				{	return rgba&~color_rgba(0,0,0,0xff)|color_rgba(0,0,0,a);}
inline u32 bgr2rgb		(u32 bgr)						{	return color_rgba(color_get_B(bgr),color_get_G(bgr),color_get_R(bgr),0);}
inline u32 rgb2bgr		(u32 rgb)						{	return bgr2rgb(rgb);}

IC u32							xr_strlen				( const char* S )
{	return (u32)strlen(S);			}
IC int							xr_strcmp				( const char* S1, const char* S2 )
{	return (int)strcmp(S1,S2);  }
IC LPCSTR						strconcat				( int dest_sz, char* dest, const char* S1, const char* S2, const char* S3)
{
	u32 l1 = xr_strlen(S1);
	u32 l2 = xr_strlen(S2);
	strcpy_s(dest,dest_sz,S1);
	strcat_s(dest,dest_sz-l1,S2);
	strcat_s(dest,dest_sz-l1-l2,S3);

	return dest;
}

ICF int iCeil (float x)
{
    int a			= (*(const int*)(&x));
    int exponent	= (127 + 31) - ((a >> 23) & 0xFF);
    int r			= (((u32)(a) << 8) | (1U << 31)) >> exponent;
    exponent		+= 31-127;
    {
        int imask	=	(!(((( (1<<(exponent)))-1)>>8)&a));
        exponent	-=	(31-127)+32;
        exponent	>>=	31;
        a			=	~((a-1)>>31);		/* change sign *//*
        r			-=	(imask&a);
        r			&=	exponent;
        r			^=	a;
        r			=	-r;                 /* change sign *//*
    }
    return r;								/* r = (int)(ceil(f)) *//*
}

template<class T>
struct _color
{
	T r,g,b,a;
	_color& set(const _color& v) {r = v.r; g = v.g; b = v.b; a = v.a; return *this;};
	_color& set(T _r, T _g, T _b, T _a) {r = _r; g = _g; b = _b; a = _a; return *this;};
	u32 get() const { return color_rgba_f(r,g,b,a); }
	_color&	mul_rgba(T s) 
	{
		r*=s;
		g*=s;
		b*=s;
		a*=s;
		return *this;
	};
	_color&	mul_rgb(T s) 
	{
		r*=s;
		g*=s;
		b*=s;
		return *this;
	};
	_color&	mul_rgba(const _color& c, T s) 
	{
		r=c.r*s;
		g=c.g*s;
		b=c.b*s;
		a=c.a*s;
		return *this;
	};
	_color&	mul_rgb(const _color& c, T s) 
	{
		r=c.r*s;
		g=c.g*s;
		b=c.b*s;
		return *this;
	};
	T magnitude_rgb(void) const
	{
		return sqrt(magnitude_sqr_rgb());
	}
	T magnitude_sqr_rgb(void) const
	{
		return r*r + g*g + b*b;
	}
};

typedef _color<float> Fcolor;
/*
template<class T, unsigned int N>
class svector
{
T array[N];
unsigned int count;
public:
svector() : count(0) {}

T & operator[](unsigned int);
const T & operator[](unsigned int) const;
T & begin(void);
const T & begin(void) const;
T & back(void);
const T & back(void)const;
T & end(void);
const T & end(void) const;
void clear(void);
bool empty(void);
void erase(unsigned int);
T & front(void);
const T & front(void) const ;
void insert(unsigned int, T&);
T & last(void);
const T & last(void) const;
void pop_back(void);
//void push_back(T);
void push_back(T);
unsigned int size(void) const;
};
*/
/*
template<class T>
struct _matrix {
_vector3<T> i;
T _14_;
_vector3<T> j;
T _24_;
_vector3<T> k;
T _34_;
_vector3<T> c;
T _44_;
};

typedef _matrix<float> Fmatrix;
extern __declspec(dllimport) Fmatrix Fidentity;
*//*

template <class T>
struct _sphere {
	_vector3<T>	P;
	T			R;

	IC void		set(const _vector3<T> &_P, T _R)	{ P.set(_P); R = _R; }
	IC void		set(const _sphere<T> &S)			{ P.set(S.P); R=S.R; }
};
typedef _sphere<float> Fsphere;

template <typename T>
struct _flags
{
public:
	typedef T			TYPE;
	typedef _flags<T>	Self;
	typedef Self&		SelfRef;
	typedef const Self&	SelfCRef;
public:
	T flags;
	T zero() {flags = 0; return flags;};
	BOOL is(const T mask) const
	{
		return mask==(flags&mask);
	}
	_flags&	set(const T mask, BOOL value)
	{ 
		if (value) 
			flags |= mask; 
		else 
			flags &= ~mask; 
		return *this; 
	}
	BOOL test (const T mask) const { return BOOL(flags&mask);}
	IC	SelfRef	assign	(const T mask)								{ flags	=	mask;		return *this;	}
};

typedef _flags<u8>	Flags8;
typedef _flags<u16> Flags16;
typedef _flags<unsigned int> Flags32;

const float EPS_L = 0.001;

inline bool fis_zero( float v, float cmp=EPS_S)
{ 
	return abs(v)<cmp;	
}

inline bool fsimilar( float a, float b, float cmp = EPS )
{
	return abs(a-b) < cmp;	
}

template<typename T>
struct _matrix33
{
	T m[3][3];
};

template<typename T>
struct _obb
{
	_matrix33<T> m_rotate;
	_vector3<T> m_translate;
	_vector3<T> m_halfsize;
};
typedef _obb<float> Fobb;

template<typename T>
struct _cylinder
{
	_vector3<T> m_center;
	_vector3<T> m_direction;
	T m_height;
	T m_radius;
};
typedef _cylinder<float> Fcylinder;
template<class T>
struct _plane
{
public:
	typedef T			TYPE;
	typedef _plane<T>	Self;
	typedef Self&		SelfRef;
	typedef const Self&	SelfCRef;
  _vector3<T> n;
  T d;

	ICF	SelfRef	build(const _vector3<T> &_p, const _vector3<T> &_n)
	{
		d			= - n.normalize(_n).dotproduct(_p);
		return *this;
	}
	ICF	SelfRef	build	(const _vector3<T> &v1, const _vector3<T> &v2, const _vector3<T> &v3) 
	{
		_vector3<T> t1,t2;
		n.crossproduct(t1.sub(v1,v2), t2.sub(v1,v3)).normalize();
		d = -n.dotproduct(v1);
		return *this;
	}
};
typedef _plane<float> Fplane;

template<class T>
struct _box2
{
	union{
		struct{
			_vector2<T> min;
			_vector2<T> max;
		};
		struct{
			T x1, y1;
			T x2, y2;
		};
	};
};
typedef _box2<float> Fbox2;*/