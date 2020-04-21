#include "stdafx.h"
#include "xrSheduler.h"
#include "xr_object.h"
// XXX: rename this file to ScheduledBase.cpp
ScheduledBase::ScheduledBase()
{
    shedule.t_min = 20;
    shedule.t_max = 1000;
    shedule.b_locked = FALSE;
}

ScheduledBase::~ScheduledBase()
{
// XXX: WTF???
// sad, but true
// we need this to become MASTER_GOLD
    Engine.Sheduler.Unregister(this);
}

void ScheduledBase::shedule_register() { Engine.Sheduler.Register(this); }
void ScheduledBase::shedule_unregister() { Engine.Sheduler.Unregister(this); }
void ScheduledBase::shedule_Update(u32 dt) {}

void ScheduledBaseMT::shedule_register() { Engine.Scheduler.Register(this); }
void ScheduledBaseMT::shedule_unregister() { Engine.Scheduler.Unregister(this); }
