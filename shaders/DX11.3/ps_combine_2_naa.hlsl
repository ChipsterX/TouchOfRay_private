//////////////////////////////////////////////////////////////////////////////////////////
#ifndef USE_MSAA
Texture2D 			s_distort;
#define	EPSDEPTH	0.001
#else
	#ifndef USE_DX11
		Texture2DMS<float4,MSAA_SAMPLES> s_distort;
	#else
		Texture2DMS<float4>	s_distort;
	#endif
#define	EPSDEPTH	0.001
#endif
uniform float4 		e_barrier;	// x=norm(.8f), y=depth(.1f), z=clr
uniform float4 		e_weights;	// x=norm, y=depth, z=clr
uniform float4 		e_kernel;	// x=norm, y=depth, z=clr

#include "common.h"
#include "_common_defines.h"
#include "common_policies.h"
#include "common_iostructs.h"
#include "common_samplers.h"
#include "common_cbuffers.h"
#include "common_functions.h"

#include "mblur.h"
#include "dof.h"
#include "nightvision.h"

//////////////////////////////////////////////////////////////////////////////////////////
#define TechniAmount 0.4         						// [0.00, 1.00]
#define TechniPower  4.0  // 2.0       						// [0.00, 8.00]
#define redNegativeAmount   0.88  						// [0.00, 1.00]
#define greenNegativeAmount 0.88 						// [0.00, 1.00]
#define blueNegativeAmount  0.88 						// [0.00, 1.00]
#define cyanfilter float3(0.0, 1.35, 0.65) // float3(0.1, 1.20, 0.75)
#define magentafilter float3(1.0, 0.0, 1.05) 
#define yellowfilter float3(1.7, 1.6, 0.05) // float3(1.7, 1.65, 0.05)
#define redorangefilter float2(1.05, 0.620) 			// RG_
#define greenfilter float2(0.30, 1.0)       			// RG_
#define magentafilter2 magentafilter.rb     			// R_B

float3 TechnicolorPass( float3 colorInput )
{
	float3 tcol = colorInput.rgb;
	
	float2 rednegative_mul   = tcol.rg * (1.0 / (redNegativeAmount * TechniPower));
	float2 greennegative_mul = tcol.rg * (1.0 / (greenNegativeAmount * TechniPower));
	float2 bluenegative_mul  = tcol.rb * (1.0 / (blueNegativeAmount * TechniPower));
	
	float rednegative   = dot( redorangefilter, rednegative_mul );
	float greennegative = dot( greenfilter, greennegative_mul );
	float bluenegative  = dot( magentafilter2, bluenegative_mul );
	
	float3 redoutput   = rednegative.rrr + cyanfilter;
	float3 greenoutput = greennegative.rrr + magentafilter;
	float3 blueoutput  = bluenegative.rrr + yellowfilter;
	
	float3 result = redoutput * greenoutput * blueoutput;
	colorInput.rgb = lerp(tcol, result, TechniAmount);
	return colorInput;
}

struct c2_out
{
	float4	Color : SV_Target;
#ifdef USE_MSAA
	float	Depth : SV_Depth;
#endif
};

c2_out main( v2p_aa_AA I )
{
	c2_out	res;
	res.Color = float4(0,0,0,0);

	int iSample = 0;

	gbuffer_data gbd	= gbuffer_load_data(I.Tex0, I.HPos, iSample );
	
#ifdef 	USE_DISTORT
  	float 	depth 	= gbd.P.z;
#ifndef USE_MSAA
	float4 	distort	= s_distort.Sample(smp_nofilter, I.Tex0);
#else // USE_MSAA
	float4 	distort	= s_distort.Load( int3( I.Tex0 * pos_decompression_params2.xy, 0 ), iSample );
#endif // USE_MSAA
	float2	offset	= (distort.xy-(127.0h/255.0h))*def_distort;  // fix newtral offset
	float2	center	= I.Tex0 + offset;

	gbuffer_data gbdx	= gbuffer_load_data_offset(I.Tex0, center, I.HPos, iSample );

	float 	depth_x	= gbdx.P.z;
	if ((depth_x+EPSDEPTH)<depth)	center	= I.Tex0;	// discard new sample
#else // USE_DISTORT
	float2	center 	= I.Tex0;
#endif

	float3 img = s_image.Sample(smp_rtlinear, center);
	float4 bloom	= s_bloom.Sample( smp_rtlinear, center);
	img = 1.0 - (1.0 - img) * (1.0 - bloom.xyz*bloom.w);
	
	#ifdef USE_MBLUR
		img = mblur( center, ( gbd ).P, img.rgb);
	#endif

	#ifdef USE_DOF
		img	= dof(img,center);
	#endif

	#ifdef 	USE_DISTORT
		float3	blurred	= bloom*def_hdr	;
		img		= lerp	(img,blurred,distort.z);
	#endif

	float4 final = float4(img,1.0);

	final.rgb = nightvision(img, center);

	res.Color = final;

	#ifdef USE_MSAA
		float4 ptp = mul(m_P, float4(gbd.P, 1));
		res.Depth = ptp.w == 0 ? 1 : ptp.z/ptp.w;
	#endif
	
	res.Color.rgb = Vibrance(res.Color.rgb, vibrance+1);
//	res.Color.rgb = TechnicolorPass(res.Color.rgb);

	return res;
}