#include "stdafx.h"

#include "blender_dlaa.h"

CBlender_dlaa::CBlender_dlaa() { description.CLS = 0; }
CBlender_dlaa::~CBlender_dlaa() { }

void CBlender_dlaa::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

	C.r_Pass("null", "dlaa_main", FALSE, FALSE, FALSE);

	C.r_Sampler_clf("s_image", "$user$generic0");
	C.r_Sampler_rtf("s_position", "$user$position");
	C.r_Sampler_clf("s_distort", "$user$generic1");

	C.r_End();
} 