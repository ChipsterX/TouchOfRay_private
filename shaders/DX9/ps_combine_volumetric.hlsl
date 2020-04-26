#include "common.h"

//	Igor: used for volumetric light
uniform sampler2D 	s_vollight;
uniform float4 screen_res;

struct         _input      {
        float4	hpos	: POSITION;
#ifdef        USE_VTF
	float4	tc0	: TEXCOORD0;        // tc.xy, tc.w = tonemap scale
#else
	float2	tc0	: TEXCOORD0;        // tc.xy
#endif
};

struct        _out                {
        float4	low	: COLOR0;
        float4	high	: COLOR1;
};

float4 gauss(sampler2D ss, float2 center, float factor)
{   
   float4 col = 0;
   float dx = factor * .5f/screen_res.x;
   float dy = factor * .5f/screen_res.y;      
   col.rgb =   
	(
      1.f * tex2D( ss, center ).rgb +
      1.f * tex2D( ss, center + float2(dx, 0)).rgb +
	  1.f * tex2D( ss, center + float2(dy, 0)).rgb +
      1.f * tex2D( ss, center + float2(-dx, 0)).rgb +
      1.f * tex2D( ss, center + float2(-dx, -dy)).rgb +
      1.f * tex2D( ss, center + float2(dx, -dy)).rgb
	)/6.f;	
	return col;
}

_out         main                ( _input I )
{
        // final tone-mapping
#ifdef        USE_VTF
        float          	tm_scale        = I.tc0.w;	// interpolated from VS
#else
        float         	tm_scale        = tex2D(s_tonemap,I.tc0).x;
#endif
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