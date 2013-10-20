#ifndef TUPLESTACK_H
#define TUPLESTACK_H

#include <tuple>
#include "../luatypes.h"

namespace Lua
{
    namespace Utils
    {
        using namespace std;

        template<int n, typename... Args> struct StackToTuple
        {
            static void fill(lua_State* l, std::tuple<Args...>& args)
            {
                luaT_getargument(l, sizeof...(Args) - n + 1, std::get<sizeof...(Args) - n>(args));
                StackToTuple<n - 1, Args...>::fill(l, args);
            }
        };

        template<typename... Args> struct StackToTuple<0, Args...>
        {
            static void fill(lua_State*, std::tuple<Args...>&)
            {

            }
        };
    }
}

#endif // TUPLESTACK_H
