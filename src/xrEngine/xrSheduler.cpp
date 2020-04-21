#include "stdafx.h"
#include "xrSheduler.h"
#include "xr_object.h"
#include "GameFont.h"
#include "PerformanceAlert.hpp"

#include <tbb/parallel_for.h>
#include <tbb/parallel_sort.h>

float psShedulerCurrent = 10.f;
float psShedulerTarget = 10.f;
const float psShedulerReaction = 0.1f;
bool isSheduleInProgress = false;

//-------------------------------------------------------------------------------------
void CSheduler::Initialize()
{
    m_current_step_obj = nullptr;
    m_processing_now = false;
}

void CSheduler::Destroy()
{
    internal_Registration();

    for (u32 it = 0; it < Items.size(); it++)
    {
        if (nullptr == Items[it].Object)
        {
            Items.erase(Items.begin() + it);
            it--;
        }
    }

    ItemsRT.clear();
    Items.clear();
    ItemsProcessed.clear();
    Registration.clear();
}

void CSheduler::DumpStatistics(IGameFont& font, IPerformanceAlert* alert)
{
    stats.FrameEnd();
    const float percentage = 100.f * stats.Update.result / Device.GetStats().EngineTotal.result;
    font.OutNext("Scheduler:");
    font.OutNext("- update:     %2.2fms, %2.1f%%", stats.Update.result, percentage);
    font.OutNext("- load:       %2.2fms", stats.Load);
    if (alert && stats.Update.result > 3.0f)
        alert->Print(font, "Update    > 3ms:  %3.1f", stats.Update.result);
    stats.FrameStart();
}

void CSheduler::internal_Registration()
{
    for (u32 it = 0; it < Registration.size(); it++)
    {
        ItemReg& R = Registration[it];
        if (R.OP)
        {
            // register
            // search for paired "unregister"
            bool foundAndErased = false;
            for (u32 pair = it + 1; pair < Registration.size(); pair++)
            {
                ItemReg& R_pair = Registration[pair];
                if (!R_pair.OP && R_pair.Object == R.Object)
                {
                    foundAndErased = true;
                    Registration.erase(Registration.begin() + pair);
                    break;
                }
            }

            // register if non-paired
            if (!foundAndErased)
            {

                internal_Register(R.Object, R.RT);
            }
        }
        else
        {
            // unregister
            internal_Unregister(R.Object, R.RT);
        }
    }
    Registration.clear();
}

void CSheduler::internal_Register(ISheduled* object, BOOL realTime)
{
    VERIFY(!object->GetSchedulerData().b_locked);

    // Fill item structure
    Item item;
    item.dwTimeForExecute = Device.dwTimeGlobal;
    item.dwTimeOfLastExecute = Device.dwTimeGlobal;
    item.scheduled_name = object->shedule_Name();
    item.Object = object;

    if (realTime)
    {
        object->GetSchedulerData().b_RT = TRUE;
        ItemsRT.emplace_back(std::move(item));
    }
    else
    {
        object->GetSchedulerData().b_RT = FALSE;

        // Insert into priority Queue
        Push(item);
    }
}

bool CSheduler::internal_Unregister(ISheduled* object, BOOL realTime, bool warn_on_not_found)
{
    // the object may be already dead
    // VERIFY (!O->shedule.b_locked);
    if (realTime)
    {
        for (u32 i = 0; i < ItemsRT.size(); i++)
        {
            if (ItemsRT[i].Object == object)
            {
                ItemsRT.erase(ItemsRT.begin() + i);
                return true;
            }
        }
    }
    else
    {
        for (auto& item : Items)
        {
            if (item.Object == object)
            {
                item.Object = nullptr;
                return true;
            }
        }
    }
    if (m_current_step_obj == object)
    {
        m_current_step_obj = nullptr;
        return true;
    }

    return false;
}


void CSheduler::Register(ISheduled* A, BOOL RT)
{

    ItemReg R;
    R.OP = TRUE;
    R.RT = RT;
    R.Object = A;
    R.Object->GetSchedulerData().b_RT = RT;

    Registration.push_back(R);
}

void CSheduler::Unregister(ISheduled* A)
{

    if (m_processing_now)
    {
        if (internal_Unregister(A, A->GetSchedulerData().b_RT, false))
            return;
    }

    ItemReg R;
    R.OP = FALSE;
    R.RT = A->GetSchedulerData().b_RT;
    R.Object = A;

    Registration.push_back(R);
}

void CSheduler::EnsureOrder(ISheduled* Before, ISheduled* After)
{
    VERIFY(Before->GetSchedulerData().b_RT && After->GetSchedulerData().b_RT);

    for (u32 i = 0; i < ItemsRT.size(); i++)
    {
        if (ItemsRT[i].Object == After)
        {
            Item A = ItemsRT[i];
            ItemsRT.erase(ItemsRT.begin() + i);
            ItemsRT.push_back(A);
            return;
        }
    }
}

void CSheduler::Push(Item& item)
{
    Items.emplace_back(std::move(item));
    std::push_heap(Items.begin(), Items.end());
}

void CSheduler::Pop()
{
    std::pop_heap(Items.begin(), Items.end());
    Items.pop_back();
}

void CSheduler::ProcessStep()
{
    // Normal priority
    const u32 dwTime = Device.dwTimeGlobal;
    CTimer eTimer;

    for (int i = 0; !Items.empty() && Top().dwTimeForExecute < dwTime; ++i)
    {
        // Update
        Item item = Top();

        if (!item.Object || !item.Object->shedule_Needed())
        {
            // Erase element
            Pop();
            continue;
        }

        // Insert into priority Queue
        Pop();

        u32 Elapsed = dwTime - item.dwTimeOfLastExecute;

        // Real update call
        // Msg("------- %d:", Device.dwFrame);

        // Calc next update interval
        const u32 dwMin = _max(u32(30), item.Object->GetSchedulerData().t_min);
        u32 dwMax = (1000 + item.Object->GetSchedulerData().t_max) / 2;
        const float scale = item.Object->shedule_Scale();
        u32 dwUpdate = dwMin + iFloor(float(dwMax - dwMin) * scale);
        clamp(dwUpdate, u32(_max(dwMin, u32(20))), dwMax);

        m_current_step_obj = item.Object;

        item.Object->shedule_Update(
            clampr(Elapsed, u32(1), u32(_max(u32(item.Object->GetSchedulerData().t_max), u32(1000)))));
        if (!m_current_step_obj)
        {
            continue;
        }

        m_current_step_obj = nullptr;

        // Fill item structure
        item.dwTimeForExecute = dwTime + dwUpdate;
        item.dwTimeOfLastExecute = dwTime;
        ItemsProcessed.emplace_back(std::move(item));

        if (i % 3 != 3 - 1)
            continue;

        if (Device.dwPrecacheFrame == 0 && CPU::QPC() > cycles_limit)
        {
            // we have maxed out the load - increase heap
            psShedulerTarget += psShedulerReaction * 3;
            break;
        }
    }

    // Push "processed" back
    while (ItemsProcessed.size())
    {
        Push(ItemsProcessed.back());
        ItemsProcessed.pop_back();
    }

    // always try to decrease target
    psShedulerTarget -= psShedulerReaction;
}

void CSheduler::Update()
{
    // Initialize
    stats.Update.Begin();
    cycles_start = CPU::QPC();
    cycles_limit = CPU::qpc_freq * u64(iCeil(psShedulerCurrent)) / 1000ul + cycles_start;
    internal_Registration();
    isSheduleInProgress = true;

    // Realtime priority
    m_processing_now = true;
    const u32 dwTime = Device.dwTimeGlobal;
    for (auto& item : ItemsRT)
    {
        R_ASSERT(item.Object);

        if (!item.Object->shedule_Needed())
        {
            item.dwTimeOfLastExecute = dwTime;
            continue;
        }

        const u32 Elapsed = dwTime - item.dwTimeOfLastExecute;
        item.Object->shedule_Update(Elapsed);
        item.dwTimeOfLastExecute = dwTime;
    }

    // Normal (sheduled)
    ProcessStep();
    m_processing_now = false;

    clamp(psShedulerTarget, 3.f, 66.f);
    psShedulerCurrent = 0.9f * psShedulerCurrent + 0.1f * psShedulerTarget;
    stats.Load = psShedulerCurrent;

    // Finalize
    isSheduleInProgress = false;
    internal_Registration();
    stats.Update.End();
}

namespace XRay
{
float psShedulerCurrent = 10.f;
float psShedulerTarget = 10.f;
const float psShedulerReaction = 0.1f;

void Scheduler::Initialize()
{
    // nothing to Initialize();
}

void Scheduler::Destroy()
{
    processRegistrationQueue();

    for (size_t it = 0; it < UpdateQueue.size(); ++it)
    {
        if (nullptr == UpdateQueue[it].Object)
        {
            UpdateQueue.erase(UpdateQueue.begin() + it);
            --it;
        }
    }

    RealtimeUpdateQueue.clear();
    UpdateQueue.clear();
    RegistrationQueue.clear();
}

void Scheduler::DumpStatistics(IGameFont& font, IPerformanceAlert* alert)
{
    stats.FrameEnd();
    const float percentage = 100.f * stats.Update.result / Device.GetStats().EngineTotal.result;
    font.OutNext("SchedulerMT:");
    font.OutNext("- update:     %2.2fms, %2.1f%%", stats.Update.result, percentage);
    font.OutNext("- load:       %2.2fms", stats.Load);
    if (alert && stats.Update.result > 3.0f)
        alert->Print(font, "Update    > 3ms:  %3.1f", stats.Update.result);
    stats.FrameStart();
}

void Scheduler::internalRegister(ItemReg&& item)
{
    VERIFY(!item.Object->GetSchedulerData().b_locked);

    // Fill item structure
    Item newItem;
    newItem.TimeForExecute = Device.dwTimeGlobal;
    newItem.TimeOfLastExecute = Device.dwTimeGlobal;
    newItem.ScheduledName = item.Object->shedule_Name();
    newItem.Object = std::move(item.Object);

    if (item.RealtimePriority)
    {
        newItem.Object->GetSchedulerData().b_RT = TRUE;
        RealtimeUpdateQueue.emplace_back(std::move(newItem));
    }
    else
    {
        newItem.Object->GetSchedulerData().b_RT = FALSE;
        UpdateQueue.emplace_back(std::move(newItem));
    }
}

bool Scheduler::internalUnregister(ItemReg&& item, const bool warnWhenNotFound /*= true*/)
{
    // the object may be already dead
    // VERIFY (!O->shedule.b_locked);
    if (item.RealtimePriority)
    {
        for (size_t i = 0; i < RealtimeUpdateQueue.size(); ++i)
        {
            if (RealtimeUpdateQueue[i].Object == item.Object)
            {

                RealtimeUpdateQueue.erase(RealtimeUpdateQueue.begin() + i);
                return true;
            }
        }
    }
    else
    {
        for (size_t i = 0; i < UpdateQueue.size(); ++i)
        {
            if (UpdateQueue[i].Object == item.Object)
            {
                UpdateQueue.erase(UpdateQueue.begin() + i);
                return true;
            }
        }
    }

    return false;
}

void Scheduler::processRegistrationQueue()
{
    for (size_t it = 0; it < RegistrationQueue.size(); ++it)
    {
        ItemReg& item = RegistrationQueue[it];
        if (item.Operational)
        {
            // register
            // search for paired "unregister"
            bool foundAndErased = false;
            for (size_t pair = it + 1; pair < RegistrationQueue.size(); ++pair)
            {
                ItemReg& itemPair = RegistrationQueue[pair];
                if (!itemPair.Operational && itemPair.Object == item.Object)
                {
                    foundAndErased = true;
                    RegistrationQueue.erase(RegistrationQueue.begin() + pair);
                    break;
                }
            }

            // register if non-paired
            if (!foundAndErased)
            {
                internalRegister(std::move(item));
            }
        }
        else
            internalUnregister(std::move(item));
    }

    RegistrationQueue.clear();
}

bool Scheduler::Registered(ISheduled* object) const
{
    u32 count = 0;

    for (const auto& it : RealtimeUpdateQueue)
    {
        if (it.Object == object)
        {
            //Msg("0x%8x found in RT", object);
            count = 1;
            break;
        }
    }

    for (const auto& it : UpdateQueue)
    {
        if (it.Object == object)
        {
            //Msg("0x%8x found in non-RT", object);
            VERIFY(!count);
            count = 1;
            break;
        }
    }

    for (const auto& it : RegistrationQueue)
    {
        if (it.Object == object)
        {
            if (it.Operational)
            {
                //Msg("0x%8x found in registration on register", object);
                VERIFY(!count);
                ++count;
            }
            else
            {
                //Msg("0x%8x found in registration on UNregister", object);
                VERIFY(count == 1);
                --count;
            }
        }
    }

    VERIFY(!count || count == 1);
    return count == 1;
}

void Scheduler::Register(ISheduled* object, const bool realtime /*= false*/)
{
    VERIFY(!Registered(object));

    ItemReg newItem = { true, realtime, object };
    newItem.Object->GetSchedulerData().b_RT = realtime;

    RegistrationQueue.emplace_back(std::move(newItem));
}

void Scheduler::Unregister(ISheduled* object)
{
    VERIFY(Registered(object));

    ItemReg item = { false, object->GetSchedulerData().b_RT, object };

    RegistrationQueue.emplace_back(std::move(item));
}

void Scheduler::EnsureOrder(ISheduled* before, ISheduled* after)
{
    VERIFY(before->GetSchedulerData().b_RT && after->GetSchedulerData().b_RT);

    auto& RUQ = RealtimeUpdateQueue;
    for (size_t i = 0; i < RUQ.size(); ++i)
    {
        if (RUQ[i].Object == after)
        {
            auto&& item = std::move(RUQ[i]);
            RUQ.erase(RUQ.begin() + i);
            RUQ.emplace_back(std::move(item));
            return;
        }
    }
}

void Scheduler::ProcessStep()
{
    // Initialize
    stats.Update.Begin();
    cyclesStart = CPU::QPC();
    cyclesLimit = CPU::qpc_freq * u64(iCeil(psShedulerCurrent)) / 1000ul + cyclesStart;
    processRegistrationQueue();

    processingNow = true;

    ProcessRealtimeQueueQueue(); // Realtime priority
    ProcessUpdateQueue(); // Normal (scheduled)

    processingNow = false;

    clamp(psShedulerTarget, 3.f, 66.f);
    psShedulerCurrent = 0.9f * psShedulerCurrent + 0.1f * psShedulerTarget;
    stats.Load = psShedulerCurrent;

    // Finalize
    processRegistrationQueue();
    stats.Update.End();
}

void Scheduler::ProcessRealtimeQueueQueue()
{
    const auto dwTime = Device.dwTimeGlobal;
    for (auto& item : RealtimeUpdateQueue)
    {
        R_ASSERT(item.Object);

        if (!item.Object->shedule_Needed())
        {
            item.TimeOfLastExecute = dwTime;
            continue;
        }

        const auto Elapsed = dwTime - item.TimeOfLastExecute;
        item.Object->shedule_Update(Elapsed);
        item.TimeOfLastExecute = dwTime;
    }
}

void Scheduler::ProcessUpdateQueue()
{
    const auto dwTime = Device.dwTimeGlobal;
    CTimer eTimer;

    ACCELERATED_SORT(UpdateQueue.begin(), UpdateQueue.end(), std::less<Item>());

    FOR_START(size_t, 0, UpdateQueue.size(), i)
        {
            auto item = UpdateQueue[i];

            if (item.TimeForExecute < dwTime)
                continue;

            if (!item.Object || !item.Object->shedule_Needed())
            {
                Unregister(item.Object);
                continue;
            }

            // Calc next update interval
            auto Elapsed = dwTime - item.TimeOfLastExecute;
            const u32 dwMin = _max(u32(30), item.Object->GetSchedulerData().t_min);
            u32 dwMax = (1000 + item.Object->GetSchedulerData().t_max) / 2;
            const float scale = item.Object->shedule_Scale();
            u32 dwUpdate = dwMin + iFloor(float(dwMax - dwMin) * scale);
            clamp(dwUpdate, u32(_max(dwMin, u32(20))), dwMax);

            item.Object->shedule_Update(
                clampr(Elapsed, u32(1), u32(_max(item.Object->GetSchedulerData().t_max, u32(1000)))));

            if (!item.Object)
            {
                Unregister(item.Object);
                continue;
            }

            // Fill item structure
            item.TimeForExecute = dwTime + dwUpdate;
            item.TimeOfLastExecute = dwTime;

            if (i % 3 != 3 - 1)
                continue;

            if (Device.dwPrecacheFrame == 0 && CPU::QPC() > cyclesLimit)
            {
                // we have maxed out the load - increase heap
                psShedulerTarget += psShedulerReaction * 3;
                break;
            }
        }
    FOR_END

    ACCELERATED_SORT(UpdateQueue.begin(), UpdateQueue.end(), std::less<Item>());

    // always try to decrease target
    psShedulerTarget -= psShedulerReaction;
}
} // namespace XRay
