/*
	*	This Shader serves as a stub for the departure of artifact models.
	*	Please do not change it, otherwise there will be departures. 
	*	This is a dependency on the brightness of the artifact model
	* 	For DX10	
	* 	OldSerpskiStalker
*/

#include "common.h"

struct vf
{
	float2 tc0	: TEXCOORD0;
	float3 c0	: COLOR0;		
	float  fog	: FOG;
	float4 hpos	: SV_Position;
};

vf main (v_static_color v)
{
	vf 		o;

	float3 	N 	= unpack_normal		(v.Nh);
	o.hpos 		= mul				(m_VP, v.P);			
	o.tc0		= unpack_tc_base	(v.tc,v.T.w,v.B.w);		

	float3 	L_rgb 	= v.color.zyx;						
	float3 	L_hemi 	= v_hemi(N)*v.Nh.w;					
	float3 	L_sun 	= v_sun(N)*v.color.w;				
	float3 	L_final	= L_rgb + L_hemi + L_sun + L_ambient;

	o.c0		= L_final;
	o.fog 		= saturate(calc_fogging 		(v.P));			

	return o;
}
