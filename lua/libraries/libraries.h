#ifndef LIBRARIES_H
#define LIBRARIES_H

#include <lua.hpp>
#include "bit32/bit32.h"

namespace Lua
{
    namespace Libraries
    {
        int luaopen_libraries(lua_State* l);
    }
}

#endif // LIBRARIES_H
