#include "stdafx.h"
#include "IGame_Level.h"

void IGame_Level::LL_CheckTextures()
{
    u32 m_base, c_base, m_lmaps, c_lmaps;
    GEnv.Render->ResourcesGetMemoryUsage(m_base, c_base, m_lmaps, c_lmaps);
    if (Core.ParamFlags.test(Core.verboselog))
    {
        Msg("* t-report - base: %d, %d K", c_base, m_base / 1024);
        Msg("* t-report - lmap: %d, %d K", c_lmaps, m_lmaps / 1024);
    }

    BOOL bError = FALSE;
    if (m_base > 64 * 1024 * 1024 || c_base > 400)
    {
        bError = TRUE;
    }
    if (m_lmaps > 32 * 1024 * 1024 || c_lmaps > 8)
    {
        bError = TRUE;
    }
}
