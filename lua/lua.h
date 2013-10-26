#ifndef LUA_H
#define LUA_H

#include "luatable.h"
#include "luacfunction.h"
#include "luathread.h"
#include "utils/overloadtable.h"

#define STRICT_LUA "local getinfo, error, rawset, rawget = debug.getinfo, error, rawset, rawget\n" \
                   "local mt = getmetatable(_G)\n" \
                   "if mt == nil then\n" \
                   "  mt = {}\n" \
                   "  setmetatable(_G, mt)\n" \
                   "end\n" \
                   "mt.__declared = {}\n" \
                   "local function what ()\n" \
                   "  local d = getinfo(3, \"S\")\n" \
                   "  return d and d.what or \"C\"\n" \
                   "end\n" \
                   "mt.__newindex = function (t, n, v)\n" \
                   "  if not mt.__declared[n] then\n" \
                   "  local w = what()\n" \
                   "  if w ~= \"main\" and w ~= \"C\" then\n" \
                   "    error(\"assign to undeclared variable '\"..n..\"'\", 2)\n" \
                   "  end\n" \
                   "  mt.__declared[n] = true\n" \
                   "end\n" \
                   "rawset(t, n, v)\n" \
                   "end\n" \
                   "mt.__index = function (t, n)\n" \
                   "  if not mt.__declared[n] and what() ~= \"C\" then\n" \
                   "    error(\"variable '\"..n..\"' is not declared\", 2)\n" \
                   "  end\n" \
                   "  return rawget(t, n)\n" \
                   "end\n"

namespace Lua
{
    namespace Core
    {
        static Utils::OverloadTable globalOverloads;
        lua_CFunction functionDispatcher();
    }

    using namespace std;

    lua_State* luaW_newstate(bool strict = false);    

    // Table Management
    void luaW_settable(lua_State* l, lua_String name, LuaTable::Ptr table);
    LuaTable::Ptr luaW_gettable(lua_State* l, lua_String name);

    // Function Management
    bool luaW_functionexists(lua_State* l, lua_String funcname);
    void luaW_setfunction(lua_State *l, lua_String funcname, const LuaCFunction::Ptr &func);

    template<typename ReturnType, typename... Args> typename LuaCFunctionT<ReturnType, Args...>::Ptr luaW_getfunction(lua_State* l, lua_String funcname)
    {
        lua_getglobal(l, funcname);
        typename LuaCFunctionT<ReturnType, Args...>::Ptr f = LuaCFunctionT<ReturnType, Args...>::create(l, -1);
        lua_pop(l, 1);
        return f;
    }

    // Misc Functions
    void luaW_addsearchpath(lua_State *l, const char* s);
    void luaW_dofile(lua_State *l, const char* file);
    void luaW_dostring(lua_State* l, const char* s);
    void luaW_register(lua_State* l, const LuaTable::Ptr &table, const luaL_Reg *functions);
}
#endif // LUA_H
