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
    case 0: //SSR generation
        C.r_Pass("stub_screen_space", "ssr_generation", false, FALSE, FALSE);
        C.r_dx10Texture("s_position", r2_RT_P);
        C.r_dx10Texture("s_normal", r2_RT_N);
        C.r_dx10Texture("s_diffuse", r2_RT_albedo);
        C.r_dx10Texture("s_image", r2_RT_generic0);		

        C.r_dx10Texture("s_sky0", r2_T_sky0);
        C.r_dx10Texture("s_sky1", r2_T_sky1);

        C.r_dx10Sampler("smp_nofilter");
        C.r_dx10Sampler("smp_rtlinear");
        C.r_End();
        break;
    case 1: //Horizontal blur
        C.r_Pass("stub_screen_space", "ssr_blur", false, FALSE, FALSE);
        C.r_dx10Texture("s_image", r2_RT_ssr);

        C.r_dx10Sampler("smp_nofilter");
        C.r_dx10Sampler("smp_rtlinear");
        C.r_End();
        break;
    case 2: //Vertical blur
        C.r_Pass("stub_screen_space", "ssr_blur", false, FALSE, FALSE);
        C.r_dx10Texture("s_image", r2_RT_ssr_blur);

        C.r_dx10Sampler("smp_nofilter");
        C.r_dx10Sampler("smp_rtlinear");
        C.r_End();
        break;
    case 3: //Combine
        C.r_Pass("stub_screen_space", "ssr_combine", false, FALSE, FALSE);
        C.r_dx10Texture("s_ssr", r2_RT_ssr);
		C.r_dx10Texture("s_extension", r2_RT_E);
        C.r_dx10Texture("s_image", r2_RT_generic0);		

        C.r_dx10Sampler("smp_nofilter");
        C.r_dx10Sampler("smp_rtlinear");
        C.r_End();
        break;
    }
}
