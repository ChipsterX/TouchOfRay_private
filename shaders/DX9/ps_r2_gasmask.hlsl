#include "common.h"
//gasmask shader by lvutner
struct		AntiAliasingStruct
{
	float4 texCoord0:TEXCOORD0;
	float4 texCoord1:TEXCOORD1;
	float4 texCoord2:TEXCOORD2;
	float4 texCoord3:TEXCOORD3;
	float4 texCoord4:TEXCOORD4;
	float4 texCoord5:TEXCOORD5;
	float4 texCoord6:TEXCOORD6;
};

uniform	sampler2D	s_mask_0;
uniform	sampler2D	s_mask_1;
uniform	sampler2D	s_mask_2;
uniform	sampler2D	s_mask_3;
uniform	sampler2D	s_mask_4;
uniform	sampler2D	s_mask_5;
uniform	sampler2D	s_mask_6;
uniform	sampler2D	s_mask_7;
uniform	sampler2D	s_mask_8;
uniform	sampler2D	s_mask_9;
uniform	sampler2D	s_mask_10;
uniform float2 		gasmask0;

float4 main(AntiAliasingStruct INStruct):COLOR
{
	//Прямая зависимость от состояния брони в скриптах
	//Пришлось переводить из 10.0 в 1.0
	
	float4 gasmask_distortion;

	if (gasmask0.x >= 1.1)
	{
		gasmask_distortion = tex2D(s_mask_0,INStruct.texCoord0);
	}

	else if (gasmask0.x >= 1.0)
	{
		gasmask_distortion = tex2D(s_mask_1,INStruct.texCoord0);
	}
	
	else if (gasmask0.x >= 0.9)
	{
		gasmask_distortion = tex2D(s_mask_2,INStruct.texCoord0);
	}
	
	else if (gasmask0.x >= 0.8)
	{
		gasmask_distortion = tex2D(s_mask_3,INStruct.texCoord0);
	}
	
	else if (gasmask0.x >= 0.7)
	{
		gasmask_distortion = tex2D(s_mask_4,INStruct.texCoord0);
	}
	
	else if (gasmask0.x >= 0.6)
	{
		gasmask_distortion = tex2D(s_mask_5,INStruct.texCoord0);
	}
	
	else if (gasmask0.x >= 0.5)
	{
		gasmask_distortion = tex2D(s_mask_6,INStruct.texCoord0);
	}
	
	else if (gasmask0.x >= 0.4)
	{
		gasmask_distortion = tex2D(s_mask_7,INStruct.texCoord0);
	}
	
	else if (gasmask0.x >= 0.3)
	{
		gasmask_distortion = tex2D(s_mask_8,INStruct.texCoord0);
	}
	
	else if (gasmask0.x >= 0.2)
	{
		gasmask_distortion = tex2D(s_mask_9,INStruct.texCoord0);
	}
	
	else if (gasmask0.x >= 0.1)
	{	
		gasmask_distortion = tex2D(s_mask_10,INStruct.texCoord0);
	}

	float2 texcoord_offset = (gasmask_distortion.xy-(127.h/255.h))*def_distort;
	float2 texcoord = INStruct.texCoord0 + texcoord_offset * 2.f;
	float3 scene = tex2D(s_image,texcoord);

	return float4(scene,1.f);
}