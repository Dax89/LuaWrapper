#include "luaobject.h"

namespace Lua
{
    LuaObject::LuaObject(lua_State *l): _state(l)
    {
    }

    LuaObject::~LuaObject()
    {
        this->_state = NULL;
    }

    lua_State *LuaObject::state() const
    {
        return this->_state;
    }

    bool LuaObject::isNull() const
    {
        return this->type() == LuaTypes::Nil;
    }
}
