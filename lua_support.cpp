#include "lua_wrapper/lua_support.h"

#include "lua_wrapper/lua_table.h"

lua_State* L = nullptr;

namespace
{
    const size_t kBufferSize = 512;
}

namespace lua
{
    void tokenize(const char* input, std::vector<std::string>& output, const char* split_symbol)
    {
        output.clear();

        char buffer[kBufferSize] = { 0 };
        strncpy(buffer, input, kBufferSize);

        const char * field_name = strtok(buffer, split_symbol);
        while (field_name)
        {
            output.push_back(field_name);
            field_name = strtok(nullptr, split_symbol);
        }
    }

    typedef std::map<std::string, std::string> StringMap;

    int GetMapValue(lua_State* L)
    {
        StringMap* a = (StringMap*)lua_touserdata(L, 1);
        luaL_argcheck(L, a != nullptr, 1, "map expected");
        luaL_argcheck(L, lua_isstring(L, 2), 2, "string expected");

        const std::string& value = (*a)[lua_tostring(L, 2)];
        lua_pop(L, 2);

        lua_pushstring(L, value.c_str());

        return 1;
    }

    int SetMapValue(lua_State* L)
    {
        if (!lua_isnil(L, 3))
        {
            StringMap* a = (StringMap*)lua_touserdata(L, 1);
            luaL_argcheck(L, a != nullptr, 1, "map expected");
            luaL_argcheck(L, lua_isstring(L, 2), 2, "string expected");
            luaL_argcheck(L, lua_isstring(L, 3) || lua_isnumber(L, 3), 3, "string or number expected");

            (*a)[lua_tostring(L, 2)] = lua_tostring(L, 3);
        }
        lua_pop(L, 3);

        return 0;
    }

    void PushStringAsNumber(const std::string& str)
    {
        lua_pushstring(L, str.c_str());
        if (0 != lua_isnumber(L, -1))
        {
            lua_pushnumber(L, lua_tointeger(L, -1));
            lua_remove(L, -2);
        }
    }

    void GarbageCollect()
    {
        if (nullptr != L)
        {
            lua_gc(L, LUA_GCCOLLECT, 0);
        }
    }

    int GetAllocatedMemory()
    {
        return (nullptr != L) ? lua_gc(L, LUA_GCCOUNT, 0) : 0;
    }

    void OpenLibs()
    {
        L = lua_open();
        luaL_openlibs(L);
    }

    void CloseVM()
    {
        lua_close(L);
    }

    void PushValue(lua_State* L) {}

    void PushValue(lua_State* L, lua::VarType value)
    {
        switch(value)
        {
            case lua::NIL:
            {
                lua_pushnil(L);
            }
            break;
        }
    }

    void PushValue(lua_State* L, const std::string& value)
    {
        lua_pushstring(L, value.c_str());
    }

    void PushValue(lua_State* L, const char* value)
    {
        lua_pushstring(L, value);
    }

    void PushValue(lua_State* L, void* value)
    {
        AssertCrush(nullptr != value);
        lua_pushlightuserdata(L, value);
    }

    void PushValue(lua_State* L, const Table& tbl)
    {
        lua_getref(L, tbl.GetRef());
    }

    // lua namespace
}
