#include "common.h"

#undef ULTRA_SHADOWS_ON
#undef USE_ULTRA_SHADOWS

#define RAY_PATH	2.0h
#define JITTER_TEXTURE_SIZE	64.0f

#define	JITTER_SUN_SHAFTS

#ifdef	SUN_SHAFTS_QUALITY
	#if SUN_SHAFTS_QUALITY==1
		#define	FILTER_LOW
		#define RAY_SAMPLES	10
	#elif SUN_SHAFTS_QUALITY==2
		#define	FILTER_LOW
		#define RAY_SAMPLES	30
	#elif SUN_SHAFTS_QUALITY==3
		#define	FILTER_LOW	
		#define RAY_SAMPLES	60
	#endif
#endif

#ifndef	FILTER_LOW
#ifdef USE_MINMAX_SM
#define SM_MINMAX
#endif
#endif

#include "shadow.h"

float4	volume_range;	//	x - near plane, y - far plane
float4	sun_shafts_intensity;
uniform	float4	screen_res;

#ifdef MSAA_OPTIMIZATION
float4 	main (v2p_volume  I, uint iSample : SV_SAMPLEINDEX  ) : SV_Target
#else
float4 	main (v2p_volume  I) : SV_Target
#endif
{
	float2	tc  = I.tc.xy/I.tc.w;
	float4	pos2d = I.hpos;
	
	gbuffer_data gbd = gbuffer_load_data( GLD_P(tc, pos2d, ISAMPLE) );

#ifndef	SUN_SHAFTS_QUALITY
	return float4(0,0,0,0);
#else	
	float3	P = gbd.P;
#ifndef	JITTER_SUN_SHAFTS
	float3	direction = P/RAY_SAMPLES;
#else	
	float4	J0 	= jitter0.Sample( smp_jitter, tc*screen_res.x*1.f/JITTER_TEXTURE_SIZE );
	float	coeff = (RAY_SAMPLES - 1*J0.x)/(RAY_SAMPLES*RAY_SAMPLES);		
	float3	direction = P*coeff;

#endif	

	float	depth = P.z;
	float	deltaDepth = direction.z;
	
	float4	current	= mul (m_shadow,float4(P,1));
	float4	delta 	= mul (m_shadow, float4(direction,0));

	float	res = 0;
	float	max_density = sun_shafts_intensity;
	float	density = max_density/RAY_SAMPLES;

	if (depth<0.0001)
		res = max_density;

	[unroll]
	for ( int i=0; i<RAY_SAMPLES; ++i )
	{
		if (depth>0.3)
		{
#ifndef	FILTER_LOW
#ifndef SM_MINMAX
#else
				res += density*shadow_dx10_1_sunshafts(current, pos2d.xy );
#endif
#else	
				res += density*sample_hw_pcf(current, float4(0,0,0,0));
#endif	
		}

		depth -= deltaDepth;
		current -= delta;
	}

#ifndef CGIM
	float fSturation = -Ldynamic_dir.z;
	// Normalize dot product to
	fSturation = 0.5f * fSturation + 0.5f;
	// Map saturation to 0.2..1
	fSturation = 0.80f * fSturation + 0.20f;
	res *= fSturation;
#endif

	return res*Ldynamic_color;
#endif	
}

