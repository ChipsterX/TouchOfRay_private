#include "common.h"

///////////////////////////////////////////////////////////////////
// pixel
float4 main( p_screen I) : COLOR
{
	float mask = tex2D(s_extension, I.tc0).w;
	float3 img = tex2D(s_image, I.tc0);
	float3 ssr = tex2D(s_ssr, I.tc0);
	
	float3 final = lerp(img,ssr,mask);	
	return float4(final,1.);
}
