#include "pch_script.h"

#include "dogsoc.h"
#include "xrScriptEngine/ScriptExporter.hpp"

using namespace luabind;

SCRIPT_EXPORT(
    CAI_Dogsoc, (CGameObject), { module(luaState)[class_<CAI_Dogsoc, CGameObject>("CAI_Dogsoc").def(constructor<>())]; });
