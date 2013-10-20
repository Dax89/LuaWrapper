#include "lualibrary.h"

namespace Lua
{
    LuaLibrary::LuaLibrary(const char *libname): _name(libname), _loaded(false)
    {

    }

    const char* LuaLibrary::name() const
    {
        return this->_name;
    }

    int LuaLibrary::tablesCount()
    {
        return this->_tables.size();
    }

    LuaLibrary::TableDefinition::Ptr LuaLibrary::tableDefinition(int i) const
    {
        return this->_tables.at(i);
    }

    LuaTable::Ptr LuaLibrary::table(const char *name)
    {
        return this->_tablemap[name];
    }

    bool LuaLibrary::isLoaded()
    {
        return this->_loaded;
    }

    LuaLibrary::TableDefinition::Ptr LuaLibrary::registerTable(const char* name)
    {
        TableDefinition::Ptr td(new TableDefinition(name));
        this->_tables.push_back(td);
        return td;
    }

    void LuaLibrary::load(lua_State *l)
    {
        if(this->_loaded)
            return;

        this->_loaded = true;
        vector<TableDefinition::Ptr>::iterator tableit;

        for(tableit = this->_tables.begin(); tableit != this->_tables.end(); tableit++)
         {
             LuaLibrary::TableDefinition::Ptr td = *tableit;
             LuaTable::Ptr table = LuaTable::create(l);

             for(int i = 0; i < td->itemCount(); i++)
             {
                 LuaLibrary::GenericTableItem::Ptr ti = td->item(i);
                 ti->setTo(table);
             }

             if(td->functionCount())
             {
                 for(int j = 0; j < td->functionCount(); j++)
                 {
                     GenericFunctionItem::Ptr fi = td->function(j);
                     fi->setTo(l, table);
                 }
             }

             table->push();
             lua_setglobal(l, td->name());
             this->_tablemap.insert(std::pair<const char*, LuaTable::Ptr>(td->name(), table));
         }
    }
}
