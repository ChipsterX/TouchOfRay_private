#pragma once

#include "../BaseMonster/base_monster.h"
#include "../telekinesis.h"
#include "../energy_holder.h"


class CPhysicsShellHolder;
class CStateManagerPoltergeist_cs;
class CPoltergeisMovementManager_cs;
class CPolterSpecialAbility_cs;

//////////////////////////////////////////////////////////////////////////

class CPoltergeist_cs :	public CBaseMonster ,
						public CTelekinesis,
						public CEnergyHolder {
	
	typedef		CBaseMonster	inherited;
	typedef		CEnergyHolder	Energy;

	friend class CPoltergeisMovementManager_cs;

	float					m_height;
	bool					m_disable_hide;

	SMotionVel				invisible_vel;

    CPolterSpecialAbility_cs* m_flame;
    CPolterSpecialAbility_cs* m_tele_cs; // Пересечение с ЗП классом

public:
					CPoltergeist_cs		();
	virtual			~CPoltergeist_cs		();	

	virtual void	Load				(LPCSTR section);
	virtual void	reload				(LPCSTR section);
	virtual void	reinit				();

	virtual BOOL	net_Spawn			(CSE_Abstract* DC);
	virtual void	net_Destroy			();
	virtual void	net_Relcase			(IGameObject *O);

	virtual void	UpdateCL			();
	virtual	void	shedule_Update		(u32 dt);

	virtual void	Die					(IGameObject* who);

	virtual CMovementManager *create_movement_manager();
	
	virtual void	ForceFinalAnimation	();

	virtual	void	on_activate			();
	virtual	void	on_deactivate		();
	virtual	void	Hit					(SHit* pHDS);
    pcstr get_monster_class_name() override { return "poltergeist_cs"; }

	IC		CPolterSpecialAbility_cs		*ability() {return (m_flame ? m_flame : m_tele_cs);}
	
	
	IC		bool	is_hidden			() {return state_invisible;}

	
	// Poltergeist ability
			void	PhysicalImpulse		(const Fvector &position);
			void	StrangeSounds		(const Fvector &position);
			
			ref_sound m_strange_sound;
	
	// Movement
			Fvector m_current_position;		// Позиция на ноде

	// Dynamic Height
			u32		time_height_updated;
			float	target_height;

			void	UpdateHeight			();

	// Invisibility 

			void	EnableHide				(){m_disable_hide = false;}
			void	DisableHide				(){m_disable_hide = true;}
	
public:												// Из приватной в публичную, для доступа в poltergeist_cs_state_manager.cpp
			void	Hide_pcs					();
			void	Show_pcs					();


};

//////////////////////////////////////////////////////////////////////////
// Interface
//////////////////////////////////////////////////////////////////////////

class CPolterSpecialAbility_cs {

	CParticlesObject	*m_particles_object;
	CParticlesObject	*m_particles_object_electro;

	LPCSTR				m_particles_hidden;
	LPCSTR				m_particles_damage;
	LPCSTR				m_particles_death;
	LPCSTR				m_particles_idle;

	ref_sound			m_sound_base;
	u32					m_last_hit_frame;

protected:
	CPoltergeist_cs		*m_object;	

public:			
					CPolterSpecialAbility_cs		(CPoltergeist_cs *polter);
	virtual			~CPolterSpecialAbility_cs		();

	virtual void	load						(LPCSTR section);
	virtual void	update_schedule				();
	virtual void	update_frame				();
	virtual void	on_hide						();
	virtual void	on_show						();
	virtual void	on_destroy					(){}
	virtual void	on_die						();
	virtual void	on_hit						(SHit* pHDS);
};



//////////////////////////////////////////////////////////////////////////
// Flame
//////////////////////////////////////////////////////////////////////////
class CPolterFlame_cs : public CPolterSpecialAbility_cs {

	typedef CPolterSpecialAbility_cs inherited;

	ref_sound				m_sound;
	LPCSTR					m_particles_prepare;
	LPCSTR					m_particles_fire;
	LPCSTR					m_particles_stop;
	u32						m_time_fire_delay;
	u32						m_time_fire_play;

	float					m_length;
	float					m_hit_value;
	u32						m_hit_delay;

	u32						m_count;
	u32						m_delay;	// between 2 flames

	u32						m_time_flame_started;

	float					m_min_flame_dist;
	float					m_max_flame_dist;
	float					m_min_flame_height;
	float					m_max_flame_height;

	float					m_pmt_aura_radius;

	
	// Scanner
	float					m_scan_radius;
	u32						m_scan_delay_min;
	u32						m_scan_delay_max;
	
	SPPInfo					m_scan_effector_info;
	float					m_scan_effector_time;
	float					m_scan_effector_time_attack;
	float					m_scan_effector_time_release;
	ref_sound				m_scan_sound;

	bool					m_state_scanning;
	u32						m_scan_next_time;


	enum EFlameState {
		ePrepare,
		eFire,
		eStop
	};


public:
	struct SFlameElement {
		const IGameObject		*target_object;
		Fvector				position;
		Fvector				target_dir;
		u32					time_started;
		ref_sound			sound;
		CParticlesObject	*particles_object;
		EFlameState			state;
		u32					time_last_hit;
	};


private:
	DEFINE_VECTOR			(SFlameElement*, FLAME_ELEMS_VEC, FLAME_ELEMS_IT);
	FLAME_ELEMS_VEC			m_flames;

public:	
					CPolterFlame_cs				(CPoltergeist_cs *polter);
	virtual			~CPolterFlame_cs				();

	virtual void	load						(LPCSTR section);
	virtual void	update_schedule				();
	virtual void	on_destroy					();
	virtual void	on_die						();

private:
			void	select_state				(SFlameElement *elem, EFlameState state);
			bool	get_valid_flame_position	(const IGameObject *target_object, Fvector &res_pos);
			void	create_flame				(const IGameObject *target_object);
};


//////////////////////////////////////////////////////////////////////////
// TELE
//////////////////////////////////////////////////////////////////////////
class CPolterTele_cs : public CPolterSpecialAbility_cs {
	typedef CPolterSpecialAbility_cs inherited;

	xr_vector<IGameObject*>	m_nearest;

	// external params
	float				m_pmt_radius;
	float				m_pmt_object_min_mass;
	float				m_pmt_object_max_mass;
	u32					m_pmt_object_count;
	u32					m_pmt_time_to_hold;
	u32					m_pmt_time_to_wait;
	u32					m_pmt_time_to_wait_in_objects;
	u32					m_pmt_raise_time_to_wait_in_objects;
	float				m_pmt_distance;
	float				m_pmt_object_height;
	u32					m_pmt_time_object_keep;
	float				m_pmt_raise_speed;
	float				m_pmt_fly_velocity;

	ref_sound			m_sound_tele_hold;
	ref_sound			m_sound_tele_throw;


	enum ETeleState {
		eStartRaiseObjects,
		eRaisingObjects,
		eFireObjects,
		eWait
	} m_state;

	u32					m_time;
	u32					m_time_next;

public:	
					CPolterTele_cs					(CPoltergeist_cs *polter);
	virtual			~CPolterTele_cs				();

	virtual void	load						(LPCSTR section);
	virtual void	update_schedule				();

private:
			void	tele_find_objects			(xr_vector<IGameObject*> &objects, const Fvector &pos);
			bool	tele_raise_objects			();
			void	tele_fire_objects			();

			bool	trace_object				(IGameObject *obj, const Fvector &target);
};
