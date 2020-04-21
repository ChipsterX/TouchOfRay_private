#pragma once
#include "Common/Common.hpp"

#include "xrEngine/stdafx.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <D3DCompiler.h>

#include "xrRender/xrRender/xrD3DDefs.h"
#include "xrRender/xrRender/HW.h"
#include "xrRender/xrRender/Shader.h"
#include "xrRender/xrRender/R_Backend.h"
#include "xrRender/xrRender/R_Backend_Runtime.h"

#define R_GL 0
#define R_R1 1
#define R_R2 2
#define R_R3 3
#define R_R4 4
#define RENDER R_R2

#include "xrRender/xrRender/ResourceManager.h"
#include "xrEngine/vis_common.h"
#include "xrEngine/Render.h"
#include "Common/_d3d_extensions.h"
#include "xrEngine/IGame_Level.h"
#include "xrRender/xrRender/blenders/Blender.h"
#include "xrRender/xrRender/blenders/Blender_CLSID.h"
#include "xrParticles/psystem.h"
#include "xrRender/xrRender/xrRender_console.h"
#include "r2.h"

inline void jitter(CBlender_Compile& C)
{
    C.r_Sampler("jitter0", JITTER(0), true, D3DTADDRESS_WRAP, D3DTEXF_POINT, D3DTEXF_NONE, D3DTEXF_POINT);
    C.r_Sampler("jitter1", JITTER(1), true, D3DTADDRESS_WRAP, D3DTEXF_POINT, D3DTEXF_NONE, D3DTEXF_POINT);
    C.r_Sampler("jitter2", JITTER(2), true, D3DTADDRESS_WRAP, D3DTEXF_POINT, D3DTEXF_NONE, D3DTEXF_POINT);
    C.r_Sampler("jitter3", JITTER(3), true, D3DTADDRESS_WRAP, D3DTEXF_POINT, D3DTEXF_NONE, D3DTEXF_POINT);
    C.r_Sampler("jitter4", JITTER(4), true, D3DTADDRESS_WRAP, D3DTEXF_POINT, D3DTEXF_NONE, D3DTEXF_POINT);
}
