#include "libraries.h"

namespace Lua
{
    namespace Libraries
    {
        int luaopen_libraries(lua_State *l)
        {
            if(LUA_VERSION_NUM < 502)
                Bit32::luaopen_bit32(l); /* Provide a bit32 library from wrapper */

            return 1;
        }
    }
}
