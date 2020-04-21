#include "pch_script.h"
#include "bocs.h"
#include "xrScriptEngine/ScriptExporter.hpp"

using namespace luabind;

SCRIPT_EXPORT(
    CAI_CBocs, (CGameObject), { module(luaState)[class_<CAI_CBocs, CGameObject>("CAI_CBocs").def(constructor<>())]; });