#ifndef LUATABLEWRAPPER_H
#define LUATABLEWRAPPER_H

#include "luatable.h"

namespace Lua
{
    class LuaTableWrapper
    {
        public:
            LuaTableWrapper(const LuaTable::Ptr& luatable);
            lua_State* state() const;
            bool isValid() const;
            LuaTable::Ptr table() const;

        private:
            LuaTable::Ptr _tableptr;
    };
}

#endif // LUATABLEWRAPPER_H
