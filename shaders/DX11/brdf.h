/*
BRDF Formulas

Credits goes to John Hable.

References:
http://filmicworlds.com/blog/optimizing-ggx-shaders-with-dotlh/

If you want to use this code in your project, just do it but...
don't be a douchebag, and do not remove this header. Thanks.

By LVutner 2020
*/
#ifndef	BRDF_H
#define BRDF_H

#include "common.h"

		/* MAIN CONSTANTS*/
#define PBR_F0 float3(0.04,0.04,0.04)

		/* MATERIAL TABLE*/
#define MAT_SKIN	5.0
#define MTL_LEAVES	6.0
#define MTL_CLOTH	7.0
#define MTL_HAIR	8.0

		/* TEXTURE FUNCTIONS*/
		
//Computes roughness map from albedo
float compute_roughness(float3 albedo)
{
	albedo = albedo - 0.32;
	albedo = pow(albedo*albedo,0.21);
    albedo *= albedo*1.25;

    float lum = 1.-dot(albedo.rgb, LUMINANCE_VECTOR);

    lum *= 0.68;
	
	return lum;
}		

		/* HELPER FUNCTIONS*/
		
//Fresnel with visibility term
float2 GGX_FV(float dotLH, float roughness)
{
	float alpha = roughness*roughness;

	// F
	float F_a, F_b;
	float dotLH5 = pow(1.0f-dotLH,5);
	F_a = 1.0f;
	F_b = dotLH5;

	// V
	float vis;
	float k = alpha/2.0f;
	float k2 = k*k;
	float invK2 = 1.0f-k2;
	vis = rcp(dotLH*dotLH*invK2 + k2);

	return float2(F_a*vis,F_b*vis);
}

//Normal distribution
float GGX_D(float dotNH, float roughness)
{
	float alpha = roughness*roughness;
	float alphaSqr = alpha*alpha;
	float pi = 3.14159f;
	float denom = dotNH * dotNH *(alphaSqr-1.0) + 1.0f;

	float D = alphaSqr/(pi * denom * denom);
	return D;
}

		/* SPECULAR BRDF*/
float GGX(float3 N, float3 V, float3 L, float roughness, float F0)
{
	float3 H = normalize(V+L);

	float dotNL = saturate(dot(N,L));
	float dotLH = saturate(dot(L,H));
	float dotNH = saturate(dot(N,H));

	float D = GGX_D(dotNH,roughness);
	float2 FV_helper = GGX_FV(dotLH,roughness);
	float FV = F0*FV_helper.x + (1.0f-F0)*FV_helper.y;
	float specular = dotNL * D * FV;

	return specular;
}

		/* DIFFUSE BRDF*/
float Lambert(float3 N, float3 L)
{
	return saturate(dot(L,N));
}

		/* SUBSURFACE SCATTERING*/
float SSS(float3 N, float3 V, float3 L)
{
	float distortion = 0.5f; 
	float dotproduct = dot(V, -L + N * distortion);
	dotproduct = saturate(dotproduct);
	
	return dotproduct;
}
#endif