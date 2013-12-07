#include "bit32.h"

namespace Lua
{
    namespace Libraries
    {
        namespace Bit32
        {
            int b_and(lua_State *l)
            {
                lua_Integer c = lua_gettop(l);
                lua_Integer val = lua_tointeger(l, 1);

                for(int i = 2; i <= c; i++)
                    val &= lua_tointeger(l, i);

                lua_pushinteger(l, val);
                return 1;
            }

            int b_or(lua_State* l)
            {
                lua_Integer c = lua_gettop(l);
                lua_Integer val = lua_tointeger(l, 1);

                for(int i = 2; i <= c; i++)
                    val |= lua_tointeger(l, i);

                lua_pushinteger(l, val);
                return 1;
            }

            int b_not(lua_State* l)
            {
                lua_Integer val = lua_tointeger(l, 1);

                lua_pushinteger(l, !val);
                return 1;
            }

            int b_mod(lua_State* l)
            {
                lua_Integer val = lua_tointeger(l, 1);
                lua_Integer mod = lua_tointeger(l, 2);

                lua_pushinteger(l, val % mod);
                return 1;
            }

            int b_lshift(lua_State* l)
            {
                lua_Integer val = lua_tointeger(l, 1);
                lua_Integer displ = lua_tointeger(l, 2);

                lua_pushinteger(l, val << displ);
                return 1;
            }

            int b_rshift(lua_State* l)
            {
                lua_Integer val = lua_tointeger(l, 1);
                lua_Integer displ = lua_tointeger(l, 2);

                lua_pushinteger(l, val >> displ);
                return 1;
            }

            int b_compl2(lua_State *l)
            {
                int i = 0;
                lua_Integer res = 0, val = lua_tointeger(l, 1);
                lua_Integer bitcount = lua_tointeger(l, 2) * 8;

                if(((val >> (bitcount - 1)) & 1) == 0) /* Is Not a Negative Number */
                {
                    lua_pushinteger(l, val);
                    return 1;
                }

                while(i < bitcount)
                {
                    bool b  = (val & (1 << i)) ? false : true; /* 1's Complement */
                    res |= b << i;

                    i++;
                }

                res += 1; /* 2's Complement */
                lua_pushinteger(l, -res);
                return 1;
            }

            const luaL_Reg Bit32Functions[] = { { "band", &Bit32::b_and },
                                                { "bor", &Bit32::b_or },
                                                { "bnot", &Bit32::b_not },
                                                { "bmod", &Bit32::b_mod },
                                                { "lshift", &Bit32::b_lshift },
                                                { "rshift", &Bit32::b_rshift },
                                                { "compl2", &Bit32::b_compl2 },
                                                { nullptr, nullptr } };

            int luaopen_bit32(lua_State *l)
            {
                luaL_register(l, BIT32_LIBRARY_NAME, Bit32Functions);
                lua_pop(l, 1); /* Remove the table from stack */
                return 1;
            }
        }
    }
}
