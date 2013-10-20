#include "luatablewrapper.h"

namespace Lua
{
    LuaTableWrapper::LuaTableWrapper(const LuaTable::Ptr &luatable): _tableptr(luatable)
    {
    }

    lua_State *LuaTableWrapper::state() const
    {
        if(this->isValid())
            return this->_tableptr->state();

        return NULL;
    }

    bool LuaTableWrapper::isValid() const
    {
        if(!this->_tableptr || !this->_tableptr->isValid())
            return false;

        return true;
    }

    LuaTable::Ptr LuaTableWrapper::table() const
    {
        return this->_tableptr;
    }
}
