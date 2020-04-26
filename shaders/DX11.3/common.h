#ifndef        COMMON_H
#define        COMMON_H

#include "shared\common.h"

#include "_common_defines.h"
#include "common_policies.h"
#include "common_iostructs.h"
#include "common_samplers.h"
#include "common_cbuffers.h"
#include "common_functions.h"
#include "ogse_functions.h"
#define L_RANGE		1.0f
#define L_BRIGHT	1.0f
// #define USE_SUPER_SPECULAR

#define xmaterial float(L_material.w)

//sweetfx
uniform float3		vibrance;
#define FXPS technique _render{pass _code{PixelShader=compile ps_3_0 main();}}
#define FXVS technique _render{pass _code{VertexShader=compile vs_3_0 main();}}

#endif
