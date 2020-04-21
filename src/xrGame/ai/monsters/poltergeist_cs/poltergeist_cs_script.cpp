#include "pch_script.h"

#include "poltergeist_cs.h"
#include "xrScriptEngine/ScriptExporter.hpp"

using namespace luabind;

SCRIPT_EXPORT(CPoltergeist_cs, (CGameObject),
    { module(luaState)[class_<CPoltergeist_cs, CGameObject>("CPoltergeist_cs").def(constructor<>())]; });

