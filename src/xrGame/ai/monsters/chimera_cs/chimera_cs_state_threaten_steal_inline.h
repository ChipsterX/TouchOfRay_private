#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>
#define CStateChimecsThreatenStealAbstract CStateChimecsThreatenSteal<_Object>

TEMPLATE_SPECIALIZATION
void CStateChimecsThreatenStealAbstract::initialize()
{
	inherited::initialize();
	
	object->SetUpperState	(false);

	data.action.action		= ACT_STEAL;
	
	data.accelerated		= true;
	data.braking			= false;
	data.accel_type 		= eAT_Calm;
	
	data.completion_dist	= 2.f;
	data.action.sound_type	= MonsterSound::eMonsterSoundIdle;
	data.action.sound_delay = object->db().m_dwIdleSndDelay;
}

TEMPLATE_SPECIALIZATION
void CStateChimecsThreatenStealAbstract::finalize()
{
	inherited::finalize();
	object->SetUpperState	();
}


TEMPLATE_SPECIALIZATION
void CStateChimecsThreatenStealAbstract::execute()
{
	data.point				= object->EnemyMan.get_enemy_position	();
	data.vertex				= object->EnemyMan.get_enemy_vertex		();
	data.time_to_rebuild	= object->get_attack_rebuild_time		();

	inherited::execute();
}

#define MIN_DISTANCE_TO_ENEMY	8.f

TEMPLATE_SPECIALIZATION
bool CStateChimecsThreatenStealAbstract::check_completion()
{	
	if (inherited::check_completion()) return true;
	
	float dist_to_enemy = object->EnemyMan.get_enemy_position().distance_to(object->Position());
	if (dist_to_enemy < MIN_DISTANCE_TO_ENEMY) return true;

	return false;
}

TEMPLATE_SPECIALIZATION
bool CStateChimecsThreatenStealAbstract::check_start_conditions()
{
	float dist_to_enemy = object->EnemyMan.get_enemy_position().distance_to(object->Position());
	if (dist_to_enemy > MIN_DISTANCE_TO_ENEMY) return true;
	return false;
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateChimecsThreatenStealAbstract
#undef MIN_DISTANCE_TO_ENEMY
