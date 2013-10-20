#include "lua.h"

namespace Lua
{
    namespace Core
    {
        lua_CFunction functionDispatcher()
        {
            auto f = [](lua_State* l) -> int {
                int argcount = lua_gettop(l);
                string func = lua_tostring(l, lua_upvalueindex(1));
                string mangledfunc = func;

                for(int i = 0; i < argcount; i++)
                {
                    LuaTypes::LuaType t = luaT_typeof(l, i + 1);
                    Utils::Mangler::MangleFunctionFromLuaType()(t, i, &mangledfunc);
                }

                LuaCFunction::Ptr fp = globalOverloads.get(func.c_str(), mangledfunc.c_str());
                fp->push();

                for(int i = 1; i <= argcount; i++)
                    lua_pushvalue(l, i); /* Push/Forward Arguments */

                lua_pcall(l, argcount, (fp->hasReturnType() ? 1 : 0), 0);
                return (fp->hasReturnType() ? 1 : 0);
            };

            return static_cast<lua_CFunction>(f);
        }
    }

    lua_State* luaW_newstate(bool strict)
    {
        lua_State* l = lua_open();
        luaL_openlibs(l);

        if(strict)
            luaL_dostring(l, STRICT_LUA);

        return l;
    }

    LuaTable::Ptr luaW_gettable(lua_State* l, lua_String name)
    {
        lua_getglobal(l, name);
        LuaTable::Ptr t = LuaTable::create(l, -1);
        lua_pop(l, 1);
        return t;
    }

    void luaW_settable(lua_State* l, lua_String name, LuaTable::Ptr table)
    {
        table->push();
        lua_setglobal(l, name);
    }

    bool luaW_functionexists(lua_State* l, lua_String funcname)
    {
        bool res = true;
        lua_getglobal(l, funcname);

        if(lua_isnoneornil(l, -1))
            res = false;

        lua_pop(l, 1);
        return res;
    }

    void luaW_setfunction(lua_State *l, lua_String funcname, const LuaCFunction::Ptr &func)
    {
        Core::globalOverloads.insert(funcname, func);

        if(!luaW_functionexists(l, funcname))
        {
            lua_pushstring(l, funcname);
            lua_pushcclosure(l, Core::functionDispatcher(), 1);
            lua_setglobal(l, funcname);
        }
    }

    void luaW_addsearchpath(lua_State *l, const char *s)
    {
        lua_getglobal(l, "package");
        lua_getfield(l, -1, "path");
        string origpath = lua_tostring(l, -1);
        lua_pop(l, 1); /* Pop 'path' */

        string newpath = string(s) + "/?.lua";
        string newsearchpath = origpath +";" + newpath;

        lua_pushstring(l, newsearchpath.c_str());
        lua_setfield(l, -2, "path");
        lua_pop(l, 1); /* Pop 'package' */
    }

    void luaW_dofile(lua_State* l, const char *file)
    {
        int res = luaL_dofile(l, file);

        if(res == 1)
            throw LuaException(lua_tostring(l, -1));
    }

    void luaW_dostring(lua_State* l, const char* s)
    {
        int res = luaL_dostring(l, s);

        if(res == 1)
            throw LuaException(lua_tostring(l, -1));
    }

    void luaW_register(lua_State *l, const LuaTable::Ptr &table, const luaL_Reg *functions)
    {
        table->push();
        luaL_register(l, NULL, functions);
        lua_pop(l, 1);
    }
}
