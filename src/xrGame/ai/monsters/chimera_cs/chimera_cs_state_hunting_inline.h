#pragma once

#include "chimera_cs_state_hunting_move_to_cover.h"
#include "chimera_cs_state_hunting_come_out.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateChimecsHuntingAbstract CStateChimecsHunting<_Object>

TEMPLATE_SPECIALIZATION
CStateChimecsHuntingAbstract::CStateChimecsHunting(_Object *obj) : inherited(obj)
{
	add_state(eStateMoveToCover,	xr_new<CStateChimecsHuntingMoveToCover<_Object> >	(obj));
	add_state(eStateComeOut,		xr_new<CStateChimecsHuntingComeOut<_Object> >		(obj));
}


TEMPLATE_SPECIALIZATION
bool CStateChimecsHuntingAbstract::check_start_conditions()
{
	return true;
}

TEMPLATE_SPECIALIZATION
bool CStateChimecsHuntingAbstract::check_completion()
{
	return false;
}

TEMPLATE_SPECIALIZATION
void CStateChimecsHuntingAbstract::reselect_state()
{
	if (prev_substate == u32(-1))					select_state(eStateMoveToCover);
	else if (prev_substate == eStateMoveToCover)	select_state(eStateComeOut);
	else											select_state(eStateMoveToCover);
}


#undef TEMPLATE_SPECIALIZATION
#undef CStateChimecsHuntingAbstract
