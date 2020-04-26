#include "common.h"
#include "lmodel.h"
#include "shadow.h"

#ifdef MSAA_OPTIMIZATION
float4 	main	( p_aa_AA_sun I, float4 pos2d : SV_Position, uint iSample : SV_SAMPLEINDEX ) : SV_Target
#else
float4 	main	( p_aa_AA_sun I, float4 pos2d : SV_Position ) : SV_Target
#endif
{
	gbuffer_data gbd = gbuffer_load_data( GLD_P(I.tc, pos2d, ISAMPLE) );

	float4 	_P	= float4( gbd.P, gbd.mtl );
	float4	_N	= float4( gbd.N, gbd.hemi );

	// ----- light-model
	float 	m	= _P.w	;

	float4	light	= plight_infinity( m, _P, _N, Ldynamic_dir );

	// ----- shadow
	float4 	s_sum;
		s_sum.x	= s_smap.Sample( smp_nofilter, I.LT).x;
		s_sum.y = s_smap.Sample( smp_nofilter, I.RT).y;
		s_sum.z	= s_smap.Sample( smp_nofilter, I.LB).z;
		s_sum.w = s_smap.Sample( smp_nofilter, I.RB).w;
	float 	s 	= ((s_sum.x+s_sum.y)+(s_sum.z+s_sum.w))*(1.h/4.h);

	return 		Ldynamic_color * light * s;
}
