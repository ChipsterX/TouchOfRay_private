/*
Vignette shaders

If you want to use this code in your project, just do it but...
don't be a douchebag, and do not remove this header. Thanks.

By oldSerpskiStalker & LVutner 2020
*/

#include "common.h"
#define VALUE_VIGNETTE -0.5f 			// Область затемнения, для первого типа
#define TYPE_1 							// Тип 1

struct vignette_DX10 
{ 
	float4 tc0:TEXCOORD0; 
};

float vignette_eff(float2 center)
{
	float dist = distance(center, float2(0.5,0.5));
	dist = smoothstep(2.0, VALUE_VIGNETTE, dist);
	return saturate(dist);	
}

float4 main(vignette_DX10 I) : SV_Target
{
	float3 col 		= s_image.Sample(smp_rtlinear, I.tc0);
#ifndef TYPE_1
	float2 center 	= I.tc0;
#endif
	if (nv_color.w >= 1.f)
		return float4(col, 1.0f);
	else
#ifdef TYPE_1	
		col *= 1.0f - (1.0f * saturate(distance(I.tc0, float2(0.5f, 0.5f))));
#else		
		col *= vignette_eff(center);
#endif			
	return float4(col, 1.0f);
} 
