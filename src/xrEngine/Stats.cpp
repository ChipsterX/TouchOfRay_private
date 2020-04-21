#include "stdafx.h"
#include "GameFont.h"
#pragma hdrstop

#include "xrCDB/ISpatial.h"
#include "IGame_Persistent.h"
#include "IGame_Level.h"
#include "Render.h"
#include "xr_object.h"

#include "Include/xrRender/DrawUtils.h"
#include "xr_input.h"
#include "xrCore/cdecl_cast.hpp"
#include "xrPhysics/IPHWorld.h"
#include "PerformanceAlert.hpp"
#include "device.h"

int g_ErrorLineCount = 15;
Flags32 g_stats_flags = {0};

class optimizer
{
    float average_;
    BOOL enabled_;

public:
    optimizer()
    {
        average_ = 30.f;
        // because Engine is not exist
        enabled_ = FALSE;
    }

    BOOL enabled() { return enabled_; }
    void enable()
    {
        if (!enabled_)
        {
            Engine.External.tune_resume();
            enabled_ = TRUE;
        }
    }
    void disable()
    {
        if (enabled_)
        {
            Engine.External.tune_pause();
            enabled_ = FALSE;
        }
    }
    void update(float value)
    {
        if (value < average_ * 0.7f)
        {
            // 25% deviation
            enable();
        }
        else
        {
            disable();
        };
        average_ = 0.99f * average_ + 0.01f * value;
    };
};
static optimizer vtune;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
BOOL g_bDisableRedText = FALSE;
CStats::CStats()
{
    statsFont = nullptr;
    fMem_calls = 0;
    Device.seqRender.Add(this, REG_PRIORITY_LOW - 1000);
}

CStats::~CStats()
{
    Device.seqRender.Remove(this);
    xr_delete(statsFont);
}

static void DumpSpatialStatistics(IGameFont& font, IPerformanceAlert* alert, ISpatial_DB& db, float engineTotal) {}

void CStats::Show()
{
    float memCalls = float(Memory.stat_calls);
    if (memCalls > fMem_calls)
        fMem_calls = memCalls;
    else
        fMem_calls = 0.9f * fMem_calls + 0.1f * memCalls;
    Memory.stat_calls = 0;
    if (GEnv.isDedicatedServer)
        return;
    auto& font = *statsFont;
    auto engineTotal = Device.GetStats().EngineTotal.result;
    PerformanceAlert alertInstance(font.GetHeight(), {300, 300});
    auto alertPtr = g_bDisableRedText ? nullptr : &alertInstance;
    if (vtune.enabled())
    {
        float sz = font.GetHeight();
        font.SetHeightI(0.02f);
        font.SetColor(0xFFFF0000);
        font.OutSet(Device.dwWidth / 2.0f + (font.SizeOf_("--= tune =--") / 2.0f), Device.dwHeight / 2.0f);
        font.OutNext("--= tune =--");
        font.SetHeight(sz);
    }
    // Show them
    if (psDeviceFlags.test(rsStatistic))
    {
        font.SetColor(0xFFFFFFFF);
        font.OutSet(0, 0);

        Device.DumpStatistics(font, alertPtr);
        font.OutNext("Memory:       %2.2f", fMem_calls);
        if (g_pGameLevel)
            g_pGameLevel->DumpStatistics(font, alertPtr);
        Engine.Sheduler.DumpStatistics(font, alertPtr);
        Engine.Scheduler.DumpStatistics(font, alertPtr);
        g_pGamePersistent->DumpStatistics(font, alertPtr);
        DumpSpatialStatistics(font, alertPtr, *g_SpatialSpace, engineTotal);
        DumpSpatialStatistics(font, alertPtr, *g_SpatialSpacePhysic, engineTotal);
        if (physics_world())
            physics_world()->DumpStatistics(font, alertPtr);
        font.OutSet(200, 0);
        GEnv.Render->DumpStatistics(font, alertPtr);
        font.OutSkip();
        GEnv.Sound->DumpStatistics(font, alertPtr);
        font.OutSkip();
        pInput->DumpStatistics(font, alertPtr);
        font.OutSkip();
        font.OutNext("QPC: %u", CPU::qpc_counter);
        CPU::qpc_counter = 0;
    }
    if (psDeviceFlags.test(rsCameraPos))
    {
        float refHeight = font.GetHeight();
        font.SetHeightI(0.02f);
        font.SetColor(0xffffffff);
        font.Out(10, 600, "CAMERA POSITION:  [%3.2f,%3.2f,%3.2f]", VPUSH(Device.vCameraPosition));
        font.SetHeight(refHeight);
    }

    font.OnRender();

    if (psDeviceFlags.test(rsShowFPS))
    {
        const auto fps = u32(Device.GetStats().fFPS);
        fpsFont->Out(Device.dwWidth - 40, 5, "%3d", fps);
        fpsFont->OnRender();
    }
}

void CStats::OnDeviceCreate()
{
    g_bDisableRedText = !!strstr(Core.Params, "-xclsx");

    statsFont = new CGameFont("stat_font", CGameFont::fsDeviceIndependent);

    fpsFont = new CGameFont("hud_font_di", CGameFont::fsDeviceIndependent);
    fpsFont->SetHeightI(0.025f);
    fpsFont->SetColor(color_rgba(250, 250, 15, 180));
}

void CStats::OnDeviceDestroy()
{
//    GEnv.Render->Reset();
    SetLogCB(nullptr);
    xr_delete(statsFont);
    xr_delete(fpsFont);
}

void CStats::FilteredLog(const char* s)
{
    if (s && s[0] == '!' && s[1] == ' ')
        errors.push_back(shared_str(s));
}

void CStats::OnRender() {}
