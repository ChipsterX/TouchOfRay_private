#ifndef NIGHTVISION_H
#define NIGHTVISION_H

#include "common.h"

float3 nightvision(float3 img, float2 tc)
{
	if(nv_color.w <= 1.f)
		return img;

    float noise  = frac(sin(dot(tc, float2(12.0, 78.0) + (timers.x*3.17f) )) * 43758.0); 
      
	//fast color remover
    img.rgb = desaturate(img.r, img.g, img.b, 1.f)*saturate(nv_color.xyz);
	img.rgb *= nv_color.w;
    //////////////////////////////////////////////////////////////////////////////////////////
    // vignette
    img += nv_postprocessing.z * sin(tc.y*timers.x);
    img += nv_postprocessing.y * sin(timers.x*50.f); 
    img += noise*nv_postprocessing.x;

 	img *= nv_postprocessing.w-(distance(tc.xy,float2( 0.5f, 0.5f)))*1.5f;   
   	return img.xyz; 
}
#endif
