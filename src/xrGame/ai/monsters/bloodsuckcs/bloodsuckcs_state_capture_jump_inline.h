#pragma once

//#include "../states/monster_state_rest_idle.h"
#include "../states/state_custom_action.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateCaptureJumpBloodsuckcsAbstract CStateCaptureJumpBloodsuckcs<_Object>


TEMPLATE_SPECIALIZATION
CStateCaptureJumpBloodsuckcsAbstract::CStateCaptureJumpBloodsuckcs(_Object *obj) : inherited(obj)
{
	add_state(eStateCustom,				xr_new CStateMonsterCustomAction<_Object> 		(obj));
}

TEMPLATE_SPECIALIZATION
CStateCaptureJumpBloodsuckcsAbstract::~CStateCaptureJumpBloodsuckcs	()
{
}

TEMPLATE_SPECIALIZATION
void CStateCaptureJumpBloodsuckcsAbstract::execute()
{
	// check alife control
	select_state	(eStateCustom);

	get_state_current()->execute();
	prev_substate = current_substate;
}
TEMPLATE_SPECIALIZATION
void CStateCaptureJumpBloodsuckcsAbstract::setup_substates()
{
	state_ptr state = get_state_current();
	if (current_substate == eStateCustom) {
		SStateDataAction data;

		data.action		= ACT_STAND_IDLE;
		data.time_out	= 0;			// do not use time out
		state->fill_data_with(&data, sizeof(SStateDataAction));

		return;
	}
}
#undef TEMPLATE_SPECIALIZATION
#undef CStateCaptureJumpBloodsuckcsAbstract
