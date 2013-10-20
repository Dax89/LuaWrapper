#include "luacfunction.h"

namespace Lua
{
    LuaCFunction::LuaCFunction(lua_State *l): LuaReference(l)
    {

    }

    lua_CFunction LuaCFunction::luaFunction() const
    {
        return nullptr;
    }

    bool LuaCFunction::hasReturnType() const
    {
        return this->returnType() != LuaTypes::None;
    }

    LuaTypes::LuaType LuaCFunction::returnType() const
    {
        return LuaTypes::None;
    }

    const char* LuaCFunction::mangledName(const char*) const
    {
        return nullptr;
    }

    LuaCFunction::operator lua_CFunction() const
    {
        return nullptr;
    }
}
