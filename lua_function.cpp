#include "lua_wrapper/lua_function.h"

namespace {
    const char * const separator = ".";
}

namespace lua
{
    Function::Function(int unique_ref) : ref(unique_ref) {}

    Function::Function(const Function& func)
    {
        lua_getref(L, func.ref);
        ref = lua_ref(L, -1);
    }

    Function::Function(const std::string& func_id)
    {
        ref = Load(func_id.c_str());
    }

    Function::~Function()
    {
        lua_unref(L, ref);
    }

    int Function::Load(const char* func_id)
    {
        std::vector<std::string> tables;
        tokenize(func_id, tables, separator);

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
            
            const bool tbl_check_failed = (i != i_end) && !lua_istable(L, -1);
            const bool func_check_failed = (i == i_end) && !lua_isfunction(L, -1);

            if ( tbl_check_failed || func_check_failed )
            {
                lua_pop(L, i);
                break;
            }
        }

        assert((1 == lua_isfunction(L, -1)) && "[lua] function not found");

        int index = lua_ref(L, LUA_REGISTRYINDEX);
        assert(LUA_REFNIL != index && "[lua] reference is nil");

        lua_pop(L, i - 1);

        CHECK_LUA_STACK_EMPTY(L);

        return index;
    }

    const Function& Function::operator=(const Function& func)
    {
        lua_unref(L, ref);
        lua_getref(L, func.ref);
        ref = lua_ref(L, -1);
        return *this;
    }

}
