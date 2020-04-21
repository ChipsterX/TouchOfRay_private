#include "stdafx.h"
#pragma hdrstop

#include "blender_fxaa.h"

CBlender_fxaa::CBlender_fxaa() { description.CLS = 0; }
CBlender_fxaa::~CBlender_fxaa() { }

void CBlender_fxaa::Compile(CBlender_Compile& C)
{
    IBlender::Compile(C);

    switch (C.iElement)
    {
    case 0:
        C.r_Pass("fxaa_main", "fxaa_main", false, FALSE, FALSE);
        C.r_dx10Texture("s_base0", r2_RT_generic0);
        C.r_dx10Sampler("smp_rtlinear");
        C.r_End();
        break;
    }
}
