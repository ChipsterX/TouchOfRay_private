#include "common.h"

//////////////////////////////////////////////////////////////////////////////////////////
// Pixel
float4 main(p_TL I) : Sv_Target
{
#ifdef CGIM
	float3 color = Vibrance(I.Color.rgb, vibrance + 1.h);
	float vignette_mask = saturate(pow(1.f - distance(I.Tex0, 0.5f), 6.0f) * 1.15f);
	return float4(color * 1.1f, I.Color.w * 10.f) * vignette_mask;
#else
	return s_base.Sample(smp_base, I.Tex0) * float4(L_sun_color.rgb, I.Color.w * 1.4f);
#endif
};