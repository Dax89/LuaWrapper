#include "luatypes.h"

namespace Lua
{
    LuaTypes::LuaType luaT_typeof(lua_State *l, int index)
    {
        int t = lua_type(l, index);

        if(t == LUA_TNONE)
            throw LuaException("Invalid Lua Type");

        return static_cast<LuaTypes::LuaType>(t);
    }

    bool luaT_isref(lua_State *l, int index)
    {
        LuaTypes::LuaType t = luaT_typeof(l, index);

        switch(t)
        {
            case LuaTypes::Nil:
            case LuaTypes::Bool:
            case LuaTypes::LightUserData:
            case LuaTypes::Number:
            case LuaTypes::String:
            case LuaTypes::UserData:
                return false;

            case LuaTypes::Table:
            case LuaTypes::Function:
            case LuaTypes::Thread:
                return true;

            default:
                break;
        }

        throw LuaException("Invalid Lua Type");
    }

    bool luaT_isvalue(lua_State *l, int index)
    {
        LuaTypes::LuaType t = luaT_typeof(l, index);

        switch(t)
        {
            case LuaTypes::Nil:
            case LuaTypes::Bool:
            case LuaTypes::LightUserData:
            case LuaTypes::Number:
            case LuaTypes::String:
            case LuaTypes::UserData:
                return true;

            case LuaTypes::Table:
            case LuaTypes::Function:
            case LuaTypes::Thread:
                return false;

            default:
                break;
        }

        throw LuaException("Invalid Lua Type");
    }

    const char *luaT_typename(LuaTypes::LuaType t)
    {
        switch(t)
        {
            case LuaTypes::Nil:
                return "Nil";

            case LuaTypes::Bool:
                return "Bool";

            case LuaTypes::LightUserData:
                return "LightUserData";

            case LuaTypes::Number:
                return "Number";

            case LuaTypes::String:
                return "String";

            case LuaTypes::Table:
                return "Table";

            case LuaTypes::Function:
                return "Function";

            case LuaTypes::UserData:
                return "UserData";

            case LuaTypes::Thread:
                return "Thread";

            default:
                break;
        }

        return "Unknown";
    }
}
