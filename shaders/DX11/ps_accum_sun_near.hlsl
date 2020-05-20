#include "common.h"
#include "lmodel.h"

#ifdef USE_MINMAX_SM
#define SM_MINMAX
#endif

#if SUN_QUALITY>2
#define ULTRA_SHADOWS_ON
#endif //	SUN_QUALITY>2

#ifdef ULTRA_SHADOWS_ON
#define USE_ULTRA_SHADOWS
#endif

#include "shadow.h"

#ifdef USE_SUNFILTER
#ifdef MSAA_OPTIMIZATION
float4 main ( v2p_volume I, uint iSample : SV_SAMPLEINDEX  ) : SV_Target
#else
float4 main ( v2p_volume I  ) : SV_Target
#endif
{
	gbuffer_data gbd = gbuffer_load_data( GLD_P(I.tc, I.hpos, ISAMPLE) );

	//	Emulate virtual offset
	gbd.P += gbd.N*0.015f;

	float4 _P = float4( gbd.P, 1.f);

	float4 PS = mul( m_shadow,  _P );

	float s	= shadowtest_sun( PS, I.tcJ ) * sunmask( _P );

	return s;
}
#else
#ifdef MSAA_OPTIMIZATION
float4 main ( v2p_volume I,  uint iSample : SV_SAMPLEINDEX ) : SV_Target
#else
float4 main ( v2p_volume I ) : SV_Target
#endif
{
	gbuffer_data gbd = gbuffer_load_data( GLD_P(I.tc.xy/I.tc.w, I.hpos, ISAMPLE) );

	//	Emulate virtual offset
	gbd.P += gbd.N*0.015f;

	float4 _P = float4( gbd.P, gbd.mtl );
	float4  _N = float4( gbd.N, gbd.hemi );
	float4  _C = float4( gbd.C, gbd.gloss );
	
	// ----- light-model
	float	m	= _P.w;

	float4	light	= plight_infinity ( m, _P, _N, _C, Ldynamic_dir );

	// ----- shadow
  	float4 	P4 	= float4( _P.x, _P.y, _P.z, 1.f);
	float4 	PS	= mul( m_shadow, P4 );
	float 	s 	= sunmask( P4 );
	#if defined	(USE_SJITTER) || SUN_QUALITY==2 // Hight quality
	  		s 	*= shadow_high(PS); //shadowtest_sun( PS, float4(0,0,0,0) );
	#else
#ifdef SM_MINMAX
		#ifdef USE_SJITTER
	  		s 	*= shadow_dx10_1( PS, I.tcJ, I.hpos.xy );
		#else
			s *= shadow( PS );
		#endif
#else
			s *= shadow( PS );
#endif
	#endif

	return 		blend( Ldynamic_color * light * s, I.tc );
}
#endif