#ifndef LUA_LUAREFERENCE_H
#define LUA_LUAREFERENCE_H

#include <cstdlib>
#include "luaobject.h"

namespace Lua
{
    class LuaReference: public LuaObject
    {
        public:
            typedef std::shared_ptr<LuaReference> Ptr;

        protected:
            LuaReference(lua_State *l);

        public:
            LuaReference(lua_State *l, int index);
            virtual ~LuaReference();
            int refId() const;
            bool isValid();

        public: /* Overriden Methods */
            virtual int length();
            virtual LuaTypes::LuaType type();
            virtual void push();

        protected:
            int _ref;
    };
}

#endif // LUA_LUAREFERENCE_H
