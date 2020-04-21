#include "stdafx.h"
#pragma hdrstop
#include "Blender_gasmask.h"

CBlender_gasmask::CBlender_gasmask() { description.CLS = 0; }
CBlender_gasmask::~CBlender_gasmask() { }

void CBlender_gasmask::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

	C.r_Pass("null", "r2_gasmask", FALSE, FALSE, FALSE);
	C.r_Sampler_clf("s_image", r2_RT_generic0);
	
	C.r_Sampler_clf("s_mask_0", "shaders\\gasmasks\\mask_nm_0");

	C.r_Sampler_clf("s_mask_1", "shaders\\gasmasks\\mask_nm_1");
	C.r_Sampler_clf("s_mask_2", "shaders\\gasmasks\\mask_nm_2");
	C.r_Sampler_clf("s_mask_3", "shaders\\gasmasks\\mask_nm_3");
	C.r_Sampler_clf("s_mask_4", "shaders\\gasmasks\\mask_nm_4");
	C.r_Sampler_clf("s_mask_5", "shaders\\gasmasks\\mask_nm_5");
	
	C.r_Sampler_clf("s_mask_6", "shaders\\gasmasks\\mask_nm_6");
	C.r_Sampler_clf("s_mask_7", "shaders\\gasmasks\\mask_nm_7");
	C.r_Sampler_clf("s_mask_8", "shaders\\gasmasks\\mask_nm_8");
	C.r_Sampler_clf("s_mask_9", "shaders\\gasmasks\\mask_nm_9");
	C.r_Sampler_clf("s_mask_10", "shaders\\gasmasks\\mask_nm_10");
	
	C.r_End();
}
