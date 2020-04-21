#include "stdafx.h"
#include <dinput.h>
#include "xrEngine/XR_IOConsole.h"
#include "entity_alive.h"
#include "game_sv_single.h"
#include "alife_simulator.h"
#include "alife_simulator_header.h"
#include "xrAICore/Navigation/level_graph.h"
#include "xrEngine/FDemoRecord.h"
#include "Level.h"
#include "xr_level_controller.h"
#include "game_cl_base.h"
#include "stalker_movement_manager_smart_cover.h"
#include "Inventory.h"
#include "xrServer.h"
#include "autosave_manager.h"
#include "xrScriptEngine/script_callback_ex.h"

#include "Actor.h"
#include "HudItem.h"
#include "UIGameCustom.h"
#include "UI/UIDialogWnd.h"
#include "xrEngine/xr_input.h"
#include "xrEngine/xr_object.h"
#include "saved_game_wrapper.h"

#include "Include/xrRender/DebugRender.h"

bool g_bDisableAllInput = false;
extern float g_fTimeFactor;

#define CURRENT_ENTITY() (game ? ((GameID() == eGameIDSingle) ? CurrentEntity() : CurrentControlEntity()) : NULL)

void CLevel::IR_OnMouseWheel(int direction)
{
    if (g_bDisableAllInput)
        return;


    /* avo: script callback */
    if (g_actor)
        g_actor->callback(GameObject::eMouseWheel)(direction);
    /* avo: end */

    if (CurrentGameUI()->IR_UIOnMouseWheel(direction))
        return;
    if (Device.Paused()
            )
        return;

    if (CURRENT_ENTITY())
    {
        IInputReceiver* IR = smart_cast<IInputReceiver*>(smart_cast<CGameObject*>(CURRENT_ENTITY()));
        if (IR)
            IR->IR_OnMouseWheel(direction);
    }
}

static int mouse_button_2_key[] = {MOUSE_1, MOUSE_2, MOUSE_3};

void CLevel::IR_OnMousePress(int btn) { IR_OnKeyboardPress(mouse_button_2_key[btn]); }
void CLevel::IR_OnMouseRelease(int btn) { IR_OnKeyboardRelease(mouse_button_2_key[btn]); }
void CLevel::IR_OnMouseHold(int btn) { IR_OnKeyboardHold(mouse_button_2_key[btn]); }
void CLevel::IR_OnMouseMove(int dx, int dy)
{
    if (g_bDisableAllInput)
        return;


    /* avo: script callback */
    if (g_actor)
        g_actor->callback(GameObject::eMouseMove)(dx, dy);
    /* avo: end */


    if (CurrentGameUI()->IR_UIOnMouseMove(dx, dy))
        return;
    if (Device.Paused()
            )
        return;
    if (CURRENT_ENTITY())
    {
        IInputReceiver* IR = smart_cast<IInputReceiver*>(smart_cast<CGameObject*>(CURRENT_ENTITY()));
        if (IR)
            IR->IR_OnMouseMove(dx, dy);
    }
}

// Обработка нажатия клавиш
extern bool g_block_pause;

// Lain: added TEMP!!!
extern float g_separate_factor;
extern float g_separate_radius;

#include "xrScriptEngine/script_engine.hpp"
#include "ai_space.h"

void CLevel::IR_OnKeyboardPress(int key)
{
    if (Device.dwPrecacheFrame)
        return;

    if (Device.editor() && (pInput->iGetAsyncKeyState(DIK_LALT) || pInput->iGetAsyncKeyState(DIK_RALT)))
        return;

    bool b_ui_exist = !!CurrentGameUI();

    EGameActions _curr = get_binded_action(key);


    /* avo: script callback */
    if (!g_bDisableAllInput && g_actor)
        g_actor->callback(GameObject::eKeyPress)(key);
    /* avo: end */


    if (_curr == kPAUSE)
    {
        if (Device.editor())
            return;

        if (!g_block_pause && IsGameTypeSingle())
        {
			Device.Pause(!Device.Paused(), TRUE, TRUE, "li_pause_key");
        }
        return;
    }

    if (g_bDisableAllInput)
        return;

    switch (_curr)
    {
    case kSCREENSHOT:
        GEnv.Render->Screenshot();
        return;
        break;

    case kCONSOLE:
        Console->Show();
        return;
        break;

    case kQUIT:
    {
        if (b_ui_exist && CurrentGameUI()->TopInputReceiver())
        {
            if (CurrentGameUI()->IR_UIOnKeyboardPress(key))
                return; // special case for mp and main_menu
            CurrentGameUI()->TopInputReceiver()->HideDialog();
        }
        else
        {
            Console->Execute("main_menu");
        }
        return;
    }
    break;
    };

    if (!bReady || !b_ui_exist)
        return;

    if (b_ui_exist && CurrentGameUI()->IR_UIOnKeyboardPress(key))
        return;

    if (Device.Paused()
            )
        return;

    if (game && game->OnKeyboardPress(get_binded_action(key)))
        return;

    luabind::functor<bool> funct;
    if (GEnv.ScriptEngine->functor("level_input.on_key_press", funct))
    {
        if (funct(key, _curr))
            return;
    }

    if (_curr == kQUICK_SAVE && IsGameTypeSingle())
    {
        Console->Execute("save");
        return;
    }
/*	
    if (_curr == kQUICK_LOAD && IsGameTypeSingle())
    {
        string_path saved_game, command;
        strconcat(sizeof(saved_game), saved_game, Core.UserName, " - ", "quicksave");
        if (!CSavedGameWrapper::valid_saved_game(saved_game))
            return;

        strconcat(sizeof(command), command, "load ", saved_game);
        Console->Execute(command);
        return;
    }
*/
    if (bindConsoleCmds.execute(key))
        return;

    if (CURRENT_ENTITY())
    {
        IInputReceiver* IR = smart_cast<IInputReceiver*>(smart_cast<CGameObject*>(CURRENT_ENTITY()));
        if (IR)
            IR->IR_OnKeyboardPress(get_binded_action(key));
    }
}

void CLevel::IR_OnKeyboardRelease(int key)
{
    if (!bReady || g_bDisableAllInput)
        return;


    /* avo: script callback */
    if (g_actor)
        g_actor->callback(GameObject::eKeyRelease)(key);
    /* avo: end */


    if (CurrentGameUI() && CurrentGameUI()->IR_UIOnKeyboardRelease(key))
        return;
    if (game && game->OnKeyboardRelease(get_binded_action(key)))
        return;
    if (Device.Paused()
            )
        return;

    if (CURRENT_ENTITY())
    {
        IInputReceiver* IR = smart_cast<IInputReceiver*>(smart_cast<CGameObject*>(CURRENT_ENTITY()));
        if (IR)
            IR->IR_OnKeyboardRelease(get_binded_action(key));
    }
}

void CLevel::IR_OnKeyboardHold(int key)
{
    if (g_bDisableAllInput)
        return;


    /* avo: script callback */
    if (g_actor)
        g_actor->callback(GameObject::eKeyHold)(key);
    /* avo: end */


    if (CurrentGameUI() && CurrentGameUI()->IR_UIOnKeyboardHold(key))
        return;
    if (Device.Paused()
            )
        return;
    if (CURRENT_ENTITY())
    {
        IInputReceiver* IR = smart_cast<IInputReceiver*>(smart_cast<CGameObject*>(CURRENT_ENTITY()));
        if (IR)
            IR->IR_OnKeyboardHold(get_binded_action(key));
    }
}

void CLevel::IR_OnMouseStop(int , int ) {}
void CLevel::IR_OnActivate()
{
    if (!pInput)
        return;
    int i;
    for (i = 0; i < CInput::COUNT_KB_BUTTONS; i++)
    {
        if (IR_GetKeyState(i))
        {
            EGameActions action = get_binded_action(i);
            switch (action)
            {
            case kFWD:
            case kBACK:
            case kL_STRAFE:
            case kR_STRAFE:
            case kLEFT:
            case kRIGHT:
            case kUP:
            case kDOWN:
            case kCROUCH:
            case kACCEL:
            case kL_LOOKOUT:
            case kR_LOOKOUT:
            case kWPN_FIRE: { IR_OnKeyboardPress(i);
            }
            break;
            };
        };
    }
}
