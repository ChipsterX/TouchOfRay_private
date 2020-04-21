#include "stdafx.h"
#include "torch.h"
#include "entity.h"
#include "actor.h"
#include "xrEngine/LightAnimLibrary.h"
#include "xrPhysics/PhysicsShell.h"
#include "xrserver_objects_alife_items.h"
#include "ai_sounds.h"

#include "Level.h"
#include "Include/xrRender/Kinematics.h"
#include "xrEngine/camerabase.h"
#include "xrEngine/xr_collide_form.h"
#include "inventory.h"
#include "game_base_space.h"

#include "UIGameCustom.h"
#include "CustomOutfit.h"
#include "ActorHelmet.h"

static const float		TIME_2_HIDE = 5.f;
static const float		TORCH_INERTION_CLAMP = PI_DIV_6;
static const float		TORCH_INERTION_SPEED_MAX = 7.5f;
static const float		TORCH_INERTION_SPEED_MIN = 0.5f;
static		 Fvector	TORCH_OFFSET = { -0.2f,+0.1f,-0.3f };
static const Fvector	OMNI_OFFSET = { -0.2f,+0.1f,-0.1f };
static const float		OPTIMIZATION_DISTANCE = 100.f;

static bool stalker_use_dynamic_lights = false;

CTorch::CTorch(void) : m_current_battery_state(0)
{
	light_render = GEnv.Render->light_create();
	light_render->set_type(IRender_Light::SPOT);
	light_render->set_shadow(true);
	light_omni = GEnv.Render->light_create();
	light_omni->set_type(IRender_Light::POINT);
	light_omni->set_shadow(false);

	m_switched_on = false;
	glow_render = GEnv.Render->glow_create();
	lanim = 0;
	time2hide = 0;
	fBrightness = 1.f;

	m_RangeMax = 20.f;
	m_RangeCurve = 20.f;

	m_prev_hp.set(0, 0);
	m_delta_h = 0;

	m_battery_state = 0.f;
	fUnchanreRate = 0.f;
}

CTorch::~CTorch(void)
{
	light_render.destroy();
	light_omni.destroy();
	glow_render.destroy();
	HUD_SOUND_ITEM::DestroySound(m_FlashlightSwitchSnd);
	sndBreaking.destroy();
}

inline bool CTorch::can_use_dynamic_lights()
{
	if (!H_Parent())
		return				(true);

	CInventoryOwner* owner = smart_cast<CInventoryOwner*>(H_Parent());
	if (!owner)
		return				(true);

	return					(owner->can_use_dynamic_lights());
}

void CTorch::Load(LPCSTR section)
{
	inherited::Load(section);
	light_trace_bone = pSettings->r_string(section, "light_trace_bone");
	HUD_SOUND_ITEM::LoadSound(section, "snd_flashlight_switch_on", m_FlashlightSwitchSnd, SOUND_TYPE_ITEM_USING);
	m_battery_duration = pSettings->r_u32(section, "battery_duration");
	if (pSettings->line_exist(section, "break_sound"))
		sndBreaking.create(pSettings->r_string(section, "break_sound"), st_Effect, sg_SourceType);

	Settings_from_ltx = READ_IF_EXISTS(pSettings, r_u8, section, "settings_from_ltx", 0);

	//tatarinrafa: Добавил возможность прописать это в конфиге
	if (Settings_from_ltx == 1)
	{
		LoadLightSettings(pSettings_not_const, section);
	}

	fUnchanreRate = READ_IF_EXISTS(pSettings, r_float, section, "uncharge_rate", 20.f);
	//lets give some more realistics to batteries. Now found tourches will have 70 to 100 precent battery status 
	float rondo = ::Random.randF(0.7f, 1.0f);
	m_battery_state = m_battery_duration * rondo;
}

void CTorch::LoadLightSettings(CInifile* ini, LPCSTR section)
{

	IKinematics* K = smart_cast<IKinematics*>(Visual());
	lanim = LALib.FindItem(READ_IF_EXISTS(ini, r_string, section, "color_animator", "nill"));
	guid_bone = K->LL_BoneID(ini->r_string(section, "guide_bone"));
	VERIFY(guid_bone != BI_NONE);

	Fcolor clr = ini->r_fcolor(section, "color_r2");
	fBrightness = clr.intensity();
	m_RangeMax = ini->r_float(section, "range_max_r2");
	m_RangeCurve = ini->r_float(section, "range_curve");

	light_render->set_color(clr);
	light_render->set_range(m_RangeMax);

	Fcolor clr_o = ini->r_fcolor(section, "omni_color_r2");
	float range_o = ini->r_float(section, "omni_range_r2");
	light_omni->set_color(clr_o);
	light_omni->set_range(range_o);

	light_render->set_cone(deg2rad(ini->r_float(section, "spot_angle")));
	light_render->set_texture(ini->r_string(section, "spot_texture"));

	glow_render->set_texture(ini->r_string(section, "glow_texture"));
	glow_render->set_color(clr);
	glow_render->set_radius(ini->r_float(section, "glow_radius"));
}

void CTorch::Switch()
{
	if (OnClient()) return;
	bool bActive = !m_switched_on;
	Switch(bActive);
}

void CTorch::Broke()
{
	if (OnClient()) return;
	if (m_switched_on)
	{
		sndBreaking.play_at_pos(0, Position(), false);
		Switch(false);
	}
}

void CTorch::SetBatteryStatus(u32 val)
{
	Msg("SetBatteryStatus = [%d], [%d], [%d], [%d], [%d], [%f]", m_current_battery_state, val, m_battery_duration, m_current_battery_state - val, (m_current_battery_state - val)/m_battery_duration*100, (m_current_battery_state - val)/m_battery_duration);
	m_battery_state = val;
	float condition = 1.f * m_battery_state / m_battery_duration;
	SetCondition(condition);
}

void CTorch::Switch(bool light_on)
{
	CActor* pA = smart_cast<CActor*>(H_Parent());
	m_actor_item = (pA) ? true : false;
	if (light_on && m_battery_state < 0 && m_actor_item)//!m_current_battery_state && m_actor_item)
	{
		light_on = false;

        StaticDrawableWrapper* _s = CurrentGameUI()->AddCustomStatic("torch_battery_low", true);
		_s->m_endTime = Device.fTimeGlobal + 3.0f;// 3sec
	}
	m_switched_on = light_on;
	if (can_use_dynamic_lights())
	{
		light_render->set_active(light_on);
		if (!pA)light_omni->set_active(light_on);
	}
	glow_render->set_active(light_on);

	if (*light_trace_bone)
	{
		IKinematics* pVisual = smart_cast<IKinematics*>(Visual()); VERIFY(pVisual);
		u16 bi = pVisual->LL_BoneID(light_trace_bone);

		pVisual->LL_SetBoneVisible(bi, light_on, TRUE);
		pVisual->CalculateBones();

		pVisual->LL_SetBoneVisible(bi, light_on, TRUE); //hack
	}
	if (m_switched_on && m_actor_item)
		HUD_SOUND_ITEM::PlaySound(m_FlashlightSwitchSnd, pA->Position(), pA, true, false);
}

BOOL CTorch::net_Spawn(CSE_Abstract* DC)
{
	CSE_Abstract* e = (CSE_Abstract*)(DC);
	CSE_ALifeItemTorch* torch = smart_cast<CSE_ALifeItemTorch*>(e);
	R_ASSERT(torch);
	cNameVisual_set(torch->get_visual());

    R_ASSERT(!GetCForm());
    R_ASSERT(smart_cast<IKinematics*>(Visual()));
    CForm = new CCF_Skeleton(this);

	if (!inherited::net_Spawn(DC))
		return				(FALSE);

	//tatarinrafa: Добавил возможность прописать это в конфиге
	if (Settings_from_ltx == 0)
	{
		IKinematics* K = smart_cast<IKinematics*>(Visual());
		CInifile* pUserData = K->LL_UserData();
		R_ASSERT3(pUserData, "Empty Torch user data!", torch->get_visual());

		LoadLightSettings(pUserData, "torch_definition");
	}

	//включить/выключить фонарик
	Switch(torch->m_active);
	VERIFY(!torch->m_active || (torch->ID_Parent != 0xffff));

	m_delta_h = PI_DIV_2 - atan((m_RangeMax * 0.5f) / _abs(TORCH_OFFSET.x));

	m_current_battery_state = torch->m_battery_state;
	return					(TRUE);
}

void CTorch::net_Destroy()
{
	Switch(false);
	inherited::net_Destroy();
}

void CTorch::OnH_A_Chield()
{
	inherited::OnH_A_Chield();
	m_focus.set(Position());
}

void CTorch::OnH_B_Independent(bool just_before_destroy)
{
	inherited::OnH_B_Independent(just_before_destroy);
	time2hide = TIME_2_HIDE;

	Switch(false);
}

void CTorch::Recharge(void)
{
	m_battery_state = m_battery_duration;
	SetCondition(1.f);
}

void CTorch::UpdateBattery(void)
{
	if (m_switched_on)
	{

		float minus = fUnchanreRate * Device.fTimeDelta;
		Msg("UpdateBattery %f %f", m_battery_state, minus);
		m_battery_state -= minus;
		Msg("UpdateBattery %f", m_battery_state);

		float condition = 1.f * m_battery_state / m_battery_duration;
		SetCondition(condition);
		Msg("UpdateBattery condition %f", condition);

		float rangeCoef = atan(m_RangeCurve * m_battery_state / m_battery_duration) / PI_DIV_2;
		clamp(rangeCoef, 0.f, 1.f);
		float range = m_RangeMax * rangeCoef;
	    Msg("set_range [%f]", range);
		light_render->set_range(range);
		m_delta_h = PI_DIV_2 - atan((range * 0.5f) / _abs(TORCH_OFFSET.x));

		if (m_battery_state < 0)
		{
			Switch(false);
			return;
		}
	}
}

void CTorch::UpdateCL()
{
    inherited::UpdateCL();

    if (!m_switched_on)
        return;

    CBoneInstance& BI = smart_cast<IKinematics*>(Visual())->LL_GetBoneInstance(guid_bone);
    Fmatrix M;

    if (H_Parent())
    {
        CActor* actor = smart_cast<CActor*>(H_Parent());
        if (actor)
            smart_cast<IKinematics*>(H_Parent()->Visual())->CalculateBones_Invalidate();

        if (H_Parent()->XFORM().c.distance_to_sqr(Device.vCameraPosition) < _sqr(OPTIMIZATION_DISTANCE))
        {
            // near camera
            smart_cast<IKinematics*>(H_Parent()->Visual())->CalculateBones();
            M.mul_43(XFORM(), BI.mTransform);
        }
        else
        {
            // approximately the same
            M = H_Parent()->XFORM();
            H_Parent()->Center(M.c);
            M.c.y += H_Parent()->Radius() * 2.f / 3.f;
        }

        if (actor)
        {
            if (actor->active_cam() == eacLookAt)
            {
                m_prev_hp.x = angle_inertion_var(m_prev_hp.x, -actor->cam_Active()->yaw, TORCH_INERTION_SPEED_MIN,
                    TORCH_INERTION_SPEED_MAX, TORCH_INERTION_CLAMP, Device.fTimeDelta);
                m_prev_hp.y = angle_inertion_var(m_prev_hp.y, -actor->cam_Active()->pitch, TORCH_INERTION_SPEED_MIN,
                    TORCH_INERTION_SPEED_MAX, TORCH_INERTION_CLAMP, Device.fTimeDelta);
            }
            else
            {
                m_prev_hp.x = angle_inertion_var(m_prev_hp.x, -actor->cam_FirstEye()->yaw, TORCH_INERTION_SPEED_MIN,
                    TORCH_INERTION_SPEED_MAX, TORCH_INERTION_CLAMP, Device.fTimeDelta);
                m_prev_hp.y = angle_inertion_var(m_prev_hp.y, -actor->cam_FirstEye()->pitch, TORCH_INERTION_SPEED_MIN,
                    TORCH_INERTION_SPEED_MAX, TORCH_INERTION_CLAMP, Device.fTimeDelta);
            }

            Fvector dir, right, up;
            dir.setHP(m_prev_hp.x + m_delta_h, m_prev_hp.y);
            Fvector::generate_orthonormal_basis_normalized(dir, up, right);

            if (true)
            {
                Fvector offset = M.c;
                offset.mad(M.i, TORCH_OFFSET.x);
                offset.mad(M.j, TORCH_OFFSET.y);
                offset.mad(M.k, TORCH_OFFSET.z);
                light_render->set_position(offset);
                light_render->set_volumetric(false);

                offset = M.c;
                offset.mad(M.i, OMNI_OFFSET.x);
                offset.mad(M.j, OMNI_OFFSET.y);
                offset.mad(M.k, OMNI_OFFSET.z);
                light_omni->set_position(offset);

            }
            glow_render->set_position(M.c);

            if (true)
            {
                light_render->set_rotation(dir, right);

                if (true)
                {
                    light_omni->set_rotation(dir, right);
                }
            }
            glow_render->set_direction(dir);

        }
        else
        {
            if (can_use_dynamic_lights())
            {
                light_render->set_position(M.c);
                light_render->set_rotation(M.k, M.i);

                Fvector offset = M.c;
                offset.mad(M.i, OMNI_OFFSET.x);
                offset.mad(M.j, OMNI_OFFSET.y);
                offset.mad(M.k, OMNI_OFFSET.z);
                light_omni->set_position(M.c);
                light_omni->set_rotation(M.k, M.i);
            }

            glow_render->set_position(M.c);
            glow_render->set_direction(M.k);
        }
    }
    else
    {
        if (getVisible() && m_pPhysicsShell)
        {
            M.mul(XFORM(), BI.mTransform);

            m_switched_on = false;
            light_render->set_active(false);
            light_omni->set_active(false);
            glow_render->set_active(false);
        }
    }

    if (!m_switched_on)
        return;

    if (!lanim)
        return;

    int frame;
    u32 clr = lanim->CalculateBGR(Device.fTimeGlobal, frame);

    Fcolor fclr;
    fclr.set((float)color_get_B(clr), (float)color_get_G(clr), (float)color_get_R(clr), 1.f);
    fclr.mul_rgb(fBrightness / 255.f);
    if (can_use_dynamic_lights())
    {
        light_render->set_color(fclr);
        light_omni->set_color(fclr);
    }
    glow_render->set_color(fclr);
}

void CTorch::create_physic_shell()
{
	CPhysicsShellHolder::create_physic_shell();
}

void CTorch::activate_physic_shell()
{
	CPhysicsShellHolder::activate_physic_shell();
}

void CTorch::setup_physic_shell()
{
	CPhysicsShellHolder::setup_physic_shell();
}

void CTorch::net_Export(NET_Packet& P)
{
	inherited::net_Export(P);
	//	P.w_u8						(m_switched_on ? 1 : 0);


	BYTE F = 0;
	F |= (m_switched_on ? eTorchActive : 0);

	const CActor* pA = smart_cast<const CActor*>(H_Parent());
	if (pA)
	{
		if (pA->attached(this))
			F |= eAttached;
	}
	P.w_u8(F);
	P.w_u16(m_current_battery_state);
}

void CTorch::net_Import(NET_Packet& P)
{
	inherited::net_Import(P);

	BYTE F = P.r_u8();
	bool new_m_switched_on = !!(F & eTorchActive);
	bool new_m_bNightVisionOn = !!(F & eNightVisionActive);

	if (new_m_switched_on != m_switched_on)			Switch(new_m_switched_on);

	m_current_battery_state = P.r_u16();
}


void CTorch::save(NET_Packet& output_packet)
{
	inherited::save(output_packet);
	save_data(m_battery_state, output_packet);

}

void CTorch::load(IReader& input_packet)
{
	inherited::load(input_packet);
	load_data(m_battery_state, input_packet);
}

bool CTorch::can_be_attached() const
{
	const CActor* pA = smart_cast<const CActor*>(H_Parent());
	if (pA)
		return pA->inventory().InSlot(this);
	else
		return true;
}

void CTorch::afterDetach()
{
	inherited::afterDetach();
	Switch(false);
}

void CTorch::renderable_Render()
{
	inherited::renderable_Render();
}

u32 CTorch::GetBatteryLifetime() const
{
	return m_battery_duration;
}

u32 CTorch::GetBatteryStatus() const
{
	return m_battery_state;
}

bool CTorch::IsSwitchedOn() const
{
	return m_switched_on;
}