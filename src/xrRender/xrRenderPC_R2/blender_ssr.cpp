#include "stdafx.h"
#pragma hdrstop

#include "blender_ssr.h"

CBlender_ssr::CBlender_ssr() { description.CLS = 0; }
CBlender_ssr::~CBlender_ssr() { }

void CBlender_ssr::Compile(CBlender_Compile& C)
{
    IBlender::Compile(C);

    switch (C.iElement)
    {
    case 0:
        C.r_Pass("null", "ssr_generation", false, FALSE, FALSE);
        C.r_Sampler_rtf("s_position", r2_RT_P);
        C.r_Sampler_rtf("s_normal", r2_RT_N);
        C.r_Sampler_rtf("s_diffuse", r2_RT_albedo);
        C.r_Sampler_rtf("s_image", r2_RT_generic0);
		
        C.r_Sampler_clf("s_sky0", r2_T_sky0);
        C.r_Sampler_clf("s_sky1", r2_T_sky1);		
        C.r_End();
        break;
    case 1:
        C.r_Pass("null", "ssr_blur", false, FALSE, FALSE);
        C.r_Sampler_rtf("s_image", r2_RT_ssr);

        C.r_End();
        break;
    case 2:
        C.r_Pass("null", "ssr_blur", false, FALSE, FALSE);
        C.r_Sampler_rtf("s_image", r2_RT_ssr_blur);

        C.r_End();
        break;	
    case 3:
        C.r_Pass("null", "ssr_combine", false, FALSE, FALSE);
        C.r_Sampler_rtf("s_image", r2_RT_generic0);
        C.r_Sampler_rtf("s_ssr", r2_RT_ssr);
        C.r_Sampler_rtf("s_extension", r2_RT_E);
        C.r_End();
        break;
    }
}
