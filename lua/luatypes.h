#ifndef LUATYPES_H
#define LUATYPES_H

#include <lua.hpp>
#include "luaexception.h"

#define NULL_USERDATA static_cast<void*>(NULL)

namespace Lua
{
    typedef void* lua_UserData;
    typedef const char* lua_String;

    namespace LuaTypes
    {
        enum LuaType
        {
            None = LUA_TNONE,
            Nil = LUA_TNIL,
            Bool = LUA_TBOOLEAN,
            LightUserData = LUA_TLIGHTUSERDATA,
            Number = LUA_TNUMBER,
            String = LUA_TSTRING,
            Table = LUA_TTABLE,
            Function = LUA_TFUNCTION,
            UserData = LUA_TUSERDATA,
            Thread = LUA_TTHREAD
        };
    }

    template<typename T> inline LuaTypes::LuaType typeOf()
    {
        return LuaTypes::None;
    }

    template<> inline LuaTypes::LuaType typeOf<lua_Integer>()
    {
        return LuaTypes::Number;
    }

    template<> inline LuaTypes::LuaType typeOf<lua_Number>()
    {
        return LuaTypes::Number;
    }

    template<> inline LuaTypes::LuaType typeOf<lua_String>()
    {
        return LuaTypes::String;
    }

    template<> inline LuaTypes::LuaType typeOf<bool>()
    {
        return LuaTypes::Bool;
    }

    template<typename T> struct ValueExtractor
    {

    };

    template<> struct ValueExtractor<lua_Integer>
    {
        static lua_Integer get(lua_State *l)
        {
            lua_Integer val = lua_tointeger(l, -1);
            lua_pop(l, 1);
            return val;
        }
    };

    template<LuaTypes::LuaType> struct TypeCreator
    {

    };

    template<> struct TypeCreator<LuaTypes::Number>
    {
        typedef lua_Number Type;
    };

    template<> struct TypeCreator<LuaTypes::String>
    {
        typedef lua_String Type;
    };

    template<> struct TypeCreator<LuaTypes::Bool>
    {
        typedef bool Type;
    };

    template<> struct ValueExtractor<lua_Number>
    {
        static lua_Number get(lua_State *l)
        {
            lua_Number val = lua_tonumber(l, -1);
            lua_pop(l, 1);
            return val;
        }
    };

    template<> struct ValueExtractor<lua_String>
    {
        static lua_String get(lua_State *l)
        {
            lua_String val = lua_tostring(l, -1);
            lua_pop(l, 1);
            return val;
        }
    };

    template<> struct ValueExtractor<lua_UserData>
    {
        static lua_UserData get(lua_State *l)
        {
            lua_UserData val = lua_touserdata(l, -1);
            lua_pop(l, 1);
            return val;
        }
    };

    template<> struct ValueExtractor<bool>
    {
        static bool get(lua_State *l)
        {
            bool val = (lua_toboolean(l, -1) == 1);
            lua_pop(l, 1);
            return val;
        }
    };

    LuaTypes::LuaType luaT_typeof(lua_State* l, int index);
    bool luaT_isref(lua_State* l, int index);
    bool luaT_isvalue(lua_State* l, int index);
    lua_String luaT_typename(LuaTypes::LuaType t);

    inline void luaT_getvalue(lua_State* l, int index, lua_Integer& v) { v = lua_tointeger(l, index); }
    inline void luaT_getvalue(lua_State* l, int index, lua_Number& v) { v = lua_tonumber(l, index); }
    inline void luaT_getvalue(lua_State* l, int index, lua_String& v) { v = lua_tostring(l, index); }
    inline void luaT_getvalue(lua_State* l, int index, bool& v) { v = (lua_toboolean(l, index) == 1); }

    inline void luaT_pushvalue(lua_State* l, lua_Integer v) { lua_pushinteger(l, v); }
    inline void luaT_pushvalue(lua_State* l, lua_Number v) { lua_pushnumber(l, v); }
    inline void luaT_pushvalue(lua_State* l, lua_String v) { lua_pushstring(l, v); }
    inline void luaT_pushvalue(lua_State* l, bool v) { lua_pushboolean(l, v); }

    template<typename T> inline void luaT_getargument(lua_State* l, int narg, T& t)
    {
        LuaTypes::LuaType t1 = typeOf<T>();
        LuaTypes::LuaType t2 = luaT_typeof(l, narg);

        if(t2 != t1)
            luaL_typerror(l, narg, luaT_typename(t1));

        luaT_getvalue(l, narg, t);
    }
}
#endif // LUATYPES_H
