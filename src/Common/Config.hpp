#pragma once

//--' Для отделения версий шейдеров
#define PATH_SHADERS_CACHE_DX9  strconcat(sizeof(file), file, "shaders\\DX9\\", name, ".", extension, "\\", sh_name)
#define PATH_SHADERS_CACHE_DX10 strconcat(sizeof(file), file, "shaders\\DX10\\", name, ".", extension, "\\", sh_name)
#define PATH_SHADERS_CACHE_DX11 strconcat(sizeof(file), file, "shaders\\DX11\\", name, ".", extension, "\\", sh_name)
//--' -2

//--' Для не совместимости со старыми сейвами
#define ALIFE_VERSION 0x0892 
//--' -1

//--' Вылета не будет, просто в лог напишем
#define ASSERT_FMT_DBG(cond, ...) do { if (!(cond)) Msg(__VA_ARGS__); } while (0) 
//--' 0

//--' Смерть гг от первого лица
#define FP_DEATH
//--' 1

//--' Отключить летающие предметы возле аномалии (?)
#define COC_DISABLE_ANOMALY_AND_ITEMS_PLANNER
//--' 2

//--' For extending the life of the actor to fake death or do other tasks that need to happen before actor is dead
//#define ACTOR_BEFORE_DEATH_CALLBACK 
//--' 3

//--' Правка перепаковщик патронов на пояс
#define AMMO_REPACK
//--' 4

//--' Отключить использование STCoP 3.3
//#define DISABLE_STCOP_3_3
//--' 5

//--' Отключить всю систему SSAO в рендерах
#define DISABLE_SSAO_SYSTEM_R4
#define DISABLE_SSAO_SYSTEM_R3
#define DISABLE_SSAO_SYSTEM_R2
//--' 6

//--' Новая система загрузки текстур
#define NEW_LOADING_TEXTURES
//--' 7

//--' Фонарик на батарейках
#define BATTERY_STATUS
//--' 8
