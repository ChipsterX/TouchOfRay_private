#include "common.h"

float4 blur_params; // xy - direction, zw - resolution
float4 screen_res;

///////////////////////////////////////////////////////////////////
// pixel
float4 main (p_screen I):SV_Target
{
	const int support = int(2.5 * 1.5); 
	float2 dir = (0,0);

	if(blur_params.y == 1.0)
		dir = float2(0.f, 1.f/screen_res.y);
	else if(blur_params.x == 1.0)
		dir = float2(1.f/screen_res.x, 0.f);
		
	float4 acc = (0.0);
	float norm = 0.0;
	
	[unroll (support)]
	for (int i = -support; i <= support; i++) 
	{
		float coeff = exp(-0.5 * float(i) * float(i) / (2.5  * 2.5 ));
		acc += (s_image.Sample(smp_rtlinear, I.tc0 + float(i) * dir)) * coeff;
		norm += coeff;
	}
	
	acc *= 1.0/norm; 
    return acc;
}
