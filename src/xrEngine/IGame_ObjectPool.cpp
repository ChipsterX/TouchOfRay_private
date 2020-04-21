#include "stdafx.h"
#include "IGame_Level.h"
#include "IGame_Persistent.h"
#include "IGame_ObjectPool.h"
#include "xr_object.h"
#include "xrServerEntities/smart_cast.h"

IGame_ObjectPool::IGame_ObjectPool(void) {}
IGame_ObjectPool::~IGame_ObjectPool(void) { R_ASSERT(m_PrefetchObjects.empty()); }
void IGame_ObjectPool::prefetch()
{
    R_ASSERT(m_PrefetchObjects.empty());

    int p_count = 0;
    GEnv.Render->model_Logging(FALSE);

    string256 section;
    // prefetch objects
    strconcat(sizeof(section), section, "prefetch_objects_", g_pGamePersistent->m_game_params.m_game_type);
    CInifile::Sect const& sect = pSettings->r_section(section);
    for (const auto& item : sect.Data)
    {
        CLASS_ID CLS = pSettings->r_clsid(item.first.c_str(), "class");
        p_count++;
        IGameObject* pObject = smart_cast<IGameObject*>(NEW_INSTANCE(CLS));
        pObject->Load(item.first.c_str());
        VERIFY2(pObject->cNameSect().c_str(), item.first.c_str());
        m_PrefetchObjects.push_back(pObject);
    }

    // out statistic
    GEnv.Render->model_Logging(TRUE);
}

void IGame_ObjectPool::clear()
{
    // Clear POOL
    for (auto& it : m_PrefetchObjects)
        xr_delete(it);
    m_PrefetchObjects.clear();
}

IGameObject* IGame_ObjectPool::create(LPCSTR name)
{
    CLASS_ID CLS = pSettings->r_clsid(name, "class");
    IGameObject* O = smart_cast<IGameObject*>(NEW_INSTANCE(CLS));
    if (!O)
    {
        xrDebug::LogStackTrace("");
        Msg("xrFactory_Create | failed to create %s by clsid %d", name, CLS);
        return nullptr;
    }
    O->cNameSect_set(name);
    O->Load(name);
    O->PostLoad(name); //--#SM+#--
    return O;
}

void IGame_ObjectPool::destroy(IGameObject* O) { xr_delete(O); }
