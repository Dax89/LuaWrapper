#include "luatable.h"

namespace Lua
{
    const lua_String LuaTable::COBJECT_FIELD = "__cthis__";

    LuaTable::LuaTable(lua_State *l): LuaReference(l)
    {
        lua_newtable(l);

        /* Set metamethod '__index' */
        lua_pushstring(l, "__index");
        lua_pushvalue(l, -2);
        lua_settable(l, -3);

        this->_ref = luaL_ref(l, LUA_REGISTRYINDEX);
    }

    LuaTable::LuaTable(lua_State *l, const LuaTable::Ptr& meta): LuaReference(l)
    {
        lua_newtable(l);
        meta->push();
        lua_setmetatable(l, -2);

        this->_ref = luaL_ref(l, LUA_REGISTRYINDEX);
    }

    LuaTable::LuaTable(lua_State *l, int index): LuaReference(l, index)
    {

    }

    LuaTable::Ptr LuaTable::create(lua_State *l)
    {
        return LuaTable::Ptr(new LuaTable(l));
    }

    LuaTable::Ptr LuaTable::create(lua_State *l, int index)
    {
        return LuaTable::Ptr(new LuaTable(l, index));
    }

    LuaTable::Ptr LuaTable::create(lua_State *l, const LuaTable::Ptr &meta)
    {
        return LuaTable::Ptr(new LuaTable(l, meta));
    }

    lua_CFunction LuaTable::dispatcher()
    {
        auto f = [](lua_State* l) -> int {
            int argcount = lua_gettop(l);
            LuaTable* thethis = reinterpret_cast<LuaTable*>(lua_touserdata(l, lua_upvalueindex(1)));
            string func = lua_tostring(l, lua_upvalueindex(2));
            string mangledfunc = func;

            for(int i = 0; i < argcount; i++)
            {
                LuaTypes::LuaType t = luaT_typeof(l, i + 1);
                Utils::Mangler::MangleFunctionFromLuaType()(t, i, &mangledfunc);
            }

            try
            {
                LuaCFunction::Ptr fp = thethis->_overloads.get(func.c_str(), mangledfunc.c_str());
                fp->push();

                for(int i = 1; i <= argcount; i++)
                    lua_pushvalue(l, i); /* Push/Forward Arguments */

                lua_pcall(l, argcount, (fp->hasReturnType() ? 1 : 0), 0);
                return (fp->hasReturnType() ? 1 : 0);
            }
            catch(std::out_of_range&)
            {
                string s = "Unknown Overload: ";
                throw LuaException(s.append(mangledfunc).c_str());
            }

            return 0;
        };

        return static_cast<lua_CFunction>(f);
    }

    bool LuaTable::containsKey(const LuaReference::Ptr& key) const
    {
        this->push();
        key->push();
        lua_gettable(this->state(), -2);

        bool res = !lua_isnil(this->state(), -1);
        lua_pop(this->state(), 2);
        return res;
    }

    bool LuaTable::isEmpty() const
    {
        this->push();

        lua_pushnil(this->state());
        bool res = lua_next(this->state(), -2);

        if(res)
            lua_pop(this->state(), 2); /* Pop Key <-> Value, the table is not empty */

        return !res;
    }

    LuaTable::Iterator LuaTable::begin() const
    {
        return LuaTable::Iterator(this);
    }

    LuaTable::Iterator LuaTable::end() const
    {
        return LuaTable::Iterator(this, true);
    }

    void LuaTable::setObject(lua_UserData thethis) const
    {
        this->set(LuaTable::COBJECT_FIELD, thethis);
    }

    void LuaTable::set(lua_Number key, lua_UserData value) const
    {
        LuaTable::ValueSetter<lua_Number, lua_UserData>()(this, key, value);
    }

    void LuaTable::set(lua_String key, lua_UserData value) const
    {
        LuaTable::ValueSetter<lua_String, lua_UserData>()(this, key, value);
    }

    void LuaTable::set(lua_Number key, lua_Number value) const
    {
        LuaTable::ValueSetter<lua_Number, lua_Number>()(this, key, value);
    }

    void LuaTable::set(lua_String key, lua_String value) const
    {
        LuaTable::ValueSetter<lua_String, lua_String>()(this, key, value);
    }

    void LuaTable::set(lua_String key, lua_Number value) const
    {
        LuaTable::ValueSetter<lua_String, lua_Number>()(this, key, value);
    }

    void LuaTable::set(lua_Number key, lua_String value) const
    {
        LuaTable::ValueSetter<lua_Number, lua_String>()(this, key, value);
    }

    void LuaTable::set(lua_Number key, LuaTable::Ptr value)
    {
        LuaTable::ReferenceSetter<lua_Number>()(this, key, value);
        this->_numberrefs[key] = value;
    }

    void LuaTable::set(lua_String key, LuaTable::Ptr value)
    {
        LuaTable::ReferenceSetter<lua_String>()(this, key, value);
        this->_stringrefs[key] = value;
    }

    void LuaTable::set(lua_Number key, LuaCFunction::Ptr value)
    {
        LuaTable::ReferenceSetter<lua_Number>()(this, key, value);
        this->_numberrefs[key] = value;
    }

    void LuaTable::set(lua_String key, LuaCFunction::Ptr value)
    {
        this->_overloads.insert(key, value);
        this->_stringrefs[key] = value;

        if(!this->containsKey(key))
        {
            this->push();

            /* Key */
            lua_pushstring(this->state(), key);

            /* Value */
            lua_pushlightuserdata(this->state(), reinterpret_cast<void*>(this));
            lua_pushstring(this->state(), key);
            lua_pushcclosure(this->state(), LuaTable::dispatcher(), 2);

            lua_settable(this->state(), -3);
            lua_pop(this->state(), 1);
        }
    }

    void luaT_getvalue(lua_State *l, int index, LuaTable::Ptr &v)
    {
        v = LuaTable::create(l, index);
    }

    void luaT_pushvalue(lua_State *l, const LuaTable::Ptr &v)
    {
        v->push(l);
    }

}
