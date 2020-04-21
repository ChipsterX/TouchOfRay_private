// ActorCondition.h: класс состояния игрока
//

#pragma once

#include "EntityCondition.h"
#include "actor_defs.h"

template <typename _return_type>
class CScriptCallbackEx;
class CActor;
class CActorDeathEffector;

class CActorCondition : public CEntityCondition
{
private:
    typedef CEntityCondition inherited;
    enum
    {
        eCriticalPowerReached = (1 << 0),
        eCriticalMaxPowerReached = (1 << 1),
        eCriticalBleedingSpeed = (1 << 2),
        eCriticalSatietyReached = (1 << 3),
        eCriticalRadiationReached = (1 << 4),
        eWeaponJammedReached = (1 << 5),
        ePhyHealthMinReached = (1 << 6),
        eCantWalkWeight = (1 << 7),
        eCantWalkWeightReached = (1 << 8),
		eCriticalThirstReached =(1<<9),
		eCriticalSleepReached =(1<<10),	
		eCriticalKurrReached =(1<<11),		
		eCriticalArtefReactionReached = (1 << 12),	
    };
    Flags16 m_condition_flags;

private:
    CActor* m_object;
    CActorDeathEffector* m_death_effector;
    void UpdateTutorialThresholds();
    void UpdateSatiety();
	void UpdateArtefReaction();
	void UpdateSleep();
	void UpdateThirst();
	void UpdateKurr();	

    virtual void UpdateRadiation();

public:
    CActorCondition(CActor* object);
    virtual ~CActorCondition();

    virtual void LoadCondition(LPCSTR section);
    virtual void reinit();

    virtual CWound* ConditionHit(SHit* pHDS);
    virtual void UpdateCondition();
    void UpdateBoosters();

    virtual void ChangeAlcohol(const float value);
    virtual void ChangeSatiety(const float value);
	
	virtual void ChangeArtefReaction(const float value);
	
	virtual void ChangeSleep(const float value);
	virtual void ChangeThirst(const float value);
	virtual void ChangeKurr(const float value);	
	
    void BoostParameters(const SBooster& B);
    void DisableBoostParameters(const SBooster& B);
    inline void BoostMaxWeight(const float value);
    inline void BoostHpRestore(const float value);
    inline void BoostPowerRestore(const float value);
    inline void BoostRadiationRestore(const float value);
    inline void BoostBleedingRestore(const float value);
    inline void BoostBurnImmunity(const float value);
    inline void BoostShockImmunity(const float value);
    inline void BoostRadiationImmunity(const float value);
    inline void BoostTelepaticImmunity(const float value);
    inline void BoostChemicalBurnImmunity(const float value);
    inline void BoostExplImmunity(const float value);
    inline void BoostStrikeImmunity(const float value);
    inline void BoostFireWoundImmunity(const float value);
    inline void BoostWoundImmunity(const float value);
    inline void BoostRadiationProtection(const float value);
    inline void BoostTelepaticProtection(const float value);
    inline void BoostChemicalBurnProtection(const float value);
    BOOSTER_MAP& GetCurBoosterInfluences() { return m_booster_influences; };
    // хромание при потере сил и здоровья
    virtual bool IsLimping() const;
    virtual bool IsCantWalk() const;
    virtual bool IsCantWalkWeight();
    virtual bool IsCantSprint() const;

    void PowerHit(float power, bool apply_outfit);
    float GetPower() const { return m_fPower; }
    void ConditionJump(float weight);
    void ConditionWalk(float weight, bool accel, bool sprint);
    void ConditionStand(float weight);
    inline float MaxWalkWeight() const { return m_MaxWalkWeight; }
    float xr_stdcall GetAlcohol() { return m_fAlcohol; }
    float xr_stdcall GetPsy() { return 1.0f - GetPsyHealth(); }
    float GetSatiety() { return m_fSatiety; }
    inline float GetSatietyPower() const { return m_fV_SatietyPower * m_fSatiety; };
	
	float GetArtefReaction() { return m_fArtefReaction; }
    inline float GetArtefReactionPower() const { return m_fV_ArtefReactionPower * m_fArtefReaction; };
	
	float	GetSleep			()  {return m_fSleep;}
	inline  float	GetSleepPower		() const {return m_fV_SleepPower*m_fSleep;};	
	float	GetThirst			()  {return m_fThirst;}
	inline	float	GetThirstPower		() const {return m_fV_ThirstPower*m_fThirst;};		
	
	float	GetKurr			()  {return m_fKurr;}
	inline	float	GetKurrPower		() const {return m_fV_KurrPower*m_fKurr;};		
	
    void AffectDamage_InjuriousMaterialAndMonstersInfluence();
    float GetInjuriousMaterialDamage();

    void SetZoneDanger(float danger, ALife::EInfluenceType type);
    float GetZoneDanger() const;

public:
    inline CActor& object() const
    {
        VERIFY(m_object);
        return (*m_object);
    }
    virtual void save(NET_Packet& output_packet);
    virtual void load(IReader& input_packet);

    inline float const& V_Satiety() { return m_fV_Satiety; }
    inline float const& V_SatietyPower() { return m_fV_SatietyPower; }
    inline float const& V_SatietyHealth() { return m_fV_SatietyHealth; }
    inline float const& SatietyCritical() { return m_fSatietyCritical; }
	
	inline float const& V_ArtefReaction() { return m_fV_ArtefReaction; }
    inline float const& V_ArtefReactionPower() { return m_fV_ArtefReactionPower; }
    inline float const& V_ArtefReactionHealth() { return m_fV_ArtefReactionHealth; }
    inline float const& ArtefReactionCritical() { return m_fArtefReactionCritical; }
	
	inline		float const&	V_Sleep					()	{ return m_fV_Sleep; }
	inline		float const&	V_SleepPower			()	{ return m_fV_SleepPower; }
	inline		float const&	V_SleepHealth			()	{ return m_fV_SleepHealth; }
	inline		float const&	SleepCritical			()	{ return m_fSleepCritical; }	

	inline		float const&	V_Thirst				()	{ return m_fV_Thirst; }
	inline		float const&	V_ThirstPower			()	{ return m_fV_ThirstPower; }
	inline		float const&	V_ThirstHealth			()	{ return m_fV_ThirstHealth; }
	inline		float const&	ThirstCritical			()	{ return m_fThirstCritical; }

	inline		float const&	V_Kurr				()	{ return m_fV_Kurr; }
	inline		float const&	V_KurrPower			()	{ return m_fV_KurrPower; }
	inline		float const&	V_KurrHealth			()	{ return m_fV_KurrHealth; }
	inline		float const&	KurrCritical			()	{ return m_fKurrCritical; }	

    float GetZoneMaxPower(ALife::EInfluenceType type) const;
    float GetZoneMaxPower(ALife::EHitType hit_type) const;

    bool DisableSprint(SHit* pHDS);
    bool PlayHitSound(SHit* pHDS);
    float HitSlowmo(SHit* pHDS);
    virtual bool ApplyInfluence(const SMedicineInfluenceValues& V, const shared_str& sect);
    virtual bool ApplyBooster(const SBooster& B, const shared_str& sect);
    float GetMaxPowerRestoreSpeed() { return m_max_power_restore_speed; };
    float GetMaxWoundProtection() { return m_max_wound_protection; };
    float GetMaxFireWoundProtection() { return m_max_fire_wound_protection; };
protected:
    SMedicineInfluenceValues m_curr_medicine_influence;
    float m_fAlcohol;
    float m_fV_Alcohol;
    //--
    float m_fSatiety;
    float m_fV_Satiety;
    float m_fV_SatietyPower;
    float m_fV_SatietyHealth;
    float m_fSatietyCritical;
	
	float m_fArtefReaction;
    float m_fV_ArtefReaction;
    float m_fV_ArtefReactionPower;
    float m_fV_ArtefReactionHealth;
    float m_fArtefReactionCritical;
	
	float m_fSleep;
	float m_fV_Sleep;
	float m_fV_SleepPower;
	float m_fV_SleepHealth;
	float m_fSleepCritical;	

	float m_fThirst;
	float m_fV_Thirst;
	float m_fV_ThirstPower;
	float m_fV_ThirstHealth;
	float m_fThirstCritical;		
	
	float m_fKurr;
	float m_fV_Kurr;
	float m_fV_KurrPower;
	float m_fV_KurrHealth;
	float m_fKurrCritical;		
	
    //--
    float m_fPowerLeakSpeed;

    float m_fJumpPower;
    float m_fStandPower;
    float m_fWalkPower;
    float m_fJumpWeightPower;
    float m_fWalkWeightPower;
    float m_fOverweightWalkK;
    float m_fOverweightJumpK;
    float m_fAccelK;
    float m_fSprintK;

public:
    float m_MaxWalkWeight;

protected:
    float m_zone_max_power[ALife::infl_max_count];
    float m_zone_danger[ALife::infl_max_count];
    float m_f_time_affected;
    float m_max_power_restore_speed;
    float m_max_wound_protection;
    float m_max_fire_wound_protection;

    mutable bool m_bLimping;
    mutable bool m_bCantWalk;
    mutable bool m_bCantSprint;

    //порог силы и здоровья меньше которого актер начинает хромать
    float m_fLimpingPowerBegin;
    float m_fLimpingPowerEnd;
    float m_fCantWalkPowerBegin;
    float m_fCantWalkPowerEnd;

    float m_fCantSprintPowerBegin;
    float m_fCantSprintPowerEnd;

    float m_fLimpingHealthBegin;
    float m_fLimpingHealthEnd;

    // typedef xr_vector<SMedicineInfluenceValues> BOOSTS_VECTOR;
    // typedef xr_vector<SMedicineInfluenceValues>::iterator BOOSTS_VECTOR_ITER;
    // BOOSTS_VECTOR m_vecBoosts;
    ref_sound m_use_sound;
};

class CActorDeathEffector
{
    CActorCondition* m_pParent;
    ref_sound m_death_sound;
    bool m_b_actual;
    float m_start_health;
    void xr_stdcall OnPPEffectorReleased();

public:
    CActorDeathEffector(CActorCondition* parent, LPCSTR sect); // -((
    ~CActorDeathEffector();
    void UpdateCL();
    inline bool IsActual() { return m_b_actual; }
    void Stop();
};
