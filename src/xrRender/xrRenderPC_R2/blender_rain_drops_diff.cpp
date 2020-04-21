#include "stdafx.h"
#pragma hdrstop
#include "Blender_rain_drops_diff.h"

CBlender_rain_drops_diff::CBlender_rain_drops_diff() { description.CLS = 0; }
CBlender_rain_drops_diff::~CBlender_rain_drops_diff() { }

void CBlender_rain_drops_diff::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

	C.r_Pass("null", "rain_drops_diff", FALSE, FALSE, FALSE);
	C.r_Sampler_clf("s_image", r2_RT_generic0);
	C.r_Sampler_clf("s_rain_drops0_diff", "shaders\\rain_drops");
	C.r_End();
}
