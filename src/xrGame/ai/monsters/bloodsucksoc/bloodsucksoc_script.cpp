#include "pch_script.h"
#include "bloodsucksoc.h"
#include "xrScriptEngine/ScriptExporter.hpp"

using namespace luabind;

SCRIPT_EXPORT(CAI_Bloodsucksoc, (CGameObject), 
    {
        module(luaState)[class_<CAI_Bloodsucksoc, CGameObject>("CAI_Bloodsucksoc")
            .def(constructor<>())
    ];
});
