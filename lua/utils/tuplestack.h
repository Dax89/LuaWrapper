#ifndef TUPLESTACK_H
#define TUPLESTACK_H

#include <tuple>
#include "../luatypes.h"

namespace Lua
{
    namespace Utils
    {
        using namespace std;

        template<int Index, typename... Args> struct StackToTuple
        {
            static void fill(lua_State* l, std::tuple<Args...>& args)
            {
                luaT_getargument(l, sizeof...(Args) - Index + 1, std::get<sizeof...(Args) - Index>(args));
                StackToTuple<Index - 1, Args...>::fill(l, args);
            }
        };

        template<typename... Args> struct StackToTuple<0, Args...>
        {
            static void fill(lua_State*, std::tuple<Args...>&)
            {

            }
        };

        namespace Stack
        {
            template<int Index, int Length, typename Tuple> struct TupleToStackImpl
            {
                static void push(lua_State* l, const Tuple& args)
                {
                    luaT_pushvalue(l, std::get<Index>(args));
                    TupleToStackImpl<Index + 1, Length, Tuple>::push(l, args);
                }
            };

            template<int Length, typename Tuple> struct TupleToStackImpl<Length, Length, Tuple>
            {
                static void push(lua_State* l, const Tuple& args)
                {
                    luaT_pushvalue(l, std::get<Length>(args));
                }
            };

            template<typename Tuple> void tupleToStack(lua_State* l, const Tuple& args)
            {
                TupleToStackImpl<0, std::tuple_size<Tuple>::value - 1, Tuple>::push(l, args);
            }
        }
    }
}

#endif // TUPLESTACK_H
