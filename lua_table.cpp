#include "lua_wrapper/lua_table.h"

#include <sstream>

namespace
{
    const char * const separator = ".";
}

namespace lua
{
    Table::Table()
    {
        lua_newtable(L);
        ref = lua_ref(L, LUA_REGISTRYINDEX);
    }

    Table::Table(int unique_ref): ref(unique_ref) {}

    Table::Table(const Table& table)
    {
        lua_getref(L, table.ref);
        ref = lua_ref(L, -1);
    }

    Table::Table(const std::string& table_id)
    {
        ref = Load(table_id.c_str());
    }

    Table::~Table()
    {
        lua_unref(L, ref);
    }

    int Table::GetRef() const
    {
        return ref;
    }

    size_t Table::GetSize() const
    {
        lua_getref(L, ref);
        size_t size = lua_objlen(L, -1);
        lua_pop(L, 1);
        return size;
    }

    int Table::Load(const char* table_name)
    {
        std::vector<std::string> tables;
        tokenize(table_name, tables, separator);

        int i(0);
        const int i_end = tables.size();
        for (const std::string& tbl_name : tables)
        {
            if (0 == i)
            {
                lua_getglobal(L, tbl_name.c_str());
            }
            else
            {
                PushStringAsNumber(tbl_name);
                lua_rawget(L, -2);
            }
            ++i;

            const bool tbl_check_failed = !lua_istable(L, -1);

            if (tbl_check_failed)
            {
                lua_pop(L, i);
                break;
            }
        }

        assert((1 == lua_istable(L, -1)) && "[lua] table not found");

        int index = lua_ref(L, LUA_REGISTRYINDEX);
        assert(LUA_REFNIL != index && "[lua] reference is nil");

        lua_pop(L, i - 1);

        CHECK_LUA_STACK_EMPTY(L);

        return index;
    }

    const Table& Table::operator=(const Table& table)
    {
        lua_unref(L, ref);
        lua_getref(L, table.ref);
        ref = lua_ref(L, -1);
        return *this;
    }

    bool Table::IsEmpty() const
    {
        lua_getref(L, ref);
        assert((1 == lua_istable(L, -1)) && "[lua] table not found");

        lua_pushnil(L);  /* first key */
        const bool result(0 == lua_next(L, -2));
        if (!result)
        {
            lua_pop(L, 2);
        }
        lua_pop(L, 1);

        assert((0 == lua_gettop(L)) && "[lua] stack is not empty");
        return result;
    }

    lua::Table Table::Copy() const
    {
        lua_newtable(L);

        lua_getref(L, ref);
        assert((1 == lua_istable(L, -1)) && "[lua] table not found");

        lua_pushnil(L);  /* first key */
        while (0 != lua_next(L, -2))
        {
            // create key copy
            lua_pushvalue(L, -2);
            // relocate key
            lua_insert(L, -2);
            // removes key and value from stack, key copy remains
            lua_settable(L, -5);
        }
        lua_pop(L, 1);

        const int new_ref = lua_ref(L, LUA_REGISTRYINDEX);

        CHECK_LUA_STACK_EMPTY(L);

        return lua::Table(new_ref);
    }

    std::string Table::GetString(const std::string & key)
    {
        return Get<const char*>(key);
    }
}
