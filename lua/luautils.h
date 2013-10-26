#ifndef LUAUTILS_H
#define LUAUTILS_H

#include <tuple>
#include <functional>
#include "luatypes.h"

namespace Lua
{
    namespace Utils
    {
        using namespace std;

        namespace TupleCall /* From: https://github.com/JakobOvrum/LuaCpp/blob/master/luacpp/tuplecall.hpp */
        {
            template<typename Return, typename Function, typename Tuple, std::size_t need_indices, std::size_t... index>
            struct Dispatcher : public Dispatcher<Return, Function, Tuple, need_indices - 1, need_indices - 1, index...>
            {

            };

            template<typename Return, typename Function, typename Tuple, std::size_t... index> struct Dispatcher<Return, Function, Tuple, 0, index...>
            {
                static Return call(Function func, const Tuple& args)
                {
                    return func(std::get<index>(args)...);
                }
            };

            template <typename Return, typename... Arguments> Return call(function<Return(Arguments...)> func, const std::tuple <Arguments...>& args)
            {
                return Dispatcher<Return, function<Return(Arguments...)>, std::tuple <Arguments...>, sizeof...(Arguments)>::call(func, args);
            }
        }

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

#endif // LUAUTILS_H
