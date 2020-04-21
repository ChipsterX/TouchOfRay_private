#pragma once

#include "chimera_cs_state_threaten_steal.h"
#include "chimera_cs_state_threaten_walk.h"
#include "chimera_cs_state_threaten_roar.h"


#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateChimecsThreatenAbstract CStateChimecsThreaten<_Object>

TEMPLATE_SPECIALIZATION
CStateChimecsThreatenAbstract::CStateChimecsThreaten(_Object *obj) : inherited(obj)
{
	add_state(eStateWalk,		new CStateChimecsThreatenWalk<_Object> 	(obj));
	add_state(eStateThreaten,	new CStateChimecsThreatenRoar<_Object> 	(obj));
	add_state(eStateSteal,		new CStateChimecsThreatenSteal<_Object> (obj));
}

TEMPLATE_SPECIALIZATION
CStateChimecsThreatenAbstract::~CStateChimecsThreaten()
{
}

TEMPLATE_SPECIALIZATION
void CStateChimecsThreatenAbstract::reinit()
{
	inherited::reinit	();

	m_last_time_threaten = 0;
}


#define MIN_DIST_TO_ENEMY	3.f
#define MORALE_THRESHOLD	0.8f
#define THREATEN_DELAY		10000

TEMPLATE_SPECIALIZATION
bool CStateChimecsThreatenAbstract::check_start_conditions()
{
	if (object->tfGetRelationType(object->EnemyMan.get_enemy()) == ALife::eRelationTypeWorstEnemy) return false;
	if (object->Position().distance_to(object->EnemyMan.get_enemy_position()) < MIN_DIST_TO_ENEMY) return false;
	if (object->HitMemory.is_hit())						return false;
	if (object->hear_dangerous_sound)					return false;
	if (m_last_time_threaten + THREATEN_DELAY > Device.dwTimeGlobal) return false;

	return true;
}

TEMPLATE_SPECIALIZATION
bool CStateChimecsThreatenAbstract::check_completion()
{
	if (object->Position().distance_to(object->EnemyMan.get_enemy_position()) < MIN_DIST_TO_ENEMY) return true;
	if (object->HitMemory.is_hit()) return true;
	if (object->tfGetRelationType(object->EnemyMan.get_enemy()) == ALife::eRelationTypeWorstEnemy) return true;

	return false;
}

TEMPLATE_SPECIALIZATION
void CStateChimecsThreatenAbstract::initialize()
{
	inherited::initialize	();
	object->SetUpperState	();
}

TEMPLATE_SPECIALIZATION
void CStateChimecsThreatenAbstract::reselect_state()
{
	if (prev_substate == u32(-1)) {
		select_state(eStateThreaten);
		return;
	}

	if (prev_substate == eStateSteal) {
		select_state(eStateThreaten);
		return;
	}

	if (prev_substate == eStateThreaten) {
		if (get_state(eStateSteal)->check_start_conditions()) {
			select_state(eStateSteal);
			return;
		} else if (get_state(eStateWalk)->check_start_conditions()) {
			select_state(eStateWalk);
			return;
		}
	}

	select_state(eStateThreaten);
}

TEMPLATE_SPECIALIZATION
void CStateChimecsThreatenAbstract::finalize()
{
	inherited::finalize		();
	object->SetUpperState	(false);
	m_last_time_threaten	 = Device.dwTimeGlobal;
}

TEMPLATE_SPECIALIZATION
void CStateChimecsThreatenAbstract::critical_finalize()
{
	inherited::critical_finalize();
	object->SetUpperState	(false);
	m_last_time_threaten	 = Device.dwTimeGlobal;
}


#undef TEMPLATE_SPECIALIZATION
#undef CStateChimecsThreatenAbstract

