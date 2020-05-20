#ifndef	LMODEL_H
#define LMODEL_H

#include "common.h"
#include "brdf.h"

float4 plight_infinity( float m, float3 pnt, float3 normal, float4 color, float3 light_direction )
{
  	float3 N			= normal;							// normal 
  	float3 V 		= -normalize	(pnt);					// vector2eye
  	float3 L 		= -light_direction;						// vector2light
  	float3 H			= normalize	(L+V);						// float-angle-vector 
	
	//Prepare textures
	float3 albedo_tex = color.rgb;
	float specular_tex = color.a;
	float roughness_tex = compute_roughness(albedo_tex.xyz);
	
	//Compute F0
	float3 F0 = lerp(PBR_F0, albedo_tex, specular_tex);
	
	//Lambert diffuse
	float diffuse = Lambert(N,L);
	//GGX speculars
	float specular = GGX(N,V,L,roughness_tex,F0);
	//Simple subsurface scattering
	float subsurface = SSS(N,V,L);
	
	//Combined light
	float4 light = float4(diffuse.xxx, specular);
	return light;
}

float4 plight_local( float m, float3 pnt, float3 normal, float4 color, float3 light_position, float light_range_rsq, out float rsqr )
{
  	float3 N		= normal;							// normal 
  	float3 L2P 	= pnt-light_position;                         		// light2point 
  	float3 V 	= -normalize	(pnt);					// vector2eye
  	float3 L 	= -normalize	((float3)L2P);					// vector2light
  	float3 H		= normalize	(L+V);						// float-angle-vector
		rsqr	= dot		(L2P,L2P);					// distance 2 light (squared)
  	float  att 	= saturate	(1 - rsqr*light_range_rsq);			// q-linear attenuate

	//Prepare textures
	float3 albedo_tex = color.rgb;
	float specular_tex = color.a;
	float roughness_tex = compute_roughness(albedo_tex.xyz);
	
	//Compute F0
	float3 F0 = lerp(PBR_F0, albedo_tex, specular_tex);
	
	//Lambert diffuse
	float diffuse = Lambert(N,L);
	//GGX speculars
	float specular = GGX(N,V,L,roughness_tex,F0);
	//Simple subsurface scattering
	float subsurface = SSS(N,V,L);
	
	//Combined light
	float4 light = float4(diffuse.xxx, specular);

  	return att*light;
}

//	TODO: DX10: Remove path without blending
float4 blendp( float4 value, float4 tcp)
{
	return 	value;
}

float4 blend( float4 value, float2 tc)
{
	return 	value;
}

#endif