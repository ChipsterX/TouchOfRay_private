#pragma once
#include "Common/Common.hpp"

#include "xrEngine/stdafx.h"
#include <d3dx9.h>

#include <d3d11.h>
#include <D3Dx11core.h>
#include <d3d11_1.h>
#include <D3DCompiler.h>

#include "xrRender/xrRender/xrD3DDefs.h"

#define R_GL 0
#define R_R1 1
#define R_R2 2
#define R_R3 3
#define R_R4 4
#define RENDER R_R4

#include "xrParticles/psystem.h"
#include "xrRender/xrRender/HW.h"
#include "xrRender/xrRender/Shader.h"
#include "xrRender/xrRender/R_Backend.h"
#include "xrRender/xrRender/R_Backend_Runtime.h"
#include "xrRender/xrRender/ResourceManager.h"
#include "xrEngine/vis_common.h"
#include "xrEngine/Render.h"
#include "Common/_d3d_extensions.h"
#include "xrEngine/IGame_Level.h"
#include "xrRender/xrRender/blenders/Blender.h"
#include "xrRender/xrRender/blenders/Blender_CLSID.h"
#include "xrRender/xrRender/xrRender_console.h"
#include "r4.h"

inline void jitter(CBlender_Compile& C)
{
    C.r_dx10Texture("jitter0", JITTER(0));
    C.r_dx10Texture("jitter1", JITTER(1));
    C.r_dx10Texture("jitter2", JITTER(2));
    C.r_dx10Texture("jitter3", JITTER(3));
    C.r_dx10Texture("jitter4", JITTER(4));
    C.r_dx10Texture("jitterMipped", r2_jitter_mipped);
    C.r_dx10Sampler("smp_jitter");
}
