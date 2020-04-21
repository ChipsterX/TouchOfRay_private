////////////////////////////////////////////////////////////////////////////
// Module : ide.hpp
// Created : 11.12.2007
// Modified : 29.12.2007
// Author : Dmitriy Iassenev
// Description : editor ide function
////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Include/editor/ide.hpp"

namespace XRay { namespace Editor { } } 

inline XRay::Editor::ide_base& ide()
{
    VERIFY(Device.editor());
    return (*Device.editor());
}
