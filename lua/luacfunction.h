#ifndef LUA_LUACFUNCTION_H
#define LUA_LUACFUNCTION_H

#include <cstring>
#include <functional>
#include <tuple>
#include "luareference.h"
#include "utils/tuplestack.h"
#include "utils/tuplecall.h"
#include "utils/tupleforeach.h"
#include "utils/mangler.h"

namespace Lua
{
    using namespace std;

    namespace Detail
    {
        template<typename ReturnType, typename... Args> struct CallbackCall
        {
            static int call(lua_State* l, std::function<ReturnType(Args...)> func)
            {
                std::tuple<Args...> args;
                Utils::StackToTuple<sizeof...(Args), Args...>::fill(l, args);
                ReturnType r = Utils::TupleCall::call(func, args);
                luaT_pushvalue(l, r);
                return 1;
            }
        };

        template<typename... Args> struct CallbackCall<void, Args...>
        {
            static int call(lua_State* l, std::function<void(Args...)> func)
            {
                std::tuple<Args...> args;
                Utils::StackToTuple<sizeof...(Args), Args...>::fill(l, args);
                Utils::TupleCall::call(func, args);
                return 0;
            }
        };
    }

    class LuaCFunction: public LuaReference
    {
        public:
            typedef shared_ptr<LuaCFunction> Ptr;

        protected:
            virtual lua_CFunction luaFunction() const;

        public:
        LuaCFunction(lua_State* l);
        LuaCFunction(lua_State* l, int index): LuaReference(l, index) { }
        virtual bool hasReturnType() const;
        virtual LuaTypes::LuaType returnType() const;
        virtual const char* mangledName(const char*) const;
        virtual operator lua_CFunction() const;
    };

    template<typename ReturnType, typename... Args> class LuaCFunctionT: public LuaCFunction
    {
        private:
            typedef LuaCFunctionT<ReturnType, Args...> ClassType;

        public:
            typedef ReturnType(*FunctionType)(Args...);
            typedef shared_ptr<ClassType> Ptr;

        public:
            LuaCFunctionT(lua_State* l, ClassType::FunctionType func): LuaCFunction(l), _func(func)
            {
                lua_pushlightuserdata(l, this);
                lua_pushcclosure(l, this->luaFunction(), 1);
                this->_ref = luaL_ref(l, LUA_REGISTRYINDEX);
            }

            LuaCFunctionT(lua_State* l, int index): LuaCFunction(l, index) { }

        public: /* Overriden Protected Methods */
            virtual operator lua_CFunction() const
            {
                return this->luaFunction();
            }

            virtual const char* mangledName(const char* funcname) const
            {
                string s = funcname;
                std::tuple<Args...> args;

                if(tuple_size<decltype(args)>::value > 0)
                    Utils::TupleForeach::for_each(args, Utils::Mangler::MangleFunctionFromType(), &s);

                return strdup(s.c_str());
            }

        public:
            static ClassType::Ptr create(lua_State* l)
            {
                return ClassType::Ptr(new ClassType(l));
            }

            static ClassType::Ptr create(lua_State* l, ClassType::FunctionType func)
            {
                return ClassType::Ptr(new ClassType(l, func));
            }

            virtual LuaTypes::LuaType returnType() const
            {
                return typeOf<ReturnType>();
            }

        protected: /* Overriden Protected Methods */
            virtual lua_CFunction luaFunction() const
            {
                auto f = [](lua_State* l) -> int {
                    ClassType* thethis = reinterpret_cast<ClassType*>(lua_touserdata(l, lua_upvalueindex(1)));

                    if(thethis)
                    {
                        std::tuple<Args...> args;
                        Utils::StackToTuple<sizeof...(Args), Args...>::fill(l, args);
                        return Detail::CallbackCall<ReturnType, Args...>::call(l, thethis->_func);
                    }

                    throw LuaException("LuaCFunction::luaCallback(): 'this' Parameter NULL");
                };

                return static_cast<lua_CFunction>(f);
            }

        private:
            ClassType::FunctionType _func;
    };

    template<> inline LuaTypes::LuaType typeOf<LuaCFunction::Ptr>()
    {
        return LuaTypes::Function;
    }

    template<> struct ValueExtractor<LuaCFunction::Ptr>
    {
        static LuaCFunction::Ptr get(lua_State *l)
        {
            LuaCFunction::Ptr val(new LuaCFunction(l, -1));
            lua_pop(l, 1);
            return val;
        }
    };
}
#endif // LUA_LUACFUNCTION_H
