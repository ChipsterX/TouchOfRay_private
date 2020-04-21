#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateChimecsHuntingMoveToCoverAbstract CStateChimecsHuntingMoveToCover<_Object>

TEMPLATE_SPECIALIZATION
CStateChimecsHuntingMoveToCoverAbstract::CStateChimecsHuntingMoveToCover(_Object *obj) : inherited(obj)
{
}


TEMPLATE_SPECIALIZATION
bool CStateChimecsHuntingMoveToCoverAbstract::check_start_conditions()
{
	return true;
}

TEMPLATE_SPECIALIZATION
bool CStateChimecsHuntingMoveToCoverAbstract::check_completion()
{
	return false;
}

TEMPLATE_SPECIALIZATION
void CStateChimecsHuntingMoveToCoverAbstract::reselect_state()
{
	if (prev_substate == u32(-1))					select_state(eStateMoveToCover);
	else if (prev_substate == eStateMoveToCover)	select_state(eStateComeOut);
	else											select_state(eStateMoveToCover);
}


#undef TEMPLATE_SPECIALIZATION
#undef CStateChimecsHuntingMoveToCoverAbstract
