#include "common.h"
#include "ssr.h"
#include "rain.h"

struct   vf
{
	float2	tbase	: TEXCOORD0;	// base
	float4	tnorm	: TEXCOORD1;	// nm0
	float3	P_world	: TEXCOORD2;	// nm1
	float3	M1		: TEXCOORD3;
	float3	M2		: TEXCOORD4;
	float3	M3		: TEXCOORD5;
	float3	v2point	: TEXCOORD6;
#ifdef	USE_SOFT_WATER
#ifdef	NEED_SOFT_WATER
	float4	tctexgen: TEXCOORD7;
#endif	
#endif
	float4	c0		: COLOR0;
	float	fog		: FOG;
	float4	hpos	: SV_Position;
};

Texture2D	s_nmap;
TextureCube	s_env0;
TextureCube	s_env1;
Texture3D	s_water;
Texture2D	s_leaves;

#if defined(USE_SOFT_WATER) && defined(NEED_SOFT_WATER)
float3	water_intensity;
#endif	

////////////////////////////////////////////////////////////////////////////////
// Pixel
float4 main( vf I, float4 pos2d : SV_Position ) : SV_Target
{
	float4	base= s_base.Sample( smp_base, I.tbase);
	float3	n0	= s_nmap.Sample( smp_base, I.tnorm.xy);
	float3	n1	= s_nmap.Sample( smp_base, I.tnorm.zw);
	float3 waterSplash = GetNVNMap( s_water, I.P_world.xz, timers.x);
	float3	Navg	= n0 + n1 - 1;

	float3	Nw	= mul (float3x3(I.M1, I.M2, I.M3), Navg);
			Nw	= normalize (Nw);
			//Nw +=  waterSplash.xyz*YOUR_RAIN_TIMER_INTENSITY;
			
	float3	v2point	= normalize (I.v2point);
	float3	vreflect= reflect(v2point, Nw);
			vreflect.y= vreflect.y*2-1;     // fake remapping

	float3	env0	= s_env0.Sample( smp_rtlinear, vreflect);
	float3	env1	= s_env1.Sample( smp_rtlinear, vreflect);

	float3 env_refl = lerp(env0,env1,L_ambient.w);
	float4 img_refl = ssr(I.P_world, Nw.xyz);

	float	fresnel	= saturate (dot(vreflect,v2point));
	float	power	= pow(fresnel,9);
	float	amount	= 0.15h+0.25h*power;	// 1=full env, 0=no env
	
	float3 c_reflection = (0,0,0);
#ifdef USE_SSR	
	c_reflection = lerp(env_refl.xyz,img_refl.xyz,img_refl.w);
#else
	c_reflection = env_refl;
#endif
	
	float3	final              = lerp(c_reflection,base.rgb,base.a);

			final	*= I.c0*2;

#ifdef	NEED_SOFT_WATER
	float	alpha	= 0.75h+0.25h*power;                        // 1=full env, 0=no env
#ifdef	USE_SOFT_WATER
	float2 PosTc = I.tctexgen.xy/I.tctexgen.z;

	gbuffer_data gbd = gbuffer_load_data( PosTc, pos2d );

	float4 _P = 	float4( gbd.P, gbd.mtl );
	float waterDepth = _P.z-I.tctexgen.z;

	//	water fog
	float  fog_exp_intens = -4.0h;
	float fog	= 1-exp(fog_exp_intens*waterDepth);
	float3 Fc  	= float3( 0.1h, 0.1h, 0.1h) * water_intensity.r;
	final 		= lerp (Fc, final, alpha);

	alpha 		= min(alpha, saturate(waterDepth));
	alpha 		= max (fog, alpha);

	//	Leaves
	half4	leaves	= s_leaves.Sample( smp_base, I.tbase);
			leaves.rgb *= water_intensity.r;
	half	calc_cos = -dot(float3(I.M1.z, I.M2.z, I.M3.z),  normalize(v2point));
	half	calc_depth = saturate(waterDepth*calc_cos);
	half	fLeavesFactor = smoothstep(0.025, 0.05, calc_depth );
			fLeavesFactor *= smoothstep(0.075+SW_FOAM_THICKNESS, 0.05+SW_FOAM_THICKNESS, calc_depth );
	final		= lerp(final, leaves, leaves.a*fLeavesFactor);
	alpha		= lerp(alpha, leaves.a, leaves.a*fLeavesFactor);

	final += specular_phong(v2point,Nw.xyz,L_sun_dir_w, 2.f);

#endif	//	USE_SOFT_WATER
	//	Fogging
	final 		= lerp(fog_color, final, I.fog);
	alpha		*= I.fog*I.fog;
	return  float4   (final, alpha )                ;
#else	//	NEED_SOFT_WATER
	//	Fogging
	final 		= lerp(fog_color, final, I.fog);
	return  float4   (final, I.fog*I.fog )      ;
#endif	//	NEED_SOFT_WATER
}
