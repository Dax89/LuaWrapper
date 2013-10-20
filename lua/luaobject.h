#ifndef LUA_LUAOBJECT_H
#define LUA_LUAOBJECT_H

#include <string>
#include <memory>
#include <stdexcept>
#include "luatypes.h"

namespace Lua
{
    class LuaObject
    {
        public:
            typedef std::shared_ptr<LuaObject> Ptr;

        public:
            LuaObject(lua_State* l);
            virtual ~LuaObject();
            lua_State* state() const;
            bool isNull() const;
            virtual int length() const = 0;
            virtual LuaTypes::LuaType type() const = 0;
            virtual void push(lua_State *l) const = 0;

        private:
            lua_State* _state;
    };
}

#endif // LUA_LUAOBJECT_H
