/*
	*	This Shader serves as a stub for the departure of artifact models.
	*	Please do not change it, otherwise there will be departures. 
	*	This is a dependency on the brightness of the artifact model
	* 	For DX9	
	* 	OldSerpskiStalker
*/

#include "common.h"

struct vf
{
	float4 hpos	: POSITION;
	float2 tc0	: TEXCOORD0;
	float4 c0	: COLOR0;		
	float  fog	: FOG;
};

vf main_vs_2_0 (v_static v)
{
	vf 		o;

	float3 	N 	= unpack_normal		(v.Nh);
	o.hpos 		= mul				(m_VP, v.P);			
	o.tc0		= unpack_tc_base	(v.tc,v.T.w,v.B.w);		

	float3 	L_rgb 	= v.color.xyz;					
	float3 	L_hemi 	= v_hemi(N)*v.Nh.w;					
	float3 	L_sun 	= v_sun(N)*v.color.w;					
	float3 	L_final	= L_rgb + L_hemi + L_sun + L_ambient;

	o.fog 		= saturate(calc_fogging 		(v.P));	
	o.c0		= float4(L_final, o.fog);

	return o;
}
