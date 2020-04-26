#include "common.h"

struct 	v2p
{
 	float2 	tc0	: 	TEXCOORD0;	// base
	float4	c	:	COLOR0;		// diffuse

#ifdef	USE_SOFT_PARTICLES
//	Igor: for additional depth dest
	float4 tctexgen	: TEXCOORD1;
#endif	//	USE_SOFT_PARTICLES
};


//////////////////////////////////////////////////////////////////////////////////////////
// Pixel
uniform sampler s_distort;
float4 	main	( v2p I )	: COLOR
{
	float4	distort	= tex2D		(s_distort,I.tc0);
	float    factor	= distort.a * dot(I.c.rgb,0.33h);

	return	float4	(distort.rgb,factor);
}
