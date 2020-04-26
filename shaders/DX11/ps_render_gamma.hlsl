#include "common.h"
#define const_float 0.25f
//oldSerpski stalker

struct __textcond_DX11 
{ 
	float4 tc0:TEXCOORD0; 
};

uniform	sampler2D	s_gamma;
uniform float2 		gamma;

float4 main(__textcond_DX11 I) : SV_Target
{
	float4 final;
	
	final = s_image.Sample(smp_rtlinear, I.tc0);

	if (gamma.x >= 1)
		final += (final*gamma.x)*const_float;

	if (gamma.x <= 1)
		final += (final*gamma.x)*const_float;
	
	return final;
} 
