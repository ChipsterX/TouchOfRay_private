#include "common.h"
//gasmask shader by lvutner
struct		GasmaskStruct
{
	float4		tc0		:		TEXCOORD0;
	float4		tc1		:		TEXCOORD1;
	float4		tc2		:		TEXCOORD2;
	float4		tc3		:		TEXCOORD3;
	float4		tc4		:		TEXCOORD4;
	float4		tc5		:		TEXCOORD5;
	float4		tc6		:		TEXCOORD6;
};

uniform Texture2D s_mask_0;
uniform Texture2D s_mask_1;
uniform Texture2D s_mask_2;
uniform Texture2D s_mask_3;
uniform Texture2D s_mask_4;
uniform Texture2D s_mask_5;
uniform Texture2D s_mask_6;
uniform Texture2D s_mask_7;
uniform Texture2D s_mask_8;
uniform Texture2D s_mask_9;
uniform Texture2D s_mask_10;
uniform float2 	  gasmask0;

#define DUDV_DISTORT float(0.025f)

float4 main(GasmaskStruct I) : SV_Target
{

	float4 final = s_image.Sample(smp_rtlinear, I.tc0);
	float4 mask_tex_dudv;

	if (gasmask0.x >= 1.1)
	{
		mask_tex_dudv = s_mask_0.Sample(smp_rtlinear,I.tc0);
	}

	else if (gasmask0.x >= 1.0)
	{
		mask_tex_dudv = s_mask_1.Sample(smp_rtlinear,I.tc0);
	}
	
	else if (gasmask0.x >= 0.9)
	{
		mask_tex_dudv = s_mask_2.Sample(smp_rtlinear,I.tc0);
	}
	
	else if (gasmask0.x >= 0.8)
	{
		mask_tex_dudv = s_mask_3.Sample(smp_rtlinear,I.tc0);
	}
	
	else if (gasmask0.x >= 0.7)
	{
		mask_tex_dudv = s_mask_4.Sample(smp_rtlinear,I.tc0);
	}
	
	else if (gasmask0.x >= 0.6)
	{
		mask_tex_dudv = s_mask_5.Sample(smp_rtlinear,I.tc0);
	}
	
	else if (gasmask0.x >= 0.5)
	{
		mask_tex_dudv = s_mask_6.Sample(smp_rtlinear,I.tc0);
	}
	
	else if (gasmask0.x >= 0.4)
	{
		mask_tex_dudv = s_mask_7.Sample(smp_rtlinear,I.tc0);
	}
	
	else if (gasmask0.x >= 0.3)
	{
		mask_tex_dudv = s_mask_8.Sample(smp_rtlinear,I.tc0);
	}
	
	else if (gasmask0.x >= 0.2)
	{
		mask_tex_dudv = s_mask_9.Sample(smp_rtlinear,I.tc0);
	}
	
	else if (gasmask0.x >= 0.1)
	{	
		mask_tex_dudv = s_mask_10.Sample(smp_rtlinear,I.tc0);
	}

	float2 mask_offset = (mask_tex_dudv.xy - (127.0h/255.0h)) * DUDV_DISTORT;
	float2 mask_coord = I.tc0 + mask_offset * 2.f;

	if (gasmask0.x >= 0.1)
	{		
		float3 dudv_combined = s_image.Sample(smp_rtlinear,mask_coord);
		final = float4(dudv_combined,1.f);
	}	
	
	return final;
} 
