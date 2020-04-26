/*
Screen Space Local Reflections

Credits goes to Danil Baryshev, Misha Judin, Xerxes1138 and KD.

References:
https://habr.com/ru/post/244367/
https://www.amk-team.ru/forum/topic/14078-sslr

If you want to use this code in your project, just do it but...
don't be a douchebag, and do not remove this header. Thanks.

By LVutner 2020
*/

#ifndef SSR_H
#define SSR_H
#include "common.h"

	#define SSR_SAMPLES int(8)
	#define SSR_RANGE float(150.0)
/*
#if !defined(SSR_QUALITY)
	#define SSR_SAMPLES int(1)
	#define SSR_RANGE float(1.0)
#elif SSR_QUALITY==1		// Low
	#define SSR_SAMPLES int(8)
	#define SSR_RANGE float(100.0)
#elif SSR_QUALITY==2		// Medium
	#define SSR_SAMPLES int(16)
	#define SSR_RANGE float(200.0)
#elif SSR_QUALITY==3		// High
	#define SSR_SAMPLES int(32)
	#define SSR_RANGE float(400.0)
#endif
*/
float depth_wo_sky(float depth)
{
	return depth * (1 - step(0.001, abs(depth - 10000)));
} 

float RayAttenBorder (float2 pos, float value)
{
	float borderDist = min(1.0 - max(pos.x, pos.y), min(pos.x, pos.y));
	return saturate(borderDist > value ? 1.0 : borderDist / value);
}

float3 specular_phong(float3 pnt, float3 normal, float3 light_direction, float intensity)
{
    float3 light = L_sun_color.rgb * pow( abs( dot(normalize(pnt + light_direction), normal) ), 128.0);
    return light*intensity;
}

float4 ssr(float3 position, float3 normal)
{
	int error = 1; 
	float ray_length = 0.5;
	
	float2 refl_tc = (0,0);	
	float3 v2point = normalize(position - eye_position);
	float3 vreflect = normalize(reflect(v2point,(normal)));
	float fresnel = saturate (dot(vreflect,v2point));
		
	[unroll (SSR_SAMPLES)]
	for(int i = 0; i < SSR_SAMPLES; i++)
	{
		float3 new_position = position + vreflect * ray_length;
		float depth = mul(m_V, float4(position,1)).z;
		
		//convert our position into texcoord
		float4 view_position = mul(m_VP, float4(new_position, 1));
		float4 p2s = proj_to_screen(view_position);
		refl_tc = p2s.xy /= p2s.w;

	#ifndef USE_MSAA
	   float depth_base = s_position.Sample( smp_nofilter, refl_tc.xy).z;
	#else
	   float depth_base = s_position.Load( int3( refl_tc.xy * pos_decompression_params2.xy ,0),0 ).z;
	#endif
			
		//remove sky, remove reflections under angle, set up max range for them
		if((depth_base == depth_wo_sky(depth_base)) || (eye_direction.y < -0.8) || (depth_base > SSR_RANGE))
			error = 0;

		if((depth - depth_base) > 0.f)
			error = 0;
		
		ray_length = length(depth_base - depth);
	}

	float edge_cleaner = RayAttenBorder(refl_tc.xy, 0.05);
	
	float3 img = s_image.Sample( smp_rtlinear, refl_tc.xy);
	float3 specular = specular_phong(v2point,normal,L_sun_dir_w,2.f);
	img.xyz += specular;
	return float4(img.xyz, error*edge_cleaner*fresnel);
}

#endif