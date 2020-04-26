#include "common.h"

uniform	sampler2D	s_float_depth;

#include "lmodel.h"
#include "hmodel.h"

#include "ps_ssdo.hlsl"
//#include "ps_hbao.hlsl"

#include "fog.h"

struct         _input      {
        float4         hpos        : POSITION        ;
#ifdef        USE_VTF
          float4         tc0                : TEXCOORD0        ;        // tc.xy, tc.w = tonemap scale
#else
          float2         tc0                : TEXCOORD0        ;        // tc.xy
#endif
	float2	tcJ	: TEXCOORD1;	// jitter coords
};

struct        _out                {
        float4        low                : COLOR0        ;
        float4        high        : COLOR1        ;
};

uniform sampler1D         fog_table        ;

_out         main                ( _input I )
{
          // Sample the buffers:
          float4 P        = tex2D         (s_position,      I.tc0);                // position.(mtl or sun)
          float4 N         = tex2D         (s_normal,        I.tc0);                // normal.hemi
          float4 D         = tex2D         (s_diffuse,       I.tc0);                // rgb.gloss
          float4 L         = tex2D         (s_accumulator, 	I.tc0);                // diffuse.specular

#ifdef         USE_GAMMA_22
        D.rgb        =        (D.rgb*D.rgb)        ;        // pow(2.2)
#endif

        // static sun
	float mtl 		= P.w;
#ifdef        USE_R2_STATIC_SUN
	float	sun_occ        	=       P.w*2	;
			mtl 			= 		xmaterial;
            L              +=       Ldynamic_color * sun_occ * plight_infinity	(mtl, P.xyz, N.xyz, Ldynamic_dir);
#endif

        // hemisphere
        float3       hdiffuse,hspecular;
		hmodel		(hdiffuse, hspecular, mtl, N.w, D.w, P.xyz, N.xyz);
		
	float occ;
		
#if USE_AO_MODE == 1 // SSDO
	occ 	   		 = calc_ssdo(P, N, I.tc0);
	hdiffuse  		*= occ;
	hspecular 		*= occ;
#elif USE_AO_MODE == 2 // HBAO

#endif

    float4         light       = float4         (L.rgb + hdiffuse, L.w)        ;
    float4         C           = D*light       ;                             // rgb.gloss * light(diffuse.specular)
	float3         spec        = C.www         + hspecular;      // replicated specular


		float3       color     = C.rgb + spec     ;

		float4 lin_fog = linear_fog(P.xyz);
		float4 grnd_fog = ground_fog(P.xyz);
		
		color = lerp(color,grnd_fog.xyz,grnd_fog.w);	
		color = lerp(color,lin_fog.xyz,lin_fog.w);

        // here should be distance fog
        float3        	pos        		= P.xyz;
        float         	distance		= length		(pos);
        float         	fog				= saturate		(distance*fog_params.w + fog_params.x); //
                      	color			= lerp     		(color,fog_color,fog);        			//
        float        	skyblend		= saturate		(fog*fog);

        // final tone-mapping
#ifdef        USE_VTF
        float          	tm_scale        = I.tc0.w;                // interpolated from VS
#else
        float         	tm_scale        = tex2D        (s_tonemap,I.tc0).x;
#endif


        _out        	o;
		
        tonemap        	(o.low, o.high, color, tm_scale )	;
                        o.low.a         = skyblend	;
						o.high.a		= skyblend	;

		return        	o;
}

