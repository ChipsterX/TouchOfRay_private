#include "common.h"

struct 	v2p
{
 	float2 	tc0: 	TEXCOORD0;	// base
};


//////////////////////////////////////////////////////////////////////////////////////////
// Pixel
float4 	main	( v2p I )	: COLOR
{
	float4 r = tex2D	(s_base,I.tc0);
	r.w = 1 - r.w;
	return r;
}
