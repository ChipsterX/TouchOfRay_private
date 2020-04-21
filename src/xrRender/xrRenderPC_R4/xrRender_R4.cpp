#include "stdafx.h"
#include "xrRender/xrRender/dxRenderFactory.h"
#include "xrRender/xrRender/dxUIRender.h"
#include "xrRender/xrRender/dxDebugRender.h"
#include "Include/xrAPI/xrAPI.h"

extern "C"
{
XR_EXPORT void SetupEnv()
{
    GEnv.Render = &RImplementation;
    GEnv.RenderFactory = &RenderFactoryImpl;
    GEnv.DU = &DUImpl;
    GEnv.UIRender = &UIRenderImpl;
#ifdef DEBUG
    GEnv.DRender = &DebugRenderImpl;
#endif
    xrRender_initconsole();
}

XR_EXPORT bool CheckRendererSupport()
{
    return xrRender_test_hw() ? true : false;
}
}
