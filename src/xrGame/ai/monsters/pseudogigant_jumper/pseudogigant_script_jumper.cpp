#include "pch_script.h"
#include "pseudo_gigant_jumper.h"
#include "xrScriptEngine/ScriptExporter.hpp"

using namespace luabind;

SCRIPT_EXPORT(CPseudoGigant_jumper, (CGameObject),
	{ module(luaState)[class_<CPseudoGigant_jumper, CGameObject>("CPseudoGigant_jumper").def(constructor<>())]; });

