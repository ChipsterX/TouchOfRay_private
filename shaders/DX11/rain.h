#ifndef	RAIN_H
#define RAIN_H

#include "common.h"

float3 GetNVNMap(Texture3D s_texture, float2 tc, float time)
{
	//	Unpack NVidia normal map
	float4 water = s_texture.SampleBias(smp_base, float3(tc, time), -3.) - 0.5;

	//	Swizzle
	water.xyz = water.wyz;

	//	Renormalize (*2) and scale (*3)
	water.xyz *= 6;
	water.y = 0.1;

	return water.xyz;
}

float3 GetWaterNMap(Texture2D s_texture, float2 tc)
{
	//	Unpack normal map
	float4 water = s_texture.Sample(smp_base, tc);
	water.xyz = (water.xzy - 0.5) * 2;
	water.xyz *= 0.4;
	water.y = 0.1;

	return water.xyz;
}

#endif