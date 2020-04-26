#include "common.h"
//Fixed glows by skyloader
struct v2p
{
	float2		Tex0	: TEXCOORD0;
	float4		Color	: COLOR;
	float4		tctexgen: TEXCOORD1;
	float4		hpos	: SV_Position;
};

//////////////////////////////////////////////////////////////////////////////////////////
// Pixel
//////////////////////////////////////////////////////////////////////////////////////////

//	Must be less than view near
#define	DEPTH_EPSILON	0.1h
//////////////////////////////////////////////////////////////////////////////////////////
float4 main( v2p I ) : SV_Target
{
	float4	t_base 	= ( s_base.Sample( smp_base, I.Tex0) * I.Color ) / 1.5h;

	float2 tcProj	= I.tctexgen.xy / I.tctexgen.w;

	gbuffer_data gbd = gbuffer_load_data( tcProj, I.hpos );

	float4 _P		= float4( gbd.P, gbd.mtl );
	half spaceDepth = _P.z-I.tctexgen.z-DEPTH_EPSILON;
	if (spaceDepth < -2*DEPTH_EPSILON ) spaceDepth = 100000.0h; //  filter for skybox

	t_base.a *= Contrast( saturate(spaceDepth*1.3h), 2);
	t_base.rgb *= Contrast( saturate(spaceDepth*1.3h), 2);


	return  t_base;
}
