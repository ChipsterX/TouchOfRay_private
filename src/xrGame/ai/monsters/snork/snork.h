#pragma once
#include "../BaseMonster/base_monster.h"

class CSnork :	public CBaseMonster 
{
	typedef		CBaseMonster		inherited;

	SVelocityParam	m_fsVelocityJumpPrepare;
	SVelocityParam	m_fsVelocityJumpGround;

public:
					CSnork				();
	virtual			~CSnork				();	

	virtual void	Load				(LPCSTR section);
	virtual void	reinit				();				
	virtual void	UpdateCL			();
	virtual void	CheckSpecParams		(u32 spec_params);
	virtual void	jump				(const Fvector &position, float factor);
	virtual void	HitEntityInJump		(const CEntity *pEntity);
	virtual void	on_activate_control(ControlCom::EControlType);
	virtual bool	check_start_conditions(ControlCom::EControlType type);
	virtual bool	ability_distant_feel() { return true; }
	virtual bool	ability_jump_over_physics() { return true; }
			
			bool	find_geometry		(Fvector &dir);
			float	trace				(const Fvector &dir);

			bool	trace_geometry		(const Fvector &d, float &range);
			pcstr   get_monster_class_name() override { return "snork"; }

public:
			u32		m_target_node;
			bool	start_threaten;
};
