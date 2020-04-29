#include "stdafx.h"
#include "xr_3da/resource.h"

#include "Include/editor/ide.hpp"
#include "engine_impl.hpp"
#include "GameFont.h"
#include "PerformanceAlert.hpp"
#include "xrCore/ModuleLookup.hpp"

extern LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


void CRenderDevice::Initialize()
{
    Log("Initializing Engine...");
    TimerGlobal.Start();
    TimerMM.Start();

    // Unless a substitute hWnd has been specified, create a window to render into
    if (!m_hWnd)
    {
        const char* wndclass = "_XRAY_1.6";
        // Register the windows class
        HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(0);
        WNDCLASS wndClass = {0, WndProc, 0, 0, hInstance, LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1)),
            LoadCursor(NULL, IDC_ARROW), (HBRUSH)GetStockObject(BLACK_BRUSH), NULL, wndclass};
        RegisterClass(&wndClass);
        // Set the window's initial style
        m_dwWindowStyle = WS_BORDER | WS_DLGFRAME;
        // Set the window's initial width
        RECT rc;
        SetRect(&rc, 0, 0, psCurrentVidMode[0], psCurrentVidMode[1]);
        AdjustWindowRect(&rc, m_dwWindowStyle, FALSE);
        // Create the render window
        DWORD ExtendedStyle = WS_EX_TOPMOST;
        if (IsDebuggerPresent())
        {
            ExtendedStyle = 0;
        }

        m_hWnd = CreateWindowEx(ExtendedStyle, wndclass, "S.T.A.L.K.E.R.: Back to the roots", m_dwWindowStyle,
            CW_USEDEFAULT, CW_USEDEFAULT, (rc.right - rc.left), (rc.bottom - rc.top), 0L, 0, hInstance, 0L);
    }
    // Save window properties
    m_dwWindowStyle = GetWindowLong(m_hWnd, GWL_STYLE);
    GetWindowRect(m_hWnd, &m_rcWindowBounds);
    GetClientRect(m_hWnd, &m_rcWindowClient);
}

void CRenderDevice::DumpStatistics(IGameFont& font, IPerformanceAlert* alert)
{
    font.OutNext("*** ENGINE:   %2.2fms", stats.EngineTotal.result);
    font.OutNext("FPS/RFPS:     %3.1f/%3.1f", stats.fFPS, stats.fRFPS);
    font.OutNext("TPS:          %2.2f M", stats.fTPS);
    if (alert && stats.fFPS < 30)
        alert->Print(font, "FPS       < 30:   %3.1f", stats.fFPS);
}
