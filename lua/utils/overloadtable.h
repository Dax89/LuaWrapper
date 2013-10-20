#ifndef OVERLOADTABLE_H
#define OVERLOADTABLE_H

#include <map>
#include <lua.hpp>
#include "mangler.h"
#include "../luatypes.h"
#include "../luacfunction.h"

namespace Lua
{
    namespace Utils
    {
        using namespace std;

        class OverloadTable
        {
            private:
                struct StringComparator
                {
                    bool operator()(const char* s1, const char* s2) const
                    {
                        return std::strcmp(s1, s2) < 0;
                    }
                };

                typedef map<lua_String, LuaCFunction::Ptr, StringComparator> OverloadMap;
                typedef map<lua_String, OverloadMap, StringComparator> FunctionMap;

            public:
                OverloadTable();
                LuaCFunction::Ptr get(lua_String funcname, lua_String mangledname) const;
                void insert(lua_String funcname, LuaCFunction::Ptr func);

            private:
                FunctionMap _functionmap;

        };
    }
}
#endif // OVERLOADTABLE_H
