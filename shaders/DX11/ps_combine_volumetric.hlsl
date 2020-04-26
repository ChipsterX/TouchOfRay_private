#include "common.h"

//	Igor: used for volumetric light
//#ifndef USE_MSAA
Texture2D 							s_vollight;
//#else
//	#ifndef USE_DX11
//		Texture2DMS<float4,MSAA_SAMPLES> s_vollight;
//	#else
//		Texture2DMS<float4>	s_vollight;
//	#endif
//#endif

uniform float4 screen_res;
struct	_input      
{
	float4	tc0	: TEXCOORD0;	// tc.xy, tc.w = tonemap scale
};

struct	_out
{
        float4	low		: SV_Target0;
        float4	high	: SV_Target1;
};

float4 gauss(Texture2D ss, float2 center, float factor)
{   
   float4 col = 0;
   float dx = factor * .5f/screen_res.x;
   float dy = factor * .5f/screen_res.y;      
   col.rgb =   
	(		
		1.f * ss.Sample( smp_nofilter, center).rgb +
		1.f * ss.Sample( smp_nofilter, center + float2(dx, 0)).rgb +
		1.f * ss.Sample( smp_nofilter, center + float2(dy, 0)).rgb +
		1.f * ss.Sample( smp_nofilter, center + float2(-dx, 0)).rgb +
		1.f * ss.Sample( smp_nofilter, center + float2(-dx, -dy)).rgb +
		1.f * ss.Sample( smp_nofilter, center + float2(dx, -dy)).rgb
	)/6.f;	
	return col;
}


//	TODO: DX10: Use load instead of sample
_out main( _input I )
{
	// final tone-mapping
	float          	tm_scale        = I.tc0.w;	// interpolated from VS
	_out	o;

	float4 volumetric = (0,0,0,0);
	
	for (int i = 1; i < 8; i++)
	{
		volumetric += gauss(s_vollight,I.tc0.xy,i);
	}
	volumetric/=8;

	tonemap(o.low, o.high, volumetric, tm_scale );
	return o;
}