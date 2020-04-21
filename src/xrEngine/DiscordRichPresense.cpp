#include "stdafx.h"
#include "DiscordRichPresense.h"
#include "DiscordRichPresense/discord_register.h"
#include "DiscordRichPresense/discord_rpc.h"
#include "x_ray.h"
#include "IGame_Level.h"

EI game_value_discord_status;

const char* ConvertToUTF8(const char* pStr)
{
    const int pStrLen = (int)strlen(pStr);
    wchar_t utfRusStr[64] = {0};
    MultiByteToWideChar(CP_ACP, 0, pStr, pStrLen, utfRusStr, sizeof(utfRusStr));

    char OutStr[64] = {0};
    WideCharToMultiByte(CP_UTF8, 0, utfRusStr, sizeof(utfRusStr), OutStr, sizeof(OutStr), 0, 0);
    return OutStr;
};

ENGINE_API xrDiscordPresense g_discord;

void xrDiscordPresense::Initialize()
{
	if (game_value_discord_status)
	{
		DiscordEventHandlers nullHandlers;
		ZeroMemory(&nullHandlers, sizeof(nullHandlers));
		Discord_Initialize("618836258713632768", &nullHandlers, TRUE, nullptr);
		bInitialize = true;
	}
}

void xrDiscordPresense::Shutdown()
{
    if (bInitialize)
    {
        Discord_Shutdown();
        bInitialize = false;
    }
}

void xrDiscordPresense::SetStatus(StatusId status)
{
    if (!bInitialize)
        return;

    DiscordRichPresence presenseInfo;
    memset(&presenseInfo, 0, sizeof(presenseInfo));

    StatusId realStatus = status;
    if (status == StatusId::In_Game)
    {
        if (pApp->Level_Current < pApp->Levels.size())
        {
            CApplication::sLevelInfo& LevelInfo = pApp->Levels[pApp->Level_Current];
            if (xr_strcmp(LevelInfo.name, "zaton") == 0)
            {
                realStatus = StatusId::zaton;
            }
            if (xr_strcmp(LevelInfo.name, "jupiter") == 0)
            {
                realStatus = StatusId::jupiter;
            }
            if (xr_strcmp(LevelInfo.name, "pripyat") == 0)
            {
                realStatus = StatusId::pripyat;
            }

            if (xr_strcmp(LevelInfo.name, "k00_marsh") == 0)
            {
                realStatus = StatusId::k00_marsh;
            }

            if (xr_strcmp(LevelInfo.name, "l01_escape") == 0)
            {
                realStatus = StatusId::l01_escape;
            }

            if (xr_strcmp(LevelInfo.name, "l02_garbage") == 0)
            {
                realStatus = StatusId::l02_garbage;
            }

            if (xr_strcmp(LevelInfo.name, "l03_agroprom") == 0)
            {
                realStatus = StatusId::l03_agroprom;
            }

            if (xr_strcmp(LevelInfo.name, "k01_darkscape") == 0)
            {
                realStatus = StatusId::k01_darkscape;
            }

            if (xr_strcmp(LevelInfo.name, "l04_darkvalley") == 0)
            {
                realStatus = StatusId::l04_darkvalley;
            }

            if (xr_strcmp(LevelInfo.name, "l05_bar") == 0)
            {
                realStatus = StatusId::l05_bar;
            }

            if (xr_strcmp(LevelInfo.name, "l06_rostok") == 0)
            {
                realStatus = StatusId::l06_rostok;
            }

            if (xr_strcmp(LevelInfo.name, "l07_military") == 0)
            {
                realStatus = StatusId::l07_military;
            }

            if (xr_strcmp(LevelInfo.name, "l08_yantar") == 0)
            {
                realStatus = StatusId::l08_yantar;
            }

            if (xr_strcmp(LevelInfo.name, "l09_deadcity") == 0)
            {
                realStatus = StatusId::l09_deadcity;
            }

            if (xr_strcmp(LevelInfo.name, "l10_limansk") == 0)
            {
                realStatus = StatusId::l10_limansk;
            }

            if (xr_strcmp(LevelInfo.name, "l10_radar") == 0)
            {
                realStatus = StatusId::l10_radar;
            }

            if (xr_strcmp(LevelInfo.name, "l10_red_forest") == 0)
            {
                realStatus = StatusId::l10_red_forest;
            }

            if (xr_strcmp(LevelInfo.name, "l11_pripyat") == 0)
            {
                realStatus = StatusId::l11_pripyat;
            }

            if (xr_strcmp(LevelInfo.name, "l12_stancia") == 0)
            {
                realStatus = StatusId::l12_stancia;
            }

            if (xr_strcmp(LevelInfo.name, "l12_stancia_2") == 0)
            {
                realStatus = StatusId::l12_stancia_2;
            }

            if (xr_strcmp(LevelInfo.name, "l13_generators") == 0)
            {
                realStatus = StatusId::l13_generators;
            }

            if (xr_strcmp(LevelInfo.name, "l03u_agr_underground") == 0)
            {
                realStatus = StatusId::l03u_agr_underground;
            }

            if (xr_strcmp(LevelInfo.name, "l04u_labx18") == 0)
            {
                realStatus = StatusId::l04u_labx18;
            }

            if (xr_strcmp(LevelInfo.name, "l08u_brainlab") == 0)
            {
                realStatus = StatusId::l08u_brainlab;
            }

            if (xr_strcmp(LevelInfo.name, "l12u_sarcofag") == 0)
            {
                realStatus = StatusId::l12u_sarcofag;
            }

            if (xr_strcmp(LevelInfo.name, "l12u_control_monolith") == 0)
            {
                realStatus = StatusId::l12u_control_monolith;
            }

            if (xr_strcmp(LevelInfo.name, "l13u_warlab") == 0)
            {
                realStatus = StatusId::l13u_warlab;
            }

            if (xr_strcmp(LevelInfo.name, "jupiter_underground") == 0)
            {
                realStatus = StatusId::jupiter_underground;
            }

            if (xr_strcmp(LevelInfo.name, "labx8") == 0)
            {
                realStatus = StatusId::labx8;
            }

            if (xr_strcmp(LevelInfo.name, "k02_trucks_cemetery") == 0)
            {
                realStatus = StatusId::k02_trucks_cemetery;
            }

            if (xr_strcmp(LevelInfo.name, "fake_start") == 0)
            {
                realStatus = StatusId::fake_start;
            }
        }
    }

    switch (realStatus)
    {
    case StatusId::In_Game:
        presenseInfo.largeImageKey = "bttr_logo";
        presenseInfo.state = "В игре";
        break;

    case StatusId::zaton:
        presenseInfo.largeImageKey = "zaton";
        presenseInfo.state = "В игре";
        presenseInfo.details = "Локация: Затон";
        break;

    case StatusId::jupiter:
        presenseInfo.largeImageKey = "jupiter";
        presenseInfo.state = "В игре";
        presenseInfo.details = "Локация: Юпитре";
        break;

    case StatusId::pripyat:
        presenseInfo.largeImageKey = "pripyat";
        presenseInfo.state = "В игре";
        presenseInfo.details = "Локация: Восточная Припять";
        break;

    case StatusId::k00_marsh:
        presenseInfo.largeImageKey = "boloto";
        presenseInfo.state = "В игре";
        presenseInfo.details = "Локация: Болота";
        break;

    case StatusId::l01_escape:
        presenseInfo.largeImageKey = "cordon";
        presenseInfo.state = "В игре";
        presenseInfo.details = "Локация: Кордон";
        break;

    case StatusId::l02_garbage:
        presenseInfo.largeImageKey = "garbage";
        presenseInfo.state = "В игре";
        presenseInfo.details = "Локация: Свалка";
        break;

    case StatusId::l03_agroprom:
        presenseInfo.largeImageKey = "agroprom";
        presenseInfo.state = "В игре";
        presenseInfo.details = "Локация: Агропром";
        break;

    case StatusId::k01_darkscape:
        presenseInfo.largeImageKey = "darkscape";
        presenseInfo.state = "В игре";
        presenseInfo.details = "Локация: Темная лощина";
        break;

    case StatusId::l04_darkvalley:
        presenseInfo.largeImageKey = "darkvalley";
        presenseInfo.state = "В игре";
        presenseInfo.details = "Локация: Темная долина";
        break;

    case StatusId::l05_bar:
        presenseInfo.largeImageKey = "bar";
        presenseInfo.state = "В игре";
        presenseInfo.details = "Локация: Бар 100 рентген";
        break;

    case StatusId::l06_rostok:
        presenseInfo.largeImageKey = "rostok";
        presenseInfo.state = "В игре";
        presenseInfo.details = "Локация: Дикая территория";
        break;

    case StatusId::l07_military:
        presenseInfo.largeImageKey = "military";
        presenseInfo.state = "В игре";
        presenseInfo.details = "Локация: Армейские склады";
        break;

    case StatusId::l08_yantar:
        presenseInfo.largeImageKey = "yantar";
        presenseInfo.state = "В игре";
        presenseInfo.details = "Локация: Янтарь";
        break;

    case StatusId::l09_deadcity:
        presenseInfo.largeImageKey = "deadcity";
        presenseInfo.state = "В игре";
        presenseInfo.details = "Локация: Мёртвый город";
        break;

    case StatusId::l10_limansk:
        presenseInfo.largeImageKey = "limansk";
        presenseInfo.state = "В игре";
        presenseInfo.details = "Локация: Лиманск";
        break;

    case StatusId::l10_radar:
        presenseInfo.largeImageKey = "radar";
        presenseInfo.state = "В игре";
        presenseInfo.details = "Локация: Радар";
        break;

    case StatusId::l10_red_forest:
        presenseInfo.largeImageKey = "red_forest";
        presenseInfo.state = "В игре";
        presenseInfo.details = "Локация: Рыжий лес";
        break;

    case StatusId::l11_pripyat:
        presenseInfo.largeImageKey = "pripyat_soc";
        presenseInfo.state = "В игре";
        presenseInfo.details = "Локация: Центральная Припять";
        break;

    case StatusId::l12_stancia:
        presenseInfo.largeImageKey = "stancia2";
        presenseInfo.state = "В игре";
        presenseInfo.details = "Локация: ЧАЭС";
        break;

    case StatusId::l12_stancia_2:
        presenseInfo.largeImageKey = "stancia2";
        presenseInfo.state = "В игре";
        presenseInfo.details = "Локация: ЧАЭС-2";
        break;

    case StatusId::l13_generators:
        presenseInfo.largeImageKey = "generators";
        presenseInfo.state = "В игре";
        presenseInfo.details = "Локация: Генераторы";
        break;

    case StatusId::l03u_agr_underground:
        presenseInfo.largeImageKey = "agr_underground";
        presenseInfo.state = "В игре";
        presenseInfo.details = "Локация: Подземелья НИИ Агропром";
        break;

    case StatusId::l04u_labx18:
        presenseInfo.largeImageKey = "labx18";
        presenseInfo.state = "В игре";
        presenseInfo.details = "Локация: Лаборатория Х-18";
        break;

    case StatusId::l08u_brainlab:
        presenseInfo.largeImageKey = "brainlab";
        presenseInfo.state = "В игре";
        presenseInfo.details = "Локация: Лаборатория Х-10";
        break;

    case StatusId::l12u_sarcofag:
        presenseInfo.largeImageKey = "sarcofag";
        presenseInfo.state = "В игре";
        presenseInfo.details = "Локация: Саркофаг";
        break;

    case StatusId::l12u_control_monolith:
        presenseInfo.largeImageKey = "control_monolith";
        presenseInfo.state = "В игре";
        presenseInfo.details = "Локация: Управление монолитом";
        break;

    case StatusId::l13u_warlab:
        presenseInfo.largeImageKey = "warlab";
        presenseInfo.state = "В игре";
        presenseInfo.details = "Локация: Варлаб";
        break;

    case StatusId::jupiter_underground:
        presenseInfo.largeImageKey = "jupiter_underground2";
        presenseInfo.state = "В игре";
        presenseInfo.details = "Локация: Путепровод Припять-1";
        break;

    case StatusId::labx8:
        presenseInfo.largeImageKey = "labx8";
        presenseInfo.state = "В игре";
        presenseInfo.details = "Локация: Лаборатория Х-8";
        break;

    case StatusId::k02_trucks_cemetery:
        presenseInfo.largeImageKey = "trucks_cemetery";
        presenseInfo.state = "В игре";
        presenseInfo.details = "Локация: Кладбище техники";
        break;

    case StatusId::fake_start:
        presenseInfo.largeImageKey = "bttr_logo";
        presenseInfo.state = "Инициализация";
        presenseInfo.details = "Загрузка локации...";
        break;

    case StatusId::Initialization:
        presenseInfo.largeImageKey = "bttr_logo";
        presenseInfo.state = "Загрузка";
        presenseInfo.details = "Инициализация сервера...";
        break;

//    case StatusId::Load:
//        presenseInfo.largeImageKey = "bttr_logo";
//        presenseInfo.state = "Загрузка";
//        presenseInfo.details = "Локация: Синхронизация сервера...";
//        break;

    case StatusId::Menu:

    default:
        presenseInfo.state = "Пауза";
        presenseInfo.details = "В главном меню";
        presenseInfo.largeImageKey = "menuimagekey";
        break;
    }
#if defined(_WIN32) && !defined(XR_X64)
    presenseInfo.smallImageKey = "ico3_512_x32";
#else
    presenseInfo.smallImageKey = "ico3_512";
#endif
    presenseInfo.startTimestamp = time(0); // + 5 * 60;
    presenseInfo.smallImageText = "S.T.A.L.K.E.R.: Back to the roots";
    presenseInfo.largeImageText = "Engine: Touch of ray 4.3";
    Discord_UpdatePresence(&presenseInfo);
}

xrDiscordPresense::~xrDiscordPresense() { Shutdown(); }
