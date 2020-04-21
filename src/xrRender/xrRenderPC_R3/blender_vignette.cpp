#include "stdafx.h"
#pragma hdrstop
#include "Blender_vignette.h"

CBlender_vignette::CBlender_vignette() { description.CLS = 0; }
CBlender_vignette::~CBlender_vignette() { }

void CBlender_vignette::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

	C.r_Pass("stub_notransform_aa_AA", "render_vignette", FALSE, FALSE, FALSE);
	C.r_dx10Texture("s_image", r2_RT_generic0);
	
	C.r_dx10Sampler("smp_rtlinear");
	C.r_End();
}
