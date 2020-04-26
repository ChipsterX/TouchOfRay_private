/*
Fog shaders

Based on vanilla fog code and exponential calculations
Height based fog + linear fog with pseudo-scattering

If you want to use this code in your project, just do it but...
don't be a douchebag, and do not remove this header. Thanks.

By oldSerpskiStalker & LVutner 2020
*/

#ifndef FOG_H
#define FOG_H

#include "common.h"
#define NEW_VER_FOG		
		  
float4 fog_shaders_values;
float4 volumetric_fog;
static const float volumetric_vog_float = volumetric_fog.x;

//Linear fog
float4 linear_fog(float3 P)
{
#ifndef NEW_VER_FOG
	float sun_intensity = 0.05;
#endif
	//get distance
	float distance = length(P.xyz);

	//calculate linear fog
	float fog = saturate(distance*fog_params.w + fog_params.x);
#ifndef NEW_VER_FOG
	//calculate pseudoscattering!
    float sun = max(saturate(dot(-eye_direction, normalize(L_sun_dir_w))), 0);

    // final color mixing
    float3 final_color = lerp(fog_color.rgb, (L_sun_color.rgb * sun_intensity) + fog_color.rgb, smoothstep(0.3, 0.7, pow(sun, 1.5)));

	return float4(final_color.xyz,fog);
#else
	return float4(fog_color.xyz,fog);
#endif	
}

//Ground fog	
float4 ground_fog(float3 P)
{
	//our settings
#ifndef NEW_VER_FOG	
	float sun_intensity = 0.05;
	float noise_intensity = 0.0015;
#endif	
	float height = fog_shaders_values.x;
	float density = fog_shaders_values.y;
	float max_dist = fog_shaders_values.z; 
	float min_dist = fog_shaders_values.w; 	
	int error = 1;
	
	//get world position and distance!
	float3 P_world = mul(m_v2w,float4(P,1.f)).xyz;
	float distance = length(P.xyz);
	
	//height falloff (workaround for hard edges)
	float falloff = abs(P_world.y - height);

	//cut ugly fog!!1!
	if(P_world.y > height+0.01)
		error = 0;
	
	//calculate exponential fog
	float fog = pow(1.0f - exp(-max(0.0f, distance - min_dist) / ((max_dist - min_dist))), 2.0)*density;
	
	//add second dynamic layer
#ifndef NEW_VER_FOG
	fog += (fog*0.1) * sin(timers.x*0.75);

	//calculate pseudoscattering!
    float sun = max(saturate(dot(-eye_direction, normalize(L_sun_dir_w))), 0);

    // final color mixing
    float3 final_color = lerp(fog_color.rgb, (L_sun_color.rgb * sun_intensity) + fog_color.rgb, smoothstep(0.3, 0.7, pow(sun, 1.5)));

	return float4(final_color.xyz,saturate(fog*falloff*error*volumetric_vog_float));
#else 
	fog += (fog*0.125) * sin(timers.x*0.75);
	return float4(fog_color.xyz,saturate(fog*falloff*error*volumetric_vog_float));
#endif
}

#endif