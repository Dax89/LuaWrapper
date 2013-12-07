#ifndef BIT32_H
#define BIT32_H

#include <lua.hpp>
#include <sstream>

namespace Lua
{
    namespace Libraries
    {
        namespace Bit32
        {
            #define BIT32_LIBRARY_NAME "bit32"

            int b_and(lua_State* l);
            int b_or(lua_State* l);
            int b_not(lua_State* l);
            int b_mod(lua_State* l);
            int b_lshift(lua_State* l);
            int b_rshift(lua_State* l);
            int b_compl2(lua_State* l);

            int luaopen_bit32(lua_State* l);
        }
    }
}

#endif // BIT32_H
