#include "common.h"
#include "ssr.h"
uniform float4x4 m_inv_v:register(ps,c3); //view-to-world matrix
uniform samplerCUBE	s_sky0;
uniform samplerCUBE	s_sky1;
///////////////////////////////////////////////////////////////////
// pixel
float4 main( p_screen I) : COLOR
{
	//view
	float3 P_view = tex2D(s_position,I.tc0);
	float3 N_view = tex2D(s_normal,I.tc0);

	//world
	float3 P_world = mul(m_inv_v,float4(P_view,1.f)).xyz;
	float3 N_world = mul(m_inv_v,float4(N_view,0.f)).xyz;
	N_world.y *= 1000;	
	N_world = normalize(N_world);
	
	//reflect
	float3 v2point = normalize(P_world - eye_position);
	float3 vreflect = normalize(reflect(v2point,N_world));

	float3 env0	= texCUBE(s_sky0, vreflect);
	float3 env1	= texCUBE(s_sky1, vreflect);
	
	float3 env_refl = lerp(env0,env1,L_ambient.w);
	float4 img_refl = ssr(P_world, N_world);
	
	float3 final = lerp(env_refl.xyz,img_refl.xyz,img_refl.w);	
	return float4(final,1.);
}
