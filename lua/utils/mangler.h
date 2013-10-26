#ifndef MANGLER_H
#define MANGLER_H

#include <string>
#include "../luatypes.h"
#include "tupleforeach.h"

namespace Lua
{
    namespace Utils
    {
        namespace Mangler
        {
            using namespace std;

            struct MangleFunctionFromType
            {
                template<typename T> void operator()(const T&, const int i, string* s)
                {
                    if(i == 0)
                        s->append("_");

                    switch(typeOf<T>())
                    {
                        case LuaTypes::Number:
                            s->append("n");
                            break;

                        case LuaTypes::String:
                            s->append("s");
                            break;

                        case LuaTypes::Bool:
                            s->append("b");
                            break;

                        case LuaTypes::Table:
                            s->append("t");
                            break;

                        case LuaTypes::Function:
                            s->append("f");
                            break;

                        case LuaTypes::LightUserData:
                        case LuaTypes::UserData:
                            s->append("d");
                            break;

                        default:
                            throw LuaException("Mangler: Unsupported Type");
                            break;
                    }
                }
            };

            struct MangleFunctionFromLuaType
            {
                void operator()(LuaTypes::LuaType lt, const int i, string* s)
                {
                    if(i == 0)
                        s->append("_");

                    switch(lt)
                    {
                        case LuaTypes::Number:
                            s->append("n");
                            break;

                        case LuaTypes::String:
                            s->append("s");
                            break;

                        case LuaTypes::Bool:
                            s->append("b");
                            break;

                        case LuaTypes::Table:
                            s->append("t");
                            break;

                        case LuaTypes::Function:
                            s->append("f");
                            break;

                        case LuaTypes::LightUserData:
                        case LuaTypes::UserData:
                            s->append("d");
                            break;

                        default:
                            throw LuaException("(Lua)Mangler: Unsupported Type");
                            break;
                    }
                }
            };

            template<typename... Args> struct MangledName
            {
                void operator()(std::string* s)
                {
                    std::tuple<Args...> args;
                    Utils::TupleForeach::for_each(args, MangleFunctionFromType(), s);
                }
            };

            template<> struct MangledName<>
            {
                void operator()(std::string*)
                {

                }
            };
        }
    }
}

#endif // MANGLER_H
