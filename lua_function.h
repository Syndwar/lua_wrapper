#ifndef LUA_FUNCTION_H
#define LUA_FUNCTION_H

#include "lua_support.h"

extern lua_State *L;

namespace lua
{
    class Function
    {
    private:
        int ref = LUA_NOREF;

    public:
        Function() {}

        Function(int unique_ref);

        Function(const Function& func);

        Function(const std::string& func_id);

        const Function& operator=(const Function& func);

        virtual ~Function();

        template <typename... _Ret, typename... _Targs>
        typename _pop<sizeof...(_Ret), _Ret... >::type Call(const _Targs&... args)
        {
            lua_getref(L, ref);

            const int num_args = sizeof...(_Targs);
            const int num_ret = sizeof...(_Ret);

            // Push all arguments to the stack using our variadic Push
            PushValue(L, args...);

            // Call the function
            if (0 != lua_pcall(L, num_args, num_ret, 0))
            {
                if (lua_isstring(L, -1))
                {
                    const char* error_msg = lua_tostring(L, -1);                /* L: ... error */
                    lua_pop(L, 1);                                              // remove error message from stack

					DebugCrush("[LUA ERROR] %s\n", error_msg);
                }
            }

            // Return all the results and remove them from the stack
            return Pop<_Ret...>(0);
        }

    private:
        int Load(const char* func_id);
    };

    template<>
    inline Function ReadValue(lua_State* L, int index)
    {
        if (lua_isfunction(L, index))
        {
            int ref = lua_ref(L, LUA_REGISTRYINDEX);
            lua_pushnil(L);
            return Function(ref);
        }
        else
        {
            return Function(/*LUA_NOREF*/);
        }
    }
}

#endif
