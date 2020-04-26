#include "common.h"
#include "ogse_config.h"
#include "ogse_functions.h"

#define EPS 0.0001f
uniform float4 screen_res;

float4 main(p_screen I) : COLOR0
{
	float4 depth;
	depth.x = tex2D(s_position,I.tc0.xy + float2(0, 1.0) * screen_res.zw);
	depth.y = tex2D(s_position,I.tc0.xy + float2(1, 0.65) * screen_res.zw);
	depth.z = tex2D(s_position,I.tc0.xy + float2(-1, 0.65) * screen_res.zw);

	float4 sceneDepth;
	sceneDepth.x = normalize_depth(depth.x)*is_not_sky(depth.x);
	sceneDepth.y = normalize_depth(depth.y)*is_not_sky(depth.y);
	sceneDepth.z = normalize_depth(depth.z)*is_not_sky(depth.z);

	sceneDepth.w = (sceneDepth.x + sceneDepth.y + sceneDepth.z) * 0.333;
	
	depth.w = saturate(1 - sceneDepth.w*1000);
	
	float4 Color = float4(depth.w, depth.w, depth.w, sceneDepth.w);	
	return Color;
}