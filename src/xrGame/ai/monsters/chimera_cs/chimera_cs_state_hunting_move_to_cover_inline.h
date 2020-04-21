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
void CStateChimecsHuntingMoveToCoverAbstract::initialize()
{
	inherited::initialize();
	
	
}

TEMPLATE_SPECIALIZATION
bool CStateChimecsHuntingMoveToCoverAbstract::check_completion()
{
	return false;
}

TEMPLATE_SPECIALIZATION
void CStateChimecsHuntingMoveToCoverAbstract::execute()
{
	
}


#undef TEMPLATE_SPECIALIZATION
#undef CStateChimecsHuntingMoveToCoverAbstract
