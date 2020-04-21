#include "pch_script.h"
#include "bloodsuckcs.h"
#include "xrScriptEngine/ScriptExporter.hpp"

using namespace luabind;

SCRIPT_EXPORT(CAI_Bloodsuckcs, (CGameObject),
	{
		module(luaState)[class_<CAI_Bloodsuckcs, CGameObject>("CAI_Bloodsuckcs")
			.def(constructor<>())
	];
	});

