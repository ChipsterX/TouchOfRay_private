#include "common.h"

//////////////////////////////////////////////////////////////////////////////////////////
// Pixel
float4 main(p_TL I) : Sv_Target
{
	float4 final = s_base.Sample(smp_base, I.Tex0);
	final.rgb *= L_sun_color.rgb;
	final.rgb = Vibrance(final.rgb, vibrance + 1.h);
#ifdef CGIM
	final *= float4(saturate(dot(-eye_direction, normalize(L_sun_dir_w))).xxx, I.Color.w / 3.0f);
#else
	final.w *= I.Color.w / 3.0f;
#endif
	return final;
};