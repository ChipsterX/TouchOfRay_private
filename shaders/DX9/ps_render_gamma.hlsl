#include "common.h"
//oldSerpski stalker
#define const_float 0.25f

struct __textcond { float2 tc0	:  TEXCOORD0; };
uniform	sampler2D	s_gamma;
uniform float2 		gamma;

float4 main(__textcond I) : COLOR
{
	float4 final = tex2D(s_image, I.tc0);

	if (gamma.x >= 1)
		final += (final*gamma.x)*const_float;

	if (gamma.x <= 1)
		final += (final*gamma.x)*const_float;

	return final;
} 
