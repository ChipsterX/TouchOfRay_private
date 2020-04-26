#include "common.h"

//////////////////////////////////////////////////////////////////////////////////////////
// Pixel
float4 main(p_TL I) : SV_Target
{
	float4 res = s_base.Sample(smp_base, I.Tex0) * I.Color;
	res.rgb = Vibrance(res.rgb, vibrance + 1.h);
	return res;
}
