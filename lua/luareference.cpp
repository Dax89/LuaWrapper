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

    int LuaReference::length() const
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

    bool LuaReference::isValid() const
    {
        return this->_ref != -1;
    }

    LuaTypes::LuaType LuaReference::type() const
    {
        this->push();
        LuaTypes::LuaType t = luaT_typeof(this->state(), -1);

        lua_pop(this->state(), 1);
        return t;
    }

    void LuaReference::push(lua_State *l) const
    {
        lua_rawgeti(l, LUA_REGISTRYINDEX, this->_ref);
    }

    void LuaReference::push() const
    {
        this->push(this->state());
    }
}
