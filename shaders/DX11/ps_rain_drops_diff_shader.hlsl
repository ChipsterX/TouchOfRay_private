#include "common.h"

struct AntiAliasingStruct
{
	float2		texCoord0		:		TEXCOORD0;
	float2		texCoord1		:		TEXCOORD1;
	float2		texCoord2		:		TEXCOORD2;
	float2		texCoord3		:		TEXCOORD3;
	float2		texCoord4		:		TEXCOORD4;
	float4		texCoord5		:		TEXCOORD5;
	float4		texCoord6		:		TEXCOORD6;
};

Texture2D		s_rain_drops0_diff;
uniform 	    float2 rain_drops_params0_diff;

float4 main(AntiAliasingStruct INStruct) : SV_Target
{
	float4 rain_drops_distortion = s_rain_drops0_diff.Sample(smp_rtlinear, INStruct.texCoord0);
	float2 texcoord_offset = (rain_drops_distortion.xy - (127.h / 255.h))*def_distort; 
	float2 texcoord = INStruct.texCoord0 + texcoord_offset * rain_drops_params0_diff.x;	
	float3 scene = s_image.Sample(smp_rtlinear, texcoord);
	return float4(scene, 1.f);
}