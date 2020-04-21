#pragma once
#include "../BaseMonster/base_monster.h"
#include "../controlled_entity.h"

class CAI_Dogsoc : public CBaseMonster, 
				public CControlledEntity<CAI_Dogsoc> {
	
	typedef		CBaseMonster				inherited;
	typedef		CControlledEntity<CAI_Dogsoc>	CControlled;

public:
					CAI_Dogsoc				();
	virtual			~CAI_Dogsoc			();	

	virtual void	Load				(LPCSTR section);
	virtual void	reinit				();

	virtual void	CheckSpecParams		(u32 spec_params);

	virtual bool	ability_can_drag		() {return true;}
    pcstr           get_monster_class_name  () override { return "dogsoc";}
	
};

