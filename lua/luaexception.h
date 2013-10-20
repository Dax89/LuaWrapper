#ifndef LUAEXCEPTION_H
#define LUAEXCEPTION_H

#include <string>
#include <stdexcept>
#include "lua.hpp"

namespace Lua
{
    class LuaException: public std::exception
    {
        public:
            LuaException(const char* msg): _msg(msg) { }
            virtual const char* what() const noexcept { return this->_msg; }

        protected:
            const char* _msg;
    };

    class LuaTypeException: public LuaException
    {
        public:
            LuaTypeException(): LuaException("Invalid Type Conversion") { }
    };

    class LuaArgumentException: public LuaException
    {
        public:
            LuaArgumentException(lua_State* l): LuaException(NULL)
            {
                std::string s = lua_tostring(l, lua_upvalueindex(1));

                s.append("Invalid Argument(s)");
                this->_msg = s.c_str();
            }

            LuaArgumentException(lua_State* l, const char* msg): LuaException(NULL)
            {
                std::string s = lua_tostring(l, lua_upvalueindex(1));

                s.append(": ").append(msg);
                this->_msg = s.c_str();
            }
    };
}

#endif // LUAEXCEPTION_H
