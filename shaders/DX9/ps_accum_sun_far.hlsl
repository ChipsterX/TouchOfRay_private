#include "common.h"
#include "lmodel.h"
#include "shadow.h"

#define SUN_FARPLANE 100.f


float4 	main		( float2 tc : TEXCOORD0, float4 tcJ : TEXCOORD1 ) : COLOR
{
  float4 	_P	= tex2D 	(s_position, 	tc);
		_P.w 	= 1.f		;
	float4 	PS	= mul		(m_shadow, 	_P);

#ifndef	SUN_QUALITY
#ifdef	USE_HWSMAP
	float 	s 	= sunmask(_P)*sample_hw_pcf	(PS, float4(0,0,0,0) );
#else
	float 	s 	= sunmask(_P)*sample_sw	(PS.xy / PS.w, float2(0,0), PS.z);
#endif
#else	//	SUN_QUALITY
	float 	s 	= sunmask(_P);
	#if SUN_QUALITY==1
		s 	*= shadow		(PS);
	#elif SUN_QUALITY==2
		s 	*= shadowtest_sun 	(PS,tcJ);
	#endif
#endif	//	SUN_QUALITY

	return 	s	;
}
#else
float4 	main		( float2 tc : TEXCOORD0, float4 tcJ : TEXCOORD1 ) : COLOR
{
  float4 _P		= tex2D 	(s_position, 	tc);
  float4  _N		= tex2D 	(s_normal,   	tc);
  float4  _C		= tex2D 	(s_diffuse,   	tc);

	// ----- light-model
	float 	m	= xmaterial	;
# ifndef USE_R2_STATIC_SUN
			m 	= _P.w		;
# endif
	float4	light 	= plight_infinity (m,_P,_N,_C,Ldynamic_dir);

	// ----- shadow
  	float4 	P4 	= float4	(_P.x,_P.y,_P.z,1);
	float4 	PS	= mul		(m_shadow, 	P4);

#ifndef	SUN_QUALITY
#ifdef	USE_HWSMAP
	float 	s 	= sunmask(P4)*sample_hw_pcf	(PS, float4(0,0,0,0) );
#else
	float 	s 	= sunmask(P4)*sample_sw	(PS.xy / PS.w, float2(0,0), PS.z);
#endif
#else	//	SUN_QUALITY
	float 	s 	= sunmask(P4);
	#if SUN_QUALITY==1
		s 	*= shadow		(PS);
	#elif SUN_QUALITY==2
		s 	*= shadowtest_sun 	(PS,tcJ);
	#endif
#endif	//	SUN_QUALITY


#ifdef 	SUN_FARPLANE
	float f		= saturate	(_P.z/SUN_FARPLANE);
	s			= lerp		(s, 0.333h, f*f);
#endif

	return 		blend		(Ldynamic_color * light * s, tc);
}
#endif