#pragma once
#include "missile.h"
#include "xrPhysics/DamageSource.h"
class CBolt : public CMissile, public IDamageSource
{
    typedef CMissile inherited;
    u16 m_thrower_id;

public:
    CBolt();
    virtual ~CBolt();

    virtual void OnH_A_Chield();

    virtual void SetInitiator(u16 id);
    virtual u16 Initiator();

    virtual void Throw();
    virtual bool Action(u16 cmd, u32 flags);
    virtual bool Useful() const;
    virtual void activate_physic_shell();

	virtual bool GetBriefInfo(II_BriefInfo& info);
	virtual void GetBriefInfo_ammo(xr_string& str_name, xr_string& icon_sect_name, xr_string& str_count, string16& fire_mode);
	
    virtual BOOL UsedAI_Locations() { return FALSE; }
    virtual IDamageSource* cast_IDamageSource() { return this; }
};
