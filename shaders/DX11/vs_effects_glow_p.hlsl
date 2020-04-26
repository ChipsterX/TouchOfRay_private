#include "common.h"
//Fixed glows by skyloader
struct vv
{
	float4	P	: POSITION;
	float2	Tex0	: TEXCOORD0;
	float4	Color	: COLOR; 
};

struct v2p
{
	float2 	Tex0		: TEXCOORD0;
	float4		Color		: COLOR;
	float4		tctexgen	: TEXCOORD1;
	float4		hpos		: SV_Position;
};

uniform float4x4 	mVPTexgen;
//////////////////////////////////////////////////////////////////////////////////////////
// Vertex
v2p main ( vv I )
{
	v2p O;

	O.hpos 		= mul(m_WVP, I.P);		// xform, input in world coords
	O.Tex0		= I.Tex0;
	O.Color		= unpack_D3DCOLOR(I.Color);
	O.tctexgen		= mul( mVPTexgen, I.P);
	O.tctexgen.z	= O.hpos.z;

 	return O;
}