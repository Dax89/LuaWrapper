#include "overloadtable.h"

namespace Lua
{
    namespace Utils
    {
        OverloadTable::OverloadTable()
        {

        }

        LuaCFunction::Ptr OverloadTable::get(lua_String funcname, lua_String mangledname) const
        {
            OverloadMap overloads = this->_functionmap.at(funcname);
            return overloads.at(mangledname);
        }

        void OverloadTable::insert(lua_String funcname, LuaCFunction::Ptr func)
        {
            if(this->_functionmap.find(funcname) == this->_functionmap.end())
                this->_functionmap[funcname] = OverloadMap();

            OverloadMap& overloadmap = this->_functionmap.at(funcname);
            overloadmap[func->mangledName(funcname)] = func;
        }
    }
}
