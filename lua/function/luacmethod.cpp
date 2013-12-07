#include "luacmethod.h"

namespace Lua
{
    LuaCMethod::LuaCMethod(lua_State *l): LuaObject(l)
    {
    }

    bool LuaCMethod::hasReturnType() const
    {
        return this->returnType() != LuaTypes::None;
    }

    int LuaCMethod::argumentCount() const
    {
        return 0;
    }

    int LuaCMethod::length()
    {
        return 0;
    }

    LuaTypes::LuaType LuaCMethod::type()
    {
        return LuaTypes::Function;
    }

    void LuaCMethod::push()
    {
        luaT_pushvalue(this->state(), reinterpret_cast<lua_UserData>(this));
        lua_pushcclosure(this->state(), this->luaCFunction(), 1);
    }

    LuaCMethod::operator lua_CFunction() const
    {
        return this->luaCFunction();
    }
}
