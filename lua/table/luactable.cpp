#include "luactable.h"

namespace Lua
{
    LuaCTable::LuaCTable(lua_State *l, lua_String name): LuaObject(l), _name(name)
    {
        LuaTable::Ptr meta = LuaTable::create(l);
        meta->set("__index", &LuaCTable::staticMetaIndex, this);
        meta->set("__newindex", &LuaCTable::staticMetaNewIndex, this);

        this->_table = LuaTable::create(l, meta);
        this->_table->setMe(this);
    }

    LuaCTable::~LuaCTable()
    {

    }

    lua_String LuaCTable::name()
    {
        return this->_name;
    }

    void LuaCTable::pushGlobal()
    {
        this->pushGlobal(this->_name);
    }

    LuaCTable& LuaCTable::operator=(const LuaTable::Ptr& rhs)
    {
        if(this->_table != rhs)
            this->_table = rhs;

        return *this;
    }

    int LuaCTable::staticMetaIndex(lua_State *l)
    {
        LuaCTable* thethis = reinterpret_cast<LuaCTable*>(lua_touserdata(l, lua_upvalueindex(1)));
        thethis->metaIndex(l);
        return 1;
    }

    int LuaCTable::staticMetaNewIndex(lua_State *l)
    {
        LuaCTable* thethis = reinterpret_cast<LuaCTable*>(lua_touserdata(l, lua_upvalueindex(1)));
        thethis->metaNewIndex(l);
        return 0;
    }

    void LuaCTable::pushGlobal(lua_String name)
    {
        LuaObject::pushGlobal(name);
    }

    Lua::LuaCTable::operator LuaTable::Ptr()
    {
        return this->_table;
    }

    int LuaCTable::length()
    {
        return this->_table->length();
    }

    LuaTypes::LuaType LuaCTable::type()
    {
        return this->_table->type();
    }

    void LuaCTable::push()
    {
        this->_table->push();
    }

    void LuaCTable::metaIndex(lua_State *l)
    {
        if(luaT_typeof(l, 2) == LuaTypes::String)
        {
            lua_String key = lua_tostring(l, 2);

            if(this->_fieldmap.find(key) != this->_fieldmap.end())
            {
                FieldItem fi = this->_fieldmap[key];

                switch(fi.Type)
                {
                    case LuaTypes::String:
                    {
                        lua_String* s = reinterpret_cast<lua_String*>(fi.FieldAddress);
                        lua_pushstring(l, *s);
                        return;
                    }

                    case LuaTypes::Integer:
                    {
                        lua_Integer* i = reinterpret_cast<lua_Integer*>(fi.FieldAddress);
                        lua_pushnumber(l, *i);
                        return;
                    }

                    case LuaTypes::Number:
                    {
                        lua_Number* i = reinterpret_cast<lua_Number*>(fi.FieldAddress);
                        lua_pushnumber(l, *i);
                        return;
                    }

                    case LuaTypes::Table:
                    {
                        LuaTable::Ptr* pt = reinterpret_cast<LuaTable::Ptr*>(fi.FieldAddress);

                        if(*pt)
                            (*pt)->push();
                        else
                            lua_pushnil(l);

                        return;
                    }

                    case LuaTypes::CTable:
                    {
                        LuaCTable::Ptr* cpt = reinterpret_cast<LuaCTable::Ptr*>(fi.FieldAddress);

                        if(*cpt)
                            (*cpt)->push(); /* Push the Lua Internal Table Rappresentation */
                        else
                            lua_pushnil(l);

                        return;
                    }

                    case LuaTypes::Bool:
                    {
                        bool* b = reinterpret_cast<bool*>(fi.FieldAddress);
                        lua_pushboolean(l, *b);
                        return;
                    }

                    default:
                        throw LuaException("LuaTable.__index: Unsupported Type");
                }
            }
        }

        lua_pushvalue(l, 2);
        lua_rawget(l, 1);
    }

    void LuaCTable::metaNewIndex(lua_State *l)
    {
        if(luaT_typeof(l, 2) == LuaTypes::String)
        {
            lua_String key = lua_tostring(l, 2);

            if(this->_fieldmap.find(key) != this->_fieldmap.end())
            {
                FieldItem fi = this->_fieldmap[key];

                switch(fi.Type)
                {
                    case LuaTypes::String:
                    {
                        lua_String* s = reinterpret_cast<lua_String*>(fi.FieldAddress);
                        *s = lua_tostring(l, 3);
                        break;
                    }

                    case LuaTypes::Integer:
                    {
                        lua_Integer* i = reinterpret_cast<lua_Integer*>(fi.FieldAddress);
                        *i = lua_tointeger(l, 3);
                        break;
                    }

                    case LuaTypes::Number:
                    {
                        lua_Number* i = reinterpret_cast<lua_Number*>(fi.FieldAddress);
                        *i = lua_tonumber(l, 3);
                        break;
                    }

                    case LuaTypes::Table:
                    {
                        LuaTable::Ptr* t = reinterpret_cast<LuaTable::Ptr*>(fi.FieldAddress);
                        *t = LuaTable::fromStack(l, 3);
                        break;
                    }

                    case LuaTypes::CTable:
                    {
                        LuaCTable::Ptr* ct = reinterpret_cast<LuaCTable::Ptr*>(fi.FieldAddress);
                        (*ct)->_table = LuaTable::fromStack(l, 3); /* Set THE NEW Lua Table Rappresentation */
                        break;
                    }

                    case LuaTypes::Bool:
                    {
                        bool* b = reinterpret_cast<bool*>(fi.FieldAddress);
                        *b = lua_toboolean(l, 3) != 0;
                        break;
                    }

                    default:
                        throw LuaException("LuaTable.__newindex: Unsupported Type");
                }
            }
        }

        lua_pushvalue(l, 2);
        lua_pushvalue(l, 3);
        lua_rawset(l, 1);
    }

    void luaT_getvalue(lua_State *l, int index, LuaCTable::Ptr &v)
    {
        *v = LuaTable::fromStack(l, index);
    }

    void luaT_pushvalue(lua_State*, const LuaCTable::Ptr &v)
    {
        v->push();
    }
}
