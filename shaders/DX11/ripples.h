/*
Water ripples

Credits goes to Zavie and Dave_Hoskins.

References:
https://www.shadertoy.com/view/ldfyzl

If you want to use this code in your project, just do it but...
don't be a douchebag, and do not remove this header. Thanks.

By LVutner 2020


Original header:

A quick experiment with rain drop ripples.

This effect was written for and used in the launch scene of the
64kB intro "H - Immersion", by Ctrl-Alt-Test.

 > http://www.ctrl-alt-test.fr/productions/h-immersion/
 > https://www.youtube.com/watch?v=27PN1SsXbjM

-- 
Zavie / Ctrl-Alt-Test

*/

#ifndef RIPPLES_H
#define RIPPLES_H
#include "common.h"

float hash12(float2 p)
{
	float3 p3  = frac(float3(p.xyx) * .1031);
    p3 += dot(p3, p3.yzx + 19.19);
    return frac((p3.x + p3.y) * p3.z);
}

float2 hash22(float2 p)
{
	float3 p3 = frac(float3(p.xyx) * float3(.1031, .1030, .0973));
    p3 += dot(p3, p3.yzx+19.19);
    return frac((p3.xx+p3.yz)*p3.zy);
}

float ripples(float2 tc, float size)
{
	float2 uv = tc * size;
    float2 p0 = floor(uv);

    float2 circles;
    for (int j = -2; j <= 2; ++j)
    {
        for (int i = -2; i <= 2; ++i)
        {
			float2 pi = p0 + float2(i, j);
            float2 hsh = hash22(pi);
            float2 p = pi + hash22(hsh);

            float t = frac(0.35f*timers.x + hash12(hsh));
            float2 v = p - uv;
            float d = length(v) - (float(2.f) + 1.)*t;

            float h = 1e-3;

			//1st pass
            float d1 = d - h;
            float p1 = sin(31.*d1) * smoothstep(-0.6, -0.3, d1) * smoothstep(0., -0.3, d1);
			//sec pass
            float d2 = d + h;
            float p2 = sin(31.*d2) * smoothstep(-0.6, -0.3, d2) * smoothstep(0., -0.3, d2);
            circles += 0.5 * normalize(v) * ((p2 - p1) / (2. * h) * (1. - t) * (1. - t));
        }
    }
    circles /= float((2*3+1)*(2*2+1));

    float intensity = lerp(0.025f, 0.15, smoothstep(0.1, 0.6, frac(0.05 + 0.05)*2.-1.));
    float3 n = float3(circles, sqrt(1. - dot(circles, circles)));
	
	float final = 10.*pow(clamp(dot(n, normalize(float3(1., 0.7, 0.5))), 0., 1.), 6.);
	return final;
}


#endif