#include "common.h"

///////////////////////////////////////////////////////////////////
// pixel
float4 main( p_screen I, float4 pos2d : SV_Position ) : SV_Target
{
	gbuffer_data gbd = gbuffer_load_data(I.tc0, pos2d);

	float mask = gbd.puddles;
	float3 img = s_image.Sample(smp_rtlinear, I.tc0);
	float3 ssr = s_ssr.Sample(smp_rtlinear, I.tc0);
	
	float3 final = lerp(img,ssr,mask);	
	return float4(final,1.);
}
