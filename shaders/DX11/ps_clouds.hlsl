#include "common.h"

struct v2p
{
	float4 color : COLOR0;  // rgb. intensity, for SM3 - tonemap prescaled
	float3 tc0 : TEXCOORD0; // .z - tonemap scale
	float2 tc1 : TEXCOORD1;
};

Texture2D s_clouds0 : register(t0);
Texture2D s_clouds1 : register(t1);

//////////////////////////////////////////////////////////////////////////////////////////
// Pixel
float4 main(v2p I) : SV_Target
{
	float4 s0 = s_clouds0.Sample(smp_base, I.tc0.xy);
	float4 s1 = s_clouds1.Sample(smp_base, I.tc1.xy);

	float4 final = I.color * (s0 + s1);

#ifdef CGIM
	final *= float4(I.tc0.zzz, 1.0f / 1.1f);
#else
	final.rgb *= I.tc0.z;
#endif

	return final;
};