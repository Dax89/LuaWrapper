#ifndef LUA_LUALIBRARY_H
#define LUA_LUALIBRARY_H

#include <vector>
#include <map>
#include <memory>
#include <functional>
#include "lua.h"

#define luamethods public

namespace Lua
{
    using namespace std;

    class LuaLibrary
    {
        public:
            class GenericTableItem
            {
                public:
                    typedef shared_ptr<GenericTableItem> Ptr;

                public:
                    GenericTableItem(LuaTypes::LuaType ft, LuaTypes::LuaType st): _keytype(ft), _valuetype(st) { }
                    LuaTypes::LuaType keyType() const { return this->_keytype; }
                    LuaTypes::LuaType valueType() const { return this->_valuetype; }
                    virtual void setTo(LuaTable::Ptr&) { }

                private:
                    LuaTypes::LuaType _keytype;
                    LuaTypes::LuaType _valuetype;
            };

            template<typename K, typename V> class TableItem: public GenericTableItem
            {
                public:
                    typedef shared_ptr< TableItem<K, V> > Ptr;

                public:
                    TableItem(K k, V v): GenericTableItem(typeOf<K>(), typeOf<V>()), _key(k), _value(v) { }
                    virtual K key() const { return this->_key; }
                    virtual V value() const { return this->_value; }
                    virtual void setTo(LuaTable::Ptr& t) { t->set(this->_key, this->_value); }

                private:
                    K _key;
                    V _value;
            };

            class GenericFunctionItem
            {
                public:
                    typedef shared_ptr<GenericFunctionItem> Ptr;

                public:
                    GenericFunctionItem(const char* funcname): _name(funcname) { }
                    const char* name() const { return this->_name; }
                    virtual LuaCFunction::Ptr function(lua_State*) { return NULL; }
                    virtual void setTo(lua_State*, LuaTable::Ptr&){ }

                private:
                    const char* _name;
            };

            template<typename ReturnType, typename... Args> class FunctionItem: public GenericFunctionItem
            {
                public:
                    typedef shared_ptr< FunctionItem<ReturnType, Args...> > Ptr;

                private:
                    typedef ReturnType (*FunctionType)(Args...);
                    typedef LuaCFunctionT<ReturnType, Args...> LuaCFunctionType;

                public:
                    FunctionItem(const char* funcname, FunctionType ft): GenericFunctionItem(funcname), _func(ft) { }

                    virtual LuaCFunction::Ptr function(lua_State* l)
                    {
                        if(!this->_funcobj)
                        {
                            typename LuaCFunctionType::Ptr p(new LuaCFunctionType(l, this->_func));
                            this->_funcobj = dynamic_pointer_cast<LuaCFunction>(p);
                        }

                        return this->_funcobj;
                    }

                    virtual void setTo(lua_State* l, LuaTable::Ptr& t)
                    {
                        t->set(this->name(), this->function(l));
                    }

                private:
                    FunctionType _func;
                    LuaCFunction::Ptr _funcobj; /* Prevent Deletion */
            };

        public:
            class TableDefinition
            {                
                public:
                    typedef std::shared_ptr<TableDefinition> Ptr;

                public:
                    TableDefinition(const char* name): _name(name) { }

                    template<typename K, typename V> void add(K key, V value)
                    {
                        typename TableItem<K, V>::Ptr ti(new TableItem<K, V>(key, value));
                        this->_items.push_back(ti);
                    }

                    template<typename ReturnType, typename... Args> void add(const char* name, ReturnType(*func)(LuaTable::Ptr, Args...))
                    {
                        typename FunctionItem<ReturnType, LuaTable::Ptr, Args...>::Ptr fi(new FunctionItem<ReturnType, LuaTable::Ptr, Args...>(name, func));
                        this->_functions.push_back(fi);
                    }

                    const char* name() const { return this->_name; }
                    int itemCount() const { return this->_items.size(); }
                    int functionCount() const { return this->_functions.size(); }
                    GenericTableItem::Ptr item(int i) const { return this->_items.at(i); }
                    GenericFunctionItem::Ptr function(int i) const { return this->_functions.at(i); }

                private:
                    const char* _name;
                    vector<GenericTableItem::Ptr> _items;
                    vector<GenericFunctionItem::Ptr> _functions;
            };

        public:
            LuaLibrary(const char* libname);
            const char *name() const;
            int tablesCount();
            TableDefinition::Ptr tableDefinition(int i) const;
            LuaTable::Ptr table(const char* name);
            bool isLoaded();
            virtual void load(lua_State* l);

        protected:
            TableDefinition::Ptr registerTable(const char *name);

        private:
            vector<TableDefinition::Ptr> _tables;
            map<const char*, LuaTable::Ptr> _tablemap;
            const char* _name;
            bool _loaded;
    };
}

#endif // LUA_LUALIBRARY_H
