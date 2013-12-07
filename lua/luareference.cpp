#include "luareference.h"

namespace Lua
{
    LuaReference::LuaReference(lua_State *l): LuaObject(l)
    {

    }

    LuaReference::LuaReference(lua_State* l, int index): LuaObject(l)
    {
        if(luaT_typeof(l, index) == LuaTypes::Nil)
            throw LuaException("LuaReference(): Got a Nil type");

        lua_pushvalue(l, index);
        this->_ref = luaL_ref(l, LUA_REGISTRYINDEX);
    }

    LuaReference::~LuaReference()
    {
        if(this->state() && this->_ref != -1)
            luaL_unref(this->state(), LUA_REGISTRYINDEX, this->_ref);

        this->_ref = -1;
    }

    int LuaReference::length()
    {
        this->push();
        int len = lua_objlen(this->state(), -1);

        lua_pop(this->state(), 1);
        return len;
    }

    int LuaReference::refId() const
    {
        return this->_ref;
    }

    bool LuaReference::isValid()
    {
        return this->_ref != -1;
    }

    LuaTypes::LuaType LuaReference::type()
    {
        this->push();
        LuaTypes::LuaType t = luaT_typeof(this->state(), -1);

        lua_pop(this->state(), 1);
        return t;
    }

    void LuaReference::push()
    {
        lua_rawgeti(this->state(), LUA_REGISTRYINDEX, this->_ref);
    }
}
