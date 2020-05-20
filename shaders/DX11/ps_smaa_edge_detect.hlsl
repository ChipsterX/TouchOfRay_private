#include "common.h"

uniform float4 screen_res; // Screen resolution (x-Width,y-Height, zw - 1/resolution)


#if defined(SM_4_1) || defined(SM_5)
	#define SMAA_HLSL_4_1
#else
	#define SMAA_HLSL_4
#endif
#define SMAA_RT_METRICS screen_res.zwxy

/*#if !defined(PP_AA_QUALITY) || (PP_AA_QUALITY <= 1) || (PP_AA_QUALITY > 4)
	#define	SMAA_PRESET_LOW
#elif PP_AA_QUALITY == 2
	#define	SMAA_PRESET_MEDIUM
#elif PP_AA_QUALITY == 3
	#define	SMAA_PRESET_HIGH
#elif PP_AA_QUALITY == 4*/
	#define	SMAA_PRESET_ULTRA
	#define EDGE_DETECT_COLOR
//#endif

#include "smaa.h"

struct _in
{
	float4	pos	: SV_Position;
	float2	tc0 : TEXCOORD0;
};

float4 main(_in I) : SV_Target
{
	// RainbowZerg: offset calculation can be moved to VS or CPU...
	float4 offset[3];
    offset[0] = mad(SMAA_RT_METRICS.xyxy, float4(-1.0f, 0.0f, 0.0f, -1.0f), I.tc0.xyxy);
    offset[1] = mad(SMAA_RT_METRICS.xyxy, float4( 1.0f, 0.0f, 0.0f,  1.0f), I.tc0.xyxy);
    offset[2] = mad(SMAA_RT_METRICS.xyxy, float4(-2.0f, 0.0f, 0.0f, -2.0f), I.tc0.xyxy);

#if defined(EDGE_DETECT_COLOR)
	return float4(SMAAColorEdgeDetectionPS(I.tc0, offset, s_image), 0.0f, 0.0f);
#else
	return float4(SMAALumaEdgeDetectionPS(I.tc0, offset, s_image), 0.0f, 0.0f);
#endif
}