#include "common.h"
#include "lmodel.h"

//	TODO: DX10: Move to Load
#ifdef MSAA_OPTIMIZATION
float4 main ( float4 tc:TEXCOORD0, float4 pos2d : SV_Position, uint iSample : SV_SAMPLEINDEX ) : SV_Target
#else
float4 main ( float4 tc:TEXCOORD0, float4 pos2d : SV_Position ) : SV_Target
#endif
{
	const float bias_mul 	= 0.999f;

 	// Sample the fat framebuffer:
	float2	tcProj	= tc.xy / tc.w;

	gbuffer_data gbd = gbuffer_load_data( GLD_P(tcProj, pos2d, ISAMPLE) );

	float4 _P		= float4( gbd.P,gbd.mtl );
	float4 _N		= float4( gbd.N,gbd.hemi );
	float4 _C 		= float4( gbd.C, gbd.gloss );
	
	float 	m	= _P.w	;

	float	rsqr;
	float4	light 		= plight_local( m, _P, _N, _C, Ldynamic_pos, Ldynamic_pos.w, rsqr );
	return 	blendp( Ldynamic_color * light, tc);
}
