#include "stdafx.h"
#include "UIMainIngameWnd.h"
#include "UIMotionIcon.h"
#include "UIXmlInit.h"

GetU32Int type_hud_token;

const LPCSTR MOTION_ICON_XML_COC    = "motion_icon\\maingame_motion_icon_coc.xml";
const LPCSTR MOTION_ICON_XML_COP    = "motion_icon\\maingame_motion_icon_cop.xml";
const LPCSTR MOTION_ICON_XML_CMP    = "motion_icon\\maingame_motion_icon_cmp.xml";
const LPCSTR MOTION_ICON_XML_STS    = "motion_icon\\maingame_motion_icon_stason.xml";
const LPCSTR MOTION_ICON_XML_NULL   = "motion_icon\\maingame_motion_icon_cop_coc_null.xml";
const LPCSTR MOTION_ICON_XML_SOC    = "motion_icon\\maingame_motion_icon_soc.xml";

CUIMotionIcon* g_pMotionIcon = NULL;

CUIMotionIcon::CUIMotionIcon()
{
    if (type_hud_token == 5)
        m_curren_state = stLast;

    g_pMotionIcon = this;
    m_bchanged = true;
    m_luminosity = 0.0f;
    cur_pos = 0.f;
}

CUIMotionIcon::~CUIMotionIcon() { g_pMotionIcon = NULL; }
void CUIMotionIcon::ResetVisibility()
{
    m_npc_visibility.clear();
    m_bchanged = true;
}

void CUIMotionIcon::Init(Frect const& zonemap_rect)
{
    CUIXml uiXml;
    CUIXmlInit xml_init;

    if (type_hud_token == 5)
    {

        uiXml.Load(CONFIG_PATH, UI_PATH, UI_PATH_DEFAULT, MOTION_ICON_XML_SOC);

        AttachChild(&background_shoc);
        xml_init.InitStatic(uiXml, "background", 0, &background_shoc);

        AttachChild(&m_luminosity_progress);
        xml_init.InitProgressBar(uiXml, "luminosity_progress", 0, &m_luminosity_progress);

        AttachChild(&m_noise_progress);
        xml_init.InitProgressBar(uiXml, "noise_progress", 0, &m_noise_progress);

        AttachChild(&m_states[stNormal]);
        xml_init.InitStatic(uiXml, "state_normal", 0, &m_states[stNormal]);
        m_states[stNormal].Show(false);

        AttachChild(&m_states[stCrouch]);
        xml_init.InitStatic(uiXml, "state_crouch", 0, &m_states[stCrouch]);
        m_states[stCrouch].Show(false);

        AttachChild(&m_states[stCreep]);
        xml_init.InitStatic(uiXml, "state_creep", 0, &m_states[stCreep]);
        m_states[stCreep].Show(false);

        AttachChild(&m_states[stClimb]);
        xml_init.InitStatic(uiXml, "state_climb", 0, &m_states[stClimb]);
        m_states[stClimb].Show(false);

        AttachChild(&m_states[stRun]);
        xml_init.InitStatic(uiXml, "state_run", 0, &m_states[stRun]);
        m_states[stRun].Show(false);

        AttachChild(&m_states[stSprint]);
        xml_init.InitStatic(uiXml, "state_sprint", 0, &m_states[stSprint]);
        m_states[stSprint].Show(false);

        ShowState(stNormal);
    }
    else
    {
        if (type_hud_token == 0)
            uiXml.Load(CONFIG_PATH, UI_PATH, UI_PATH_DEFAULT, MOTION_ICON_XML_COC);

        if (type_hud_token == 1)
            uiXml.Load(CONFIG_PATH, UI_PATH, UI_PATH_DEFAULT, MOTION_ICON_XML_COP);

		if (type_hud_token == 2)
            uiXml.Load(CONFIG_PATH, UI_PATH, UI_PATH_DEFAULT, MOTION_ICON_XML_CMP);

		if (type_hud_token == 3)
            uiXml.Load(CONFIG_PATH, UI_PATH, UI_PATH_DEFAULT, MOTION_ICON_XML_STS);

        if (type_hud_token == 4 || type_hud_token == 5 || type_hud_token == 6 || type_hud_token == 7 || type_hud_token == 8 || type_hud_token == 9)
            uiXml.Load(CONFIG_PATH, UI_PATH, UI_PATH_DEFAULT, MOTION_ICON_XML_NULL);

        xml_init.InitWindow(uiXml, "window", 0, this);
        float rel_sz = uiXml.ReadAttribFlt("window", 0, "rel_size", 1.0f);
        Fvector2 sz;
        Fvector2 pos;
        zonemap_rect.getsize(sz);

        pos.set(sz.x / 2.0f, sz.y / 2.0f);
        SetWndSize(sz);
        SetWndPos(pos);

        float k = UI().get_current_kx();
        sz.mul(rel_sz * k);

        AttachChild(&m_luminosity_progress_shape);
        xml_init.InitProgressShape(uiXml, "luminosity_progress", 0, &m_luminosity_progress_shape);
        m_luminosity_progress_shape.SetWndSize(sz);
        m_luminosity_progress_shape.SetWndPos(pos);

        AttachChild(&m_noise_progress_shape);
        xml_init.InitProgressShape(uiXml, "noise_progress", 0, &m_noise_progress_shape);
        m_noise_progress_shape.SetWndSize(sz);
        m_noise_progress_shape.SetWndPos(pos);
    }
}

void CUIMotionIcon::ShowState(EState state)
{
    if (type_hud_token == 5)
    {
        if (m_curren_state == state)
            return;
        if (m_curren_state != stLast)
        {
            m_states[m_curren_state].Show(false);
            m_states[m_curren_state].Enable(false);
        }
        m_states[state].Show(true);
        m_states[state].Enable(true);

        m_curren_state = state;
    }
}

void CUIMotionIcon::SetNoise(float Pos)
{
    if (type_hud_token == 5 || type_hud_token == 9)
    {
        Pos = clampr(Pos, m_noise_progress.GetRange_min(), m_noise_progress.GetRange_max());
        m_noise_progress.SetProgressPos(Pos);
    }
    else
    {
        if (!IsGameTypeSingle())
            return;
        Pos = clampr(Pos, 0.f, 100.f);
        m_noise_progress_shape.SetPos(Pos / 100.f);
    }
}

void CUIMotionIcon::SetLuminosity(float Pos)
{
    if (type_hud_token == 5 || type_hud_token == 9)
    {
        Pos = clampr(Pos, m_luminosity_progress.GetRange_min(), m_luminosity_progress.GetRange_max());
        m_luminosity = Pos;
    }
    else
    {
        if (!IsGameTypeSingle())
            return;
        m_luminosity = Pos;
    }
}

void CUIMotionIcon::Draw()
{
    if (!IsShown())
        return;

    if (psActorFlags.test(AF_BOOL_DISABLE_MINIMAP))
        return;

    inherited::Draw();
}

void CUIMotionIcon::Update()
{
    if (type_hud_token == 5 || type_hud_token == 9)
    {
        if (m_bchanged)
        {
            m_bchanged = false;
            if (m_npc_visibility.size())
            {
                std::sort(m_npc_visibility.begin(), m_npc_visibility.end());
                SetLuminosity(m_npc_visibility.back().value);
            }
            else
                SetLuminosity(m_luminosity_progress.GetRange_min());
        }
        inherited::Update();

        // m_luminosity_progress
        {
            float len = m_noise_progress.GetRange_max() - m_noise_progress.GetRange_min();
            float cur_pos = m_luminosity_progress.GetProgressPos();
            if (cur_pos != m_luminosity)
            {
                float _diff = _abs(m_luminosity - cur_pos);
                if (m_luminosity > cur_pos)
                {
                    cur_pos += _min(len * Device.fTimeDelta, _diff);
                }
                else
                {
                    cur_pos -= _min(len * Device.fTimeDelta, _diff);
                }
                clamp(cur_pos, m_noise_progress.GetRange_min(), m_noise_progress.GetRange_max());
                m_luminosity_progress.SetProgressPos(cur_pos);
            }
        }
    }
    else
    {
        if (!IsGameTypeSingle())
        {
            inherited::Update();
            return;
        }
		
        if (!IsShown())
            return;
        if (m_bchanged)
        {
            m_bchanged = false;
            if (m_npc_visibility.size())
            {
                std::sort(m_npc_visibility.begin(), m_npc_visibility.end());
                SetLuminosity(m_npc_visibility.back().value);
            }
            else
                SetLuminosity(0.f);
        }
        inherited::Update();

        // m_luminosity_progress
        if (cur_pos != m_luminosity)
        {
            float _diff = _abs(m_luminosity - cur_pos);
            if (m_luminosity > cur_pos)
            {
                cur_pos += _diff * Device.fTimeDelta;
            }
            else
            {
                cur_pos -= _diff * Device.fTimeDelta;
            }
            clamp(cur_pos, 0.f, 100.f);
            m_luminosity_progress_shape.SetPos(cur_pos / 100.f);
        }
    }
}

void SetActorVisibility(u16 who_id, float value)
{
    if (!IsGameTypeSingle())
        return;

    if (g_pMotionIcon && g_pMotionIcon->IsShown())
        g_pMotionIcon->SetActorVisibility(who_id, value);
}

void CUIMotionIcon::SetActorVisibility(u16 who_id, float value)
{
    if (type_hud_token == 5 || type_hud_token == 9)
    {
        float v = float(m_luminosity_progress.GetRange_max() - m_luminosity_progress.GetRange_min());
        value *= v;
        value += m_luminosity_progress.GetRange_min();

        xr_vector<_npc_visibility>::iterator it = std::find(m_npc_visibility.begin(), m_npc_visibility.end(), who_id);

        if (it == m_npc_visibility.end() && value != 0)
        {
            m_npc_visibility.resize(m_npc_visibility.size() + 1);
            _npc_visibility& v = m_npc_visibility.back();
            v.id = who_id;
            v.value = value;
        }
        else if (fis_zero(value))
        {
            if (it != m_npc_visibility.end())
                m_npc_visibility.erase(it);
        }
        else
        {
            (*it).value = value;
        }
    }
    else
    {
		
        if (!IsShown())
            return;

        clamp(value, 0.f, 1.f);
        value *= 100.f;

        xr_vector<_npc_visibility>::iterator it = std::find(m_npc_visibility.begin(), m_npc_visibility.end(), who_id);

        if (it == m_npc_visibility.end() && value != 0)
        {
            m_npc_visibility.resize(m_npc_visibility.size() + 1);
            _npc_visibility& v = m_npc_visibility.back();
            v.id = who_id;
            v.value = value;
        }
        else if (fis_zero(value))
        {
            if (it != m_npc_visibility.end())
                m_npc_visibility.erase(it);
        }
        else
        {
            (*it).value = value;
        }
    }

    m_bchanged = true;
}
