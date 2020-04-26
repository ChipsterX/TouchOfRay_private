#ifndef	MBLUR_H
#define MBLUR_H

#ifndef 	USE_MBLUR
float3 	mblur	(float2 UV, float3 pos, float3 color)	{ return color; }
#else
#include "common.h"

uniform float4x4	m_current;
uniform float4x4	m_previous;
uniform float2 		m_blur;		// scale_x / 12, scale_y / 12

#define MBLUR_SAMPLES 	float(12)
#define MBLUR_CLAMP	float(0.001)

float3 	mblur	(float2 UV, float3 pos, float3 color)	
{	
	pos.z += 1000000.h*saturate(0.001 - pos.z);
	
	float4 	pos4		= float4	(pos,0.01f);

	float4 	p_current	= mul	(m_current,	pos4);
	float4 	p_previous 	= mul	(m_previous, pos4);
	float2 	p_velocity 	= m_blur * ( (p_current.xy/p_current.w)-(p_previous.xy/p_previous.w) );
		p_velocity	= clamp	(p_velocity,-MBLUR_CLAMP,+MBLUR_CLAMP);

    float dither = saturate(frac(sin(dot(UV, float2(12.9898, 78.223) * 1.0)) * 43758.5453)) * 0.5;

    int samples = 0.0f;
	
	if(!isSecondVPActive())
	{
		color = 0.0; //clean
		for (int i = -2; i <= 2; ++i)
		{
			float2 coord = UV + p_velocity * (float(i + dither) / 2.0);

			if (coord.x > 0.0f && coord.x < 1.0f && coord.y > 0.0f && coord.y < 1.0f)
			{
				color += tex2D(s_image, coord).rgb;
				samples += 1;
			}
		}
		return color / samples;
	}
	else
	{
		return color;
	}
}
#endif

#endif