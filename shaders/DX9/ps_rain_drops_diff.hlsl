#include "common.h"
#include "rain_drops_diff.h"

//////////////////////////////////////////////////////////////////////////////////////////
//pixel
float4 main(AntiAliasingStruct INStruct):COLOR
{
	float4 rain_drops_distortion = tex2D(s_rain_drops0_diff,INStruct.texCoord0);
	float2 texcoord_offset = (rain_drops_distortion.xy-(127.h/255.h))*def_distort;
    float2 texcoord = INStruct.texCoord0 + texcoord_offset * rain_drops_params0_diff.x;
	float3 scene = tex2D(s_image,texcoord);
	return float4(scene,1.f);
}