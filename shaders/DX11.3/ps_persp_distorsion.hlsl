#include "common.h"

struct InTex { SamplerState smpl; Texture2D tex; };
#define SamlpleIn(t, p) t.tex.Sample(t.smpl, p)

Texture2D		 	s_base0;
uniform float4		screen_res;
uniform float4 		c_distparam;	

struct	v2p 
{
	float2 	tc0		: TEXCOORD0;
	float4 	HPos	: SV_Position;	 
};

float4 main (  v2p I ) : SV_Target
{
	float AspectR = screen_res.y / screen_res.x;
	
	InTex tex = { smp_rtlinear, s_base0};
	 
	 
	float GameFov = c_distparam.x; // Vertical FOV in Game
	float TargetFov = tan(radians(c_distparam.y * 0.417)); // correcting to max effect

	// Cjrrect to fov type
	float ResultFov = TargetFov * GameFov;
	// Convert UV to Screen Coordinates 
	float2 SphCoord = I.tc0 * 2.0 - 1.0;
	
	// Aspect Ratio correction
	//SphCoord.y *= AspectR;
	
	//Vertical disrorsion
	float SqrTanFOVq = tan(radians(ResultFov));
	SqrTanFOVq *= SqrTanFOVq;
	SphCoord.y *= (1.0 - SqrTanFOVq*AspectR) / (1.0 - SqrTanFOVq * sqrt(SphCoord.y * SphCoord.y)*AspectR);
	
	//Horisontal disrorsion
	SphCoord.x *= (1.0 - SqrTanFOVq) / (1.0 - SqrTanFOVq * sqrt(SphCoord.x * SphCoord.x));
	
	// Aspect Ratio back to square
	//SphCoord.y /= AspectR;
	
	// Back to UV Coordinates
	SphCoord = SphCoord * 0.5 + 0.5;

	// Sample display image
	float3 Display = SamlpleIn(tex, SphCoord).rgb;

	return float4(Display.r, Display.g, Display.b, 0.0f);
}