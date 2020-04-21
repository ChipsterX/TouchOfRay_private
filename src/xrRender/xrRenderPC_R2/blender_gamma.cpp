#include "stdafx.h"
#include "Blender_gamma.h"

CBlender_gamma::CBlender_gamma() { description.CLS = 0; }
CBlender_gamma::~CBlender_gamma() { }

void CBlender_gamma::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

	C.r_Pass("null", "render_gamma", FALSE, FALSE, FALSE);
	C.r_Sampler_clf("s_image", r2_RT_generic0);
	
	C.r_Sampler_clf("s_gamma", "blender\\gamma");

	C.r_End();
}
