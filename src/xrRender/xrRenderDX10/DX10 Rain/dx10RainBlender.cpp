#include "stdafx.h"
#include "./dx10RainBlender.h"

void CBlender_rain::Compile(CBlender_Compile& C)
{
    IBlender::Compile(C);

    switch (C.iElement)
    {
    case 0: 
        C.r_Pass("stub_notransform_2uv", "rain_layer", false, TRUE, FALSE, FALSE);
        C.PassSET_ZB(TRUE, FALSE, TRUE); // force inverted Z-Buffer

        C.r_dx10Texture("s_position", r2_RT_P);
        C.r_dx10Texture("s_normal", r2_RT_N);
        C.r_dx10Texture("s_material", r2_material);
        C.r_dx10Texture("s_accumulator", r2_RT_accum);
        C.r_dx10Texture("s_lmap", r2_sunmask);
        C.r_dx10Texture("s_smap", r2_RT_smap_depth);

        C.r_dx10Sampler("smp_nofilter");
        C.r_dx10Sampler("smp_material");
        C.r_dx10Sampler("smp_linear");
        jitter(C);
        C.r_dx10Sampler("smp_smap");

        C.r_dx10Texture("s_water", "water\\water_normal");

        C.r_End();

        break;

    case 1:
        if (ps_r3_dyn_wet_surf_opt)
            C.r_Pass("stub_notransform_2uv", "rain_patch_normal_nomsaa", false, TRUE, FALSE, FALSE);
        else
            C.r_Pass("stub_notransform_2uv", "rain_patch_normal_new_nomsaa", false, TRUE, FALSE, FALSE);
        C.PassSET_ZB(TRUE, FALSE, TRUE); // force inverted Z-Buffer

        C.r_dx10Texture("s_position", r2_RT_P);
        C.r_dx10Texture("s_normal", r2_RT_N);
        C.r_dx10Texture("s_material", r2_material);
        C.r_dx10Texture("s_lmap", r2_sunmask);
        C.r_dx10Texture("s_smap", r2_RT_smap_depth);

        C.r_dx10Texture("s_diffuse", r2_RT_albedo);

        C.r_dx10Sampler("smp_nofilter");
        C.r_dx10Sampler("smp_material");
        C.r_dx10Sampler("smp_linear");
        C.r_dx10Sampler("smp_base");
        jitter(C);
        C.r_dx10Sampler("smp_smap");

        C.r_dx10Texture("s_water", "water\\water_SBumpVolume");
        C.r_dx10Texture("s_waterFall", "water\\water_flowing_nmap");

        C.r_End();

        break;

    case 2: 
        C.r_Pass("stub_notransform_2uv", "rain_apply_normal_nomsaa", false, TRUE, FALSE, FALSE);
        C.PassSET_ZB(TRUE, FALSE, TRUE); // force inverted Z-Buffer

        C.r_dx10Texture("s_position", r2_RT_P);
        C.r_dx10Texture("s_material", r2_material);

        C.r_dx10Texture("s_lmap", r2_sunmask);
        C.r_dx10Texture("s_smap", r2_RT_smap_depth);

        C.r_dx10Sampler("smp_nofilter");
        C.r_dx10Sampler("smp_material");
        C.r_dx10Sampler("smp_linear");
        jitter(C);
        C.r_dx10Sampler("smp_smap");
		
        C.r_dx10Texture("s_patched_normal", r2_RT_accum);

        //	Normal can be packed into R and G
		C.r_ColorWriteEnable(true, true, false, false);

        C.r_End();

        break;

    case 3: 
        C.r_Pass(
        "stub_notransform_2uv", "rain_apply_gloss_nomsaa", false, TRUE, FALSE, TRUE, D3DBLEND_ONE, D3DBLEND_ONE);
        C.PassSET_ZB(TRUE, FALSE, TRUE); // force inverted Z-Buffer

        C.r_dx10Texture("s_position", r2_RT_P);
        C.r_dx10Texture("s_material", r2_material);
        C.r_dx10Texture("s_lmap", r2_sunmask);
        C.r_dx10Texture("s_smap", r2_RT_smap_depth);

        C.r_dx10Sampler("smp_nofilter");
        C.r_dx10Sampler("smp_material");
        C.r_dx10Sampler("smp_linear");
        jitter(C);
        C.r_dx10Sampler("smp_smap");

        C.r_dx10Texture("s_patched_normal", r2_RT_accum);

        C.RS.SetRS(D3DRS_SRCBLEND, D3DBLEND_ZERO);
        C.RS.SetRS(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);

        C.r_End();

        break;
    }
}

void CBlender_rain_msaa::SetDefine(LPCSTR Name, LPCSTR Definition)
{
    this->Name = Name;
    this->Definition = Definition;
}

void CBlender_rain_msaa::Compile(CBlender_Compile& C)
{
    IBlender::Compile(C);

    if (Name)
        GEnv.Render->m_MSAASample = atoi(Definition);
    else
        GEnv.Render->m_MSAASample = -1;

    switch (C.iElement)
    {
    case 0: 
        if (ps_r3_dyn_wet_surf_opt)
            C.r_Pass("stub_notransform_2uv", "rain_patch_normal_msaa", false, TRUE, FALSE, FALSE);
        else
            C.r_Pass("stub_notransform_2uv", "rain_patch_normal_new_msaa", false, TRUE, FALSE, FALSE);
        C.PassSET_ZB(TRUE, FALSE, TRUE); // force inverted Z-Buffer

        C.r_dx10Texture("s_position", r2_RT_P);
        C.r_dx10Texture("s_normal", r2_RT_N);
        C.r_dx10Texture("s_material", r2_material);

        C.r_dx10Texture("s_lmap", r2_sunmask);
        C.r_dx10Texture("s_smap", r2_RT_smap_depth);

        C.r_dx10Texture("s_diffuse", r2_RT_albedo);

        C.r_dx10Sampler("smp_nofilter");
        C.r_dx10Sampler("smp_material");
        C.r_dx10Sampler("smp_linear");
        C.r_dx10Sampler("smp_base");
        jitter(C);
        C.r_dx10Sampler("smp_smap");

        C.r_dx10Texture("s_water", "water\\water_SBumpVolume");
        C.r_dx10Texture("s_waterFall", "water\\water_flowing_nmap");

        C.r_End();

        break;

    case 1: 
        C.r_Pass("stub_notransform_2uv", "rain_apply_normal_msaa", false, TRUE, FALSE, FALSE);
        C.PassSET_ZB(TRUE, FALSE, TRUE); // force inverted Z-Buffer

        C.r_dx10Texture("s_position", r2_RT_P);
        C.r_dx10Texture("s_material", r2_material);

        C.r_dx10Texture("s_lmap", r2_sunmask);
        C.r_dx10Texture("s_smap", r2_RT_smap_depth);

        C.r_dx10Sampler("smp_nofilter");
        C.r_dx10Sampler("smp_material");
        C.r_dx10Sampler("smp_linear");
        jitter(C);
        C.r_dx10Sampler("smp_smap");

        C.r_dx10Texture("s_patched_normal", r2_RT_accum);

        //	Normal can be packed into R and G
		C.r_ColorWriteEnable(true, true, false, false);
     
		C.r_End();

        break;

    case 2: 
        C.r_Pass("stub_notransform_2uv", "rain_apply_gloss_msaa", false, TRUE, FALSE, TRUE, D3DBLEND_ONE, D3DBLEND_ONE);
        C.PassSET_ZB(TRUE, FALSE, TRUE); // force inverted Z-Buffer

        C.r_dx10Texture("s_position", r2_RT_P);
      
        C.r_dx10Texture("s_material", r2_material);

        C.r_dx10Texture("s_lmap", r2_sunmask);
        C.r_dx10Texture("s_smap", r2_RT_smap_depth);

        C.r_dx10Sampler("smp_nofilter");
        C.r_dx10Sampler("smp_material");
        C.r_dx10Sampler("smp_linear");
        jitter(C);
        C.r_dx10Sampler("smp_smap");

        C.r_dx10Texture("s_patched_normal", r2_RT_accum);

        C.RS.SetRS(D3DRS_SRCBLEND, D3DBLEND_ZERO);
        C.RS.SetRS(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);

        C.r_End();

        break;
    }
    GEnv.Render->m_MSAASample = -1;
}
