#pragma once
#include "../BaseMonster/base_monster.h"

class CStateManagerBocs;

class CAI_CBocs : public CBaseMonster {
	typedef		CBaseMonster		inherited;
	
public:
					CAI_CBocs 			();
	virtual			~CAI_CBocs 			();

	virtual void	Load				(LPCSTR section);
	virtual void	CheckSpecParams		(u32 spec_params);
	
	virtual void	reinit				();
	
	pcstr			get_monster_class_name() override { return "bocs"; }
};
