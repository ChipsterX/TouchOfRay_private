#include "stdafx.h"
#include "IGame_Level.h"
#include "IGame_Persistent.h"

#include "xrSheduler.h"
#include "xr_object_list.h"
#include "std_classes.h"

#include "xr_object.h"
#include "xrCore/net_utils.h"

#include "CustomHUD.h"
#include "GameFont.h"
#include "PerformanceAlert.hpp"

class fClassEQ
{
    CLASS_ID cls;

public:
    fClassEQ(CLASS_ID C) : cls(C){};
    inline bool operator()(IGameObject* O) { return cls == O->GetClassId(); }
};

void CObjectList::DumpStatistics(IGameFont& font, IPerformanceAlert* alert)
{
    stats.FrameEnd();
    float engineTotal = Device.GetStats().EngineTotal.result;
    float percentage = 100.0f * stats.Update.result / engineTotal;
    font.OutNext("Objects:      %2.2fms, %2.1f%%", stats.Update.result, percentage);
    font.OutNext("- crow:       %d", stats.Crows);
    font.OutNext("- active:     %d", stats.Active);
    font.OutNext("- total:      %d", stats.Total);
    if (alert && stats.Update.result > 3.0f)
        alert->Print(font, "UpdateCL  > 3ms:  %3.1f", stats.Update.result);
}

CObjectList::CObjectList() : m_owner_thread_id(ThreadUtil::GetCurrThreadId())
{
    statsFrame = u32(-1);
    ZeroMemory(map_NETID, 0xffff * sizeof(IGameObject*));
}

CObjectList::~CObjectList()
{
    R_ASSERT(objects_active.empty());
    R_ASSERT(objects_sleeping.empty());
    R_ASSERT(destroy_queue.empty());
}

IGameObject* CObjectList::FindObjectByName(shared_str name)
{
    for (auto& it : objects_active)
        if (it->cName().equal(name))
            return it;

    for (auto& it : objects_sleeping)
        if (it->cName().equal(name))
            return it;

    return nullptr;
}
IGameObject* CObjectList::FindObjectByName(LPCSTR name) { return FindObjectByName(shared_str(name)); }
IGameObject* CObjectList::FindObjectByCLS_ID(CLASS_ID cls)
{
    {
        Objects::iterator O = std::find_if(objects_active.begin(), objects_active.end(), fClassEQ(cls));
        if (O != objects_active.end())
            return *O;
    }
    {
        Objects::iterator O = std::find_if(objects_sleeping.begin(), objects_sleeping.end(), fClassEQ(cls));
        if (O != objects_sleeping.end())
            return *O;
    }

    return NULL;
}

void CObjectList::o_remove(Objects& v, IGameObject* O)
{
    Objects::iterator _i = std::find(v.begin(), v.end(), O);
    VERIFY(_i != v.end());
    v.erase(_i);
}

void CObjectList::o_activate(IGameObject* O)
{
    VERIFY(O && O->processing_enabled());
    o_remove(objects_sleeping, O);
    objects_active.push_back(O);
    O->MakeMeCrow();
}
void CObjectList::o_sleep(IGameObject* O)
{
    VERIFY(O && !O->processing_enabled());
    o_remove(objects_active, O);
    objects_sleeping.push_back(O);
    O->MakeMeCrow();
}

void CObjectList::SingleUpdate(IGameObject* O)
{
    if (Device.dwFrame == O->GetUpdateFrame())
    {
        return;
    }

    if (!O->processing_enabled())
    {
        return;
    }

    if (O->H_Parent())
        SingleUpdate(O->H_Parent());
    stats.Updated++;
    O->SetUpdateFrame(Device.dwFrame);
    O->UpdateCL();

    if (O->H_Parent() && (O->H_Parent()->getDestroy() || O->H_Root()->getDestroy()))
    {
        // Push to destroy-queue if it isn't here already
        Msg("! ERROR: incorrect destroy sequence for object[%d:%s], section[%s], parent[%d:%s]", O->ID(), *O->cName(),
            *O->cNameSect(), O->H_Parent()->ID(), *O->H_Parent()->cName());
    }
}

void CObjectList::clear_crow_vec(Objects& o)
{
    for (auto& it : o)
        it->IAmNotACrowAnyMore();

    o.clear();
}

void CObjectList::Update(bool bForce)
{
    if (statsFrame != Device.dwFrame)
    {
        statsFrame = Device.dwFrame;
        stats.FrameStart();
    }
    if (!Device.Paused() || bForce)
    {
        // Clients
        if (Device.fTimeDelta > EPS_S || bForce)
        {
            // Select Crow-Mode
            stats.Updated = 0;

            Objects& crows = m_crows[0];

            {
                Objects& crows1 = m_crows[1];
                crows.insert(crows.end(), crows1.begin(), crows1.end());
                crows1.clear();
            }

            stats.Crows = crows.size();
            Objects* workload = 0;
            if (!psDeviceFlags.test(rsDisableObjectsAsCrows))
                workload = &crows;
            else
            {
                workload = &objects_active;
                clear_crow_vec(crows);
            }

            stats.Update.Begin();
            stats.Active = objects_active.size();
            stats.Total = objects_active.size() + objects_sleeping.size();

            u32 const objects_count = workload->size();
            IGameObject** objects = (IGameObject**)_alloca(objects_count * sizeof(IGameObject*));
            std::copy(workload->begin(), workload->end(), objects);

            crows.clear();

            IGameObject** b = objects;
            IGameObject** e = objects + objects_count;
            for (IGameObject** i = b; i != e; ++i)
            {
                (*i)->IAmNotACrowAnyMore();
                (*i)->SetCrowUpdateFrame(u32(-1));
            }

            for (IGameObject** i = b; i != e; ++i)
                SingleUpdate(*i);

            //--#SM+#-- PostUpdateCL для всех клиентских объектов [for crowed and non-crowed]
            for (auto& object : objects_active)
                object->PostUpdateCL(false);

            for (auto& object : objects_sleeping)
                object->PostUpdateCL(true);

            stats.Update.End();
        }
    }

    // Destroy
    ProcessDestroyQueue();
}

void CObjectList::ProcessDestroyQueue() {

    // Destroy
    if (!destroy_queue.empty())
    {
        // Info
        for (Objects::iterator oit = objects_active.begin(); oit != objects_active.end(); oit++)
            for (int it = destroy_queue.size() - 1; it >= 0; it--)
                (*oit)->net_Relcase(destroy_queue[it]);

        for (Objects::iterator oit = objects_sleeping.begin(); oit != objects_sleeping.end(); oit++)
            for (int it = destroy_queue.size() - 1; it >= 0; it--)
                (*oit)->net_Relcase(destroy_queue[it]);

        for (int it = destroy_queue.size() - 1; it >= 0; it--)
            GEnv.Sound->object_relcase(destroy_queue[it]);

        RELCASE_CALLBACK_VEC::iterator it = m_relcase_callbacks.begin();
        const RELCASE_CALLBACK_VEC::iterator ite = m_relcase_callbacks.end();
        for (; it != ite; ++it)
        {
            VERIFY(*(*it).m_ID == (it - m_relcase_callbacks.begin()));
            for (auto& dit : destroy_queue)
            {
                (*it).m_Callback(dit);
                g_hud->net_Relcase(dit);
            }
        }

        // Destroy
        for (int it = destroy_queue.size() - 1; it >= 0; it--)
        {
            IGameObject* O = destroy_queue[it];
            O->net_Destroy();
            Destroy(O);
        }
        destroy_queue.clear();
    }
}

void CObjectList::net_Register(IGameObject* O)
{
    R_ASSERT(O);
    R_ASSERT(O->ID() < 0xffff);

    map_NETID[O->ID()] = O;
}

void CObjectList::net_Unregister(IGameObject* O)
{
    if (O->ID() < 0xffff) // demo_spectator can have 0xffff
        map_NETID[O->ID()] = NULL;
}

int g_Dump_Export_Obj = 0;

u32 CObjectList::net_Export(NET_Packet* _Packet, u32 start, u32 max_object_size)
{
    if (g_Dump_Export_Obj)
        Msg("---- net_export --- ");

    NET_Packet& Packet = *_Packet;
    u32 position;
    for (; start < objects_active.size() + objects_sleeping.size(); start++)
    {
        IGameObject* P =
            (start < objects_active.size()) ? objects_active[start] : objects_sleeping[start - objects_active.size()];
        if (P->net_Relevant() && !P->getDestroy())
        {
            Packet.w_u16(u16(P->ID()));
            Packet.w_chunk_open8(position);
            P->net_Export(Packet);

            if (g_Dump_Export_Obj)
            {
                u32 size = u32(Packet.w_tell() - position) - sizeof(u8);
                Msg("* %s : %d", *(P->cNameSect()), size);
            }
            Packet.w_chunk_close8(position);
           
            if (max_object_size >= (NET_PacketSizeLimit - Packet.w_tell()))
                break;
        }
    }
    if (g_Dump_Export_Obj)
        Msg("------------------- ");
    return start + 1;
}

int g_Dump_Import_Obj = 0;

void CObjectList::net_Import(NET_Packet* Packet)
{
    if (g_Dump_Import_Obj)
        Msg("---- net_import --- ");

    while (!Packet->r_eof())
    {
        u16 ID;
        Packet->r_u16(ID);
        u8 size;
        Packet->r_u8(size);
        IGameObject* P = net_Find(ID);
        if (P)
        {
            u32 rsize = Packet->r_tell();

            P->net_Import(*Packet);

            if (g_Dump_Import_Obj)
                Msg("* %s : %d - %d", *(P->cNameSect()), size, Packet->r_tell() - rsize);
        }
        else
            Packet->r_advance(size);
    }

    if (g_Dump_Import_Obj)
        Msg("------------------- ");
}

void CObjectList::Load()
{
    R_ASSERT(objects_active.empty() && destroy_queue.empty() && objects_sleeping.empty());
}

void CObjectList::Unload()
{
    if (objects_sleeping.size() || objects_active.size())
        Msg("! objects-leaked: %d", objects_sleeping.size() + objects_active.size());

    // Destroy objects
    while (objects_sleeping.size())
    {
        IGameObject* O = objects_sleeping.back();
        Msg("! [%x] s[%4d]-[%s]-[%s]", O, O->ID(), *O->cNameSect(), *O->cName());
        O->setDestroy(true);

        O->net_Destroy();
        Destroy(O);
    }
    while (objects_active.size())
    {
        IGameObject* O = objects_active.back();
        Msg("! [%x] a[%4d]-[%s]-[%s]", O, O->ID(), *O->cNameSect(), *O->cName());
        O->setDestroy(true);

        O->net_Destroy();
        Destroy(O);
    }
}

IGameObject* CObjectList::Create(LPCSTR name)
{
    IGameObject* O = g_pGamePersistent->ObjectPool.create(name);
    // Msg("CObjectList::Create [%x]%s", O, name);
    objects_sleeping.push_back(O);
    return O;
}

void CObjectList::Destroy(IGameObject* O)
{
    if (0 == O)
        return;
    net_Unregister(O);

    if (!Device.Paused())
    {
        if (!m_crows[1].empty())
        {
            Msg("assertion !m_crows[1].empty() failed: %d", m_crows[1].size());

            Objects::const_iterator i = m_crows[1].begin();
            Objects::const_iterator const e = m_crows[1].end();
            for (u32 j = 0; i != e; ++i, ++j)
                Msg("%d %s", j, (*i)->cName().c_str());
            VERIFY(Device.Paused() || m_crows[1].empty());
            m_crows[1].clear();
        }
    }
    else
    {
        Objects& crows = m_crows[1];
        Objects::iterator const i = std::find(crows.begin(), crows.end(), O);
        if (i != crows.end())
        {
            crows.erase(i);
            VERIFY(std::find(crows.begin(), crows.end(), O) == crows.end());
        }
    }

    Objects& crows = m_crows[0];
    Objects::iterator _i0 = std::find(crows.begin(), crows.end(), O);
    if (_i0 != crows.end())
    {
        crows.erase(_i0);
        VERIFY(std::find(crows.begin(), crows.end(), O) == crows.end());
    }

    // active/inactive
    Objects::iterator _i = std::find(objects_active.begin(), objects_active.end(), O);
    if (_i != objects_active.end())
    {
        objects_active.erase(_i);
        VERIFY(std::find(objects_active.begin(), objects_active.end(), O) == objects_active.end());
        VERIFY(std::find(objects_sleeping.begin(), objects_sleeping.end(), O) == objects_sleeping.end());
    }
    else
    {
        Objects::iterator _ii = std::find(objects_sleeping.begin(), objects_sleeping.end(), O);
        if (_ii != objects_sleeping.end())
        {
            objects_sleeping.erase(_ii);
            VERIFY(std::find(objects_sleeping.begin(), objects_sleeping.end(), O) == objects_sleeping.end());
        }
        else
            FATAL("! Unregistered object being destroyed");
    }

    g_pGamePersistent->ObjectPool.destroy(O);
}

void CObjectList::relcase_register(RELCASE_CALLBACK cb, int* ID)
{
    *ID = m_relcase_callbacks.size();
    m_relcase_callbacks.push_back(SRelcasePair(ID, cb));
}

void CObjectList::relcase_unregister(int* ID)
{
    VERIFY(m_relcase_callbacks[*ID].m_ID == ID);
    m_relcase_callbacks[*ID] = m_relcase_callbacks.back();
    *m_relcase_callbacks.back().m_ID = *ID;
    m_relcase_callbacks.pop_back();
}

void CObjectList::dump_list(Objects& v, LPCSTR reason)
{
}

bool CObjectList::dump_all_objects()
{
    dump_list(destroy_queue, "destroy_queue");
    dump_list(objects_active, "objects_active");
    dump_list(objects_sleeping, "objects_sleeping");
    dump_list(m_crows[0], "m_crows[0]");
    dump_list(m_crows[1], "m_crows[1]");
    return false;
}

void CObjectList::register_object_to_destroy(IGameObject* object_to_destroy)
{
    destroy_queue.push_back(object_to_destroy);

    for (auto& it : objects_active)
    {
        IGameObject* O = it;
        if (!O->getDestroy() && O->H_Parent() == object_to_destroy)
        {
            Msg("setDestroy called, but not-destroyed child found parent[%d] child[%d]", object_to_destroy->ID(),
                O->ID(), Device.dwFrame);
            O->setDestroy(true);
        }
    }

    for (auto& it : objects_sleeping)
    {
        IGameObject* O = it;
        if (!O->getDestroy() && O->H_Parent() == object_to_destroy)
        {
            Msg("setDestroy called, but not-destroyed child found parent[%d] child[%d]", object_to_destroy->ID(),
                O->ID(), Device.dwFrame);
            O->setDestroy(true);
        }
    }
}
