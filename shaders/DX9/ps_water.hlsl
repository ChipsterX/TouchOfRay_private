#include "common.h"
#include "ssr.h"
#include "atmospheric.h"

struct vf
{
    float4 hpos		: POSITION;
    float2 tbase	: TEXCOORD0;  // base
    float4 tnorm	: TEXCOORD1;  // nm0
    float3 P_world	: TEXCOORD2;  // nm1
    float3  M1		: TEXCOORD3;
    float3  M2		: TEXCOORD4;
    float3  M3		: TEXCOORD5;
    float3  v2point	: TEXCOORD6;
	float4 tctexgen	: TEXCOORD7;
    float4  c0		: COLOR0;
	float  fog		: FOG;
};

uniform sampler2D	s_nmap;
uniform samplerCUBE	s_env0;
uniform samplerCUBE	s_env1;
uniform sampler2D	s_leaves;

#if defined(USE_SOFT_WATER) && defined(NEED_SOFT_WATER)
float3	water_intensity;
#endif	

float4   main( vf I )          : COLOR
{
	float4 base	= tex2D(s_base,I.tbase);
	float3 n0 = tex2D(s_nmap,I.tnorm.xy);
	float3 n1 = tex2D(s_nmap,I.tnorm.zw);
	float3 Navg = n0 + n1 - 1;

	float3 Nw = mul (float3x3(I.M1, I.M2, I.M3), Navg);
	Nw	= normalize (Nw);
	
	float3 v2point	= normalize (I.v2point);
	float3 vreflect= reflect(v2point, Nw);

	float3 env0	= texCUBE(s_env0, vreflect);
	float3 env1	= texCUBE(s_env1, vreflect);

#ifdef USE_PROC_SKY
	float3 env_refl = compute_scattering(vreflect);
#else	
	float3 env_refl = lerp(env0,env1,L_ambient.w);
#endif

	float4 img_refl = ssr(I.P_world, Nw.xyz);

	float fresnel = saturate (dot(vreflect,v2point));
	float power	= pow(fresnel,9);
	float amount = 0.15h+0.25h*power;

	float3 c_reflection = (0,0,0);
#ifdef USE_SSR	
	c_reflection = lerp(env_refl.xyz,img_refl.xyz,img_refl.w);
#else
	c_reflection = env_refl;
#endif
	
	float3 final = lerp(c_reflection,base.rgb,base.a);

	final	*= I.c0*2;

#ifdef	NEED_SOFT_WATER

	float	alpha	= 0.75h+0.25h*power;                        // 1=full env, 0=no env

#ifdef	USE_SOFT_WATER
	//	Igor: additional depth test
	float4 _P = 	tex2Dproj( s_position, I.tctexgen);
	float waterDepth = _P.z-I.tctexgen.z;

	//	water fog
	float  fog_exp_intens = -4.0h;
	float fog	= 1-exp(fog_exp_intens*waterDepth);
	float3 Fc  	= float3( 0.1h, 0.1h, 0.1h) * water_intensity.r;
	final 		= lerp (Fc, final, alpha);
	alpha 		= min(alpha, saturate(waterDepth));
	alpha 		= max (fog, alpha);

	//	Leaves
	float4	leaves	= tex2D(s_leaves, I.tbase);
			leaves.rgb *= water_intensity.r;
	float	calc_cos = -dot(float3(I.M1.z, I.M2.z, I.M3.z),  normalize(v2point));
	float	calc_depth = saturate(waterDepth*calc_cos);
	float	fLeavesFactor = smoothstep(0.025, 0.05, calc_depth );
			fLeavesFactor *= smoothstep(0.1, 0.075, calc_depth );
	final		= lerp(final, leaves, leaves.a*fLeavesFactor);
	alpha		= lerp(alpha, leaves.a, leaves.a*fLeavesFactor);

	final 		= lerp(fog_color, final, I.fog);

	final += specular_phong(v2point,Nw.xyz,L_sun_dir_w, 2.f);

#endif	
	return  float4   (final, alpha*I.fog*I.fog );
#else	
	return  float4   (final, I.fog*I.fog);
#endif	
}
