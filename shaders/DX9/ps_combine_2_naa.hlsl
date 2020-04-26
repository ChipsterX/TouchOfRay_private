#include "common.h"
#include "mblur.h"
#include "dof.h"
#include "nightvision.h"

struct 	v2p
{
  float4 tc0: 		TEXCOORD0;	// Center
  float4 tc1: 		TEXCOORD1;	// LT 		 
  float4 tc2: 		TEXCOORD2;	// RB
  float4 tc3: 		TEXCOORD3;	// RT 
  float4 tc4: 		TEXCOORD4;	// LB
  float4 tc5:		TEXCOORD5;	// Left	/ Right	
  float4 tc6:		TEXCOORD6;	// Top  / Bottom 
};

#define VALUE_VIGNETTE -0.5f 			// Область затемнения
float4 vignette;
static const float vignette_on = vignette.x;
float vignette_eff(float2 center)
{
	float dist = distance(center, float2(0.5,0.5));
	dist = smoothstep(2.0, VALUE_VIGNETTE, dist);
	return saturate(dist);	
}

//////////////////////////////////////////////////////////////////////////////////////////
uniform sampler2D 	s_distort;
#define	EPSDEPTH	0.001
//////////////////////////////////////////////////////////////////////////////////////////
// Pixel
float4 	main		( v2p I )	: COLOR
{
#ifdef 	USE_DISTORT
  	float 	depth 	= tex2D 	(s_position, 	I.tc0).z;
	float4 	distort	= tex2D		(s_distort, 	I.tc0)	;
	float2	offset	= (distort.xy-(127.0h/255.0h))*def_distort;  // fix newtral offset
	float2	center	= I.tc0 + offset;
	float 	depth_x	= tex2D 	(s_position, center).z	;
	if ((depth_x+EPSDEPTH)<depth)	center	= I.tc0;	// discard new sample
#else
	float2	center 	= I.tc0		;
#endif
	float3	img	= dof(center);
	float4 	bloom	= tex2D		(s_bloom, center);

			img = 1.0 - (1.0 - img) * (1.0 - bloom.xyz*bloom.w);
			img 	= mblur		(center,tex2D(s_position,I.tc0),img.rgb);	
	
			img = nightvision(img, center);
	
#ifdef 	USE_DISTORT
 	float3	blurred	= bloom*def_hdr	;
			img		= lerp	(img,blurred,distort.z);
#endif

	if (vignette_on == 1)
		img *= vignette_eff(center);

 	return 	float4(img,1.);
}
