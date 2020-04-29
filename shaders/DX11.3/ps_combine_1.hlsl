#include "common.h"

#include "lmodel.h"
#include "hmodel.h"
#include "ps_ssdo.hlsl"
#include "fog.h"

struct	_input
{
	float4	tc0	: TEXCOORD0;	// tc.xy, tc.w = tonemap scale
	float2	tcJ	: TEXCOORD1;	// jitter coords
	float4 pos2d : SV_Position;
};

struct	_out
{
	float4	low		: SV_Target0;
	float4	high	: SV_Target1;
};

//	TODO:	DX10: Replace Sample with Load
#ifndef MSAA_OPTIMIZATION
_out main ( _input I )
#else
_out main ( _input I, uint iSample : SV_SAMPLEINDEX )
#endif
{
	gbuffer_data gbd = gbuffer_load_data( GLD_P(I.tc0, I.pos2d, ISAMPLE) );
	
	// Sample the buffers:
	float4	P = float4( gbd.P, gbd.mtl );	// position.(mtl or sun)
	float4	N = float4( gbd.N, gbd.hemi );		// normal.hemi
	float4	D = float4( gbd.C, gbd.gloss );		// rgb.gloss
#ifndef USE_MSAA
	float4	L = s_accumulator.Sample( smp_nofilter, I.tc0);	// diffuse.specular
#else
	float4	L = s_accumulator.Load( int3( I.pos2d.xy, 0 ), ISAMPLE);	// diffuse.specular
#endif

#ifdef USE_GAMMA_22
	D.rgb = ( D.rgb*D.rgb ); // pow(2.2)
#endif

        // static sun
	float mtl = P.w;

	// hemisphere
	float3 hdiffuse, hspecular;
	hmodel	(hdiffuse, hspecular, mtl, N.w, D.w, P.xyz, N.xyz);

	float occ;
	
#if USE_AO_MODE == 1
	occ 	   	     = calc_ssdo(P, N, I.tc0);
	hdiffuse  		*= occ;
	hspecular 		*= occ;
#elif USE_AO_MODE == 2 // HBAO

#elif USE_AO_MODE == 3  // HDAO
#ifdef USE_DX11

#endif
#endif

#ifdef CGIM
	float ao = clamp(N.w, 0.05, 1.0);
	ao *= 10.0f / 2.1f;
	occ *= lerp(1.h, pow(saturate(ao), 0.35), 0.75f);
#endif

	half4         light       = half4         (L.rgb + hdiffuse, L.w)        ;
	half4         C           = D*light       ;                             // rgb.gloss * light(diffuse.specular)
	half3         spec        = C.www * L.rgb + hspecular;      // replicated specular

	half3       color     = C.rgb + spec     ;

////////////////////////////////////////////////////////////////////////////////
		
		float4 lin_fog = linear_fog(P.xyz);
		float4 grnd_fog = ground_fog(P.xyz);
		
		color = lerp(color,grnd_fog.xyz,grnd_fog.w);	
		color = lerp(color,lin_fog.xyz,lin_fog.w);
		
        // here should be distance fog
        float3        	pos        		= P.xyz;
        float         	distance		= length		(pos);
        float         	fog				= saturate		(distance*fog_params.w + fog_params.x); //
                      	color			= lerp     		(color,fog_color,fog);        			//
        half        	skyblend		= saturate		(fog*fog);

        // final tone-mapping
#ifdef        USE_VTF
        half          	tm_scale        = I.tc0.w;                // interpolated from VS
#else
        half         	tm_scale        = tex2D        (s_tonemap,I.tc0).x;
#endif

        _out        	o;
        tonemap        	(o.low, o.high, color, tm_scale )	;
                        o.low.a         = skyblend	;
						o.high.a		= skyblend	;


		return        	o;
}
