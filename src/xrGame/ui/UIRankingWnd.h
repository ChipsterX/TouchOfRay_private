////////////////////////////////////////////////////////////////////////////
//	Module 		: UIRankingWnd.h
//	Created 	: 17.01.2008
//	Author		: Evgeniy Sokolov
// 	Modified By	: Alundaio (8/22/2016)
//	Description : UI Ranking window class
////////////////////////////////////////////////////////////////////////////

#pragma once
#include "xrUICore/Windows/UIWindow.h"
#include "xrUICore/Callbacks/UIWndCallback.h"
//#include "UIRankFaction.h"
#include "UIAchievements.h"
#include "UIRankingsCoC.h"

class CUIStatic;
class CUIXml;
class CUIProgressBar;
class CUIFrameLineWnd;
class CUIFrameWindow;
class CUICharacterInfo;
class CUIScrollView;

class CUIRankingWnd : public CUIWindow, public CUIWndCallback
{
    using inherited = CUIWindow;

    CUIFrameWindow* m_background;
    CUIFrameWindow* m_down_background;

    CUIScrollView* m_achievements;
    CUIFrameWindow* m_achievements_background;

    //Alundaio: CoC Rankings
    CUIScrollView* m_coc_ranking;
    CUIScrollView* m_coc_ranking_actor_view;
    CUIFrameWindow* m_coc_ranking_background;
    //-Alundaio


    using ACHIEVES_VEC = xr_vector<CUIAchievements*>;
    ACHIEVES_VEC m_achieves_vec;

    //Alundaio: CoC Rankings
    using RANKINGCOC_VEC = xr_vector<CUIRankingsCoC*>;
    RANKINGCOC_VEC m_coc_ranking_vec;

    CUIRankingsCoC* m_coc_ranking_actor;
    //-Alundaio

    enum
    {
        max_stat_info = 32
    };

    CUITextWnd* m_stat_caption[max_stat_info];
    CUITextWnd* m_stat_info[max_stat_info];

    u32 m_delay;
    u32 m_previous_time;
    u32 m_stat_count;

public:
    CUIRankingWnd();
    virtual ~CUIRankingWnd();

    virtual void Show(bool status);
    virtual void Update();
    virtual void DrawHint();
    virtual void ResetAll();

    void Init();
    void update_info();

protected:
    void add_achievement(CUIXml& xml, shared_str const& faction_id);
    void get_statistic();

}; // class CUIRankingWnd
