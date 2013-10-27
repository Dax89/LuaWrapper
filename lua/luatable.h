#ifndef LUA_LUATABLE_H
#define LUA_LUATABLE_H

#include <functional>
#include <map>
#include <vector>
#include "luareference.h"
#include "luacfunction.h"
#include "utils/overloadtable.h"

namespace Lua
{
    class LuaTable : public LuaReference
    {
        private:
            template<typename K, typename V> struct ValueSetter
            {
                inline void operator()(const LuaTable* t, K key, V value)
                {
                    t->push();
                    luaT_pushvalue(t->state(), key);
                    luaT_pushvalue(t->state(), value);
                    lua_settable(t->state(), -3);
                    lua_pop(t->state(), 1);
                }
            };

            template<typename K> struct ReferenceSetter
            {
                inline void operator()(const LuaTable* t, K key, LuaReference::Ptr value)
                {
                    t->push();
                    luaT_pushvalue(t->state(), key);
                    value->push();
                    lua_settable(t->state(), -3);
                    lua_pop(t->state(), 1);
                }
            };

            struct StringComparator
            {
                bool operator()(const char* s1, const char* s2) const
                {
                    return std::strcmp(s1, s2) < 0;
                }
            };

        public:
            typedef shared_ptr<LuaTable> Ptr;

        private:
            typedef map<lua_Integer, LuaReference::Ptr> NumberRefMap;
            typedef map<lua_String, LuaReference::Ptr, LuaTable::StringComparator> StringRefMap;

        public:
            class Iterator
            {
                private:
                    Iterator(const LuaTable* owner, bool finished): _owner(owner), _finished(finished), _keyref(LUA_NOREF), _valueref(LUA_NOREF) { }
                    Iterator(const LuaTable* owner): _owner(owner), _finished(false), _keyref(LUA_NOREF), _valueref(LUA_NOREF)
                    {
                        owner->push();
                        lua_pushnil(owner->state());
                        this->nextPair();
                    }

                public:
                    Iterator(): _owner(nullptr), _finished(false), _keyref(LUA_NOREF), _valueref(LUA_NOREF) { }

                    Iterator(const Iterator& it): _owner(it._owner), _finished(it._finished)
                    {
                         *this = it;
                    }

                    ~Iterator()
                    {
                        this->unrefKey();
                        this->unrefValue();
                    }

                    Iterator& operator++(int)
                    {
                        this->unrefValue();

                        this->_owner->push();
                        lua_rawgeti(this->_owner->state(), LUA_REGISTRYINDEX, this->_keyref);
                        this->nextPair();

                        return *this;
                    }

                    LuaTable::Iterator& operator=(const LuaTable::Iterator& rhs)
                    {
                        if(*this != rhs)
                        {
                            this->_finished = rhs._finished;
                            this->_owner = rhs._owner;
                            this->_keyref = rhs.cloneRef(rhs._keyref);
                            this->_valueref = rhs.cloneRef(rhs._valueref);
                        }

                        return *this;
                    }

                    bool operator ==(const LuaTable::Iterator& it) const
                    {
                        return (this->_keyref == it._keyref) && (this->_valueref == it._valueref);
                    }

                    bool operator !=(const LuaTable::Iterator& it) const
                    {
                        return (this->_keyref != it._keyref) || (this->_valueref != it._valueref);
                    }

                    LuaTypes::LuaType keyType() const
                    {
                        lua_rawgeti(this->_owner->state(), LUA_REGISTRYINDEX, this->_keyref);
                        LuaTypes::LuaType t = luaT_typeof(this->_owner->state(), -1);
                        lua_pop(this->_owner->state(), 1);
                        return t;
                    }

                    LuaTypes::LuaType valueType() const
                    {
                        lua_rawgeti(this->_owner->state(), LUA_REGISTRYINDEX, this->_valueref);
                        LuaTypes::LuaType t = luaT_typeof(this->_owner->state(), -1);
                        lua_pop(this->_owner->state(), 1);
                        return t;
                    }

                    template<typename T> T key() const
                    {
                        lua_rawgeti(this->_owner->state(), LUA_REGISTRYINDEX, this->_keyref);
                        return ValueExtractor<T>::get(this->_owner->state());
                    }

                    template<typename T> T value() const
                    {
                        lua_rawgeti(this->_owner->state(), LUA_REGISTRYINDEX, this->_valueref);
                        return ValueExtractor<T>::get(this->_owner->state());
                    }

                private:
                    void unrefKey()
                    {
                        if(this->_keyref != LUA_NOREF)
                        {
                            luaL_unref(this->_owner->state(), LUA_REGISTRYINDEX, this->_keyref);
                            this->_keyref = LUA_NOREF;
                        }
                    }

                    void unrefValue()
                    {
                        if(this->_valueref != LUA_NOREF)
                        {
                            luaL_unref(this->_owner->state(), LUA_REGISTRYINDEX, this->_valueref);
                            this->_valueref = LUA_NOREF;
                        }
                    }

                    int cloneRef(int ref) const
                    {
                        if(ref == LUA_NOREF)
                            return ref;

                        lua_rawgeti(this->_owner->state(), LUA_REGISTRYINDEX, ref);
                        return luaL_ref(this->_owner->state(), LUA_REGISTRYINDEX);
                    }

                    void nextPair()
                    {
                        if(lua_next(this->_owner->state(), -2))
                        {
                            this->unrefKey();

                            this->_valueref = luaL_ref(this->_owner->state(), LUA_REGISTRYINDEX);
                            this->_keyref = luaL_ref(this->_owner->state(), LUA_REGISTRYINDEX);
                        }
                        else
                        {
                            this->_keyref = LUA_NOREF;
                            this->_valueref = LUA_NOREF;
                            this->_finished = true;
                        }
                    }

                private:
                    const LuaTable* _owner;
                    bool _finished;
                    int _keyref;
                    int _valueref;

                friend class LuaTable;
            };

            template<typename ReturnType, typename ...Args> class LuaMethodT: public LuaCFunctionT<ReturnType, LuaTable::Ptr, Args...>
            {
                private:
                    typedef LuaCFunctionT<ReturnType, LuaTable::Ptr, Args...> BaseClass;
                    typedef LuaMethodT<ReturnType, Args...> ClassType;

                public:
                    typedef shared_ptr<ClassType> Ptr;

                public:
                    LuaMethodT(lua_State* l, typename BaseClass::FunctionType func): BaseClass(l, func) { }
                    LuaMethodT(lua_State* l, int index): BaseClass(l, index) { }

                    static ClassType::Ptr create(lua_State* l, typename BaseClass::FunctionType func)
                    {
                        return ClassType::Ptr(new ClassType(l, func));
                    }
            };

        private:
            static lua_CFunction dispatcher();

        public:
            LuaTable(lua_State* l);
            LuaTable(lua_State* l, int index);
            LuaTable(lua_State *l, const LuaTable::Ptr& meta);
            static LuaTable::Ptr create(lua_State* l);
            static LuaTable::Ptr create(lua_State* l, int index);
            static LuaTable::Ptr create(lua_State *l, const LuaTable::Ptr& meta);

            template<typename T> bool containsKey(T key) const
            {
                this->push();
                luaT_pushvalue(this->state(), key);
                lua_gettable(this->state(), -2);

                bool res = !lua_isnil(this->state(), -1);
                lua_pop(this->state(), 2);
                return res;
            }

            bool containsKey(const LuaReference::Ptr& key) const;

            template<typename K, typename V> V get(K key)
            {
                this->push();
                luaT_pushvalue(this->state(), key);
                lua_gettable(this->state(), -2);

                V v = ValueExtractor<V>::get(this->state());
                lua_pop(this->state(), 1);
                return v;
            }

            template<typename K> LuaTable::Ptr get(K key)
            {
                this->push();
                luaT_pushvalue(this->state(), key);
                lua_gettable(this->state(), -2);

                LuaTable::Ptr t(new LuaTable(this->state(), -1));
                lua_pop(this->state(), 2);
                return t;
            }

            template<typename ReturnType, typename... Args> typename LuaTable::LuaMethodT<ReturnType, Args...>::Ptr method(lua_String name)
            {
                this->push();
                luaT_pushvalue(this->state(), name);
                lua_gettable(this->state(), -2);

                typename LuaTable::LuaMethodT<ReturnType, Args...>::Ptr t(new LuaTable::LuaMethodT<ReturnType, Args...>(this->state(), -1));
                lua_pop(this->state(), 2);
                return t;
            }


            void set(lua_Number key, void* value) const;
            void set(lua_String key, void* value) const;
            void set(lua_Number key, lua_Number value) const;
            void set(lua_String key, lua_String value) const;
            void set(lua_String key, lua_Number value) const;
            void set(lua_Number key, lua_String value) const;           
            void set(lua_Number key, LuaTable::Ptr value);
            void set(lua_String key, LuaTable::Ptr value);
            void set(lua_Number key, LuaCFunction::Ptr value);
            void set(lua_String key, LuaCFunction::Ptr value);

            bool isEmpty() const;
            LuaTable::Iterator begin() const;
            LuaTable::Iterator end() const;

        private:
            Utils::OverloadTable _overloads;
            NumberRefMap _numberrefs;
            StringRefMap _stringrefs;
    };

    template<> inline LuaTypes::LuaType typeOf<LuaTable::Ptr>()
    {
        return LuaTypes::Table;
    }

    template<> struct ValueExtractor<LuaTable::Ptr>
    {
        static LuaTable::Ptr get(lua_State *l)
        {
            LuaTable::Ptr val = LuaTable::create(l, -1);
            lua_pop(l, 1);
            return val;
        }
    };

    void luaT_getvalue(lua_State *l, int index, LuaTable::Ptr &v);
    void luaT_pushvalue(lua_State *l, const LuaTable::Ptr& v);
}

#endif // LUA_LUATABLE_H
