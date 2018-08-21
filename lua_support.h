#ifndef LUA_SUPPORT_H
#define LUA_SUPPORT_H

#include "lua_ext.h"

#include <vector>
#include <map>
#include <assert.h>
#include <string>
#include <set>

extern lua_State *L;

namespace lua
{
    #define CHECK_LUA_STACK_EMPTY(L) assert(0 == lua_gettop(L) && "[lua] stack is not empty")
    #define CHECK_LUA_STACK_SIZE(L, count) assert_crush((count == lua_gettop(L)) && "[lua] wrong number of arguments")
    
    class Table;

    enum VarType
    {
        NIL
    };

    void tokenize(const char* input, std::vector<std::string>& output, const char* split_symbol);

    void PushStringAsNumber(const std::string& str);
	
    int GetMapValue(lua_State* L);
    
    int SetMapValue(lua_State* L);

    void OpenLibs();

    void AssertCrush(const bool value); // defined by user

    void DebugCrush(const char* tempate, const char* message); // defined by user

    bool CheckCompression(const char* module); // defined by user

    void Decomress(const char* module, std::vector<char>& data); // defined by user

    void CloseVM();

    void GarbageCollect();

    int GetAllocatedMemory();

    bool LoadModule(const char * lua_module);

    void PushValue(lua_State* L);

    void PushValue(lua_State* L, lua::VarType value);

    void PushValue(lua_State* L, const std::string& value);

    void PushValue(lua_State* L, const char* value);

    void PushValue(lua_State* L, void* value);

    void PushValue(lua_State* L, const Table& tbl);

    template<typename _Ty> struct dummy_dependency : public std::false_type {};

    template <typename _Ty>
    void PushValue(lua_State* L, const _Ty& value)
    {
        static_assert(dummy_dependency<_Ty>::value, "Must use specialization");
    }

    template <typename _T1, typename _T2>
    void PushValue(lua_State* L, const std::map<_T1, _T2>& value)
    {
        lua_newtable(L);                                    /* L: table */
        for (auto it : value)
        {
            PushValue(L, it.first);                         /* L: table key */
            PushValue(L, it.second);                        /* L: table key value*/
            lua_rawset(L, -3);                              /* table[key] = value, L: table */
        }
    }

    template <typename _Ty>
    void PushValue(lua_State* L, const std::vector<_Ty>& value)
    {
        int index(1);
        lua_newtable(L);                                    /* L: table */
        for (auto v : value)
        {
            PushValue(L, index++);                          /* L: table key */
            PushValue(L, v);                                /* L: table key value*/
            lua_rawset(L, -3);                              /* table[key] = value, L: table */
        }
    }

    template <typename _Ty>
    void PushValue(lua_State* L, const std::set<_Ty>& value)
    {
        int index(1);
        lua_newtable(L);                                    /* L: table */
        for (auto v : value)
        {
            PushValue(L, index++);                          /* L: table key */
            PushValue(L, v);                                /* L: table key value*/
            lua_rawset(L, -3);                              /* table[key] = value, L: table */
        }
    }

    template <typename _Ty, typename... _Targs>
    void PushValue(lua_State* L, const _Ty& value, const _Targs&... values)
    {
        PushValue(L, value);
        PushValue(L, values...);
    }

    template<>
    inline void PushValue(lua_State* L, const bool& value)
    {
        lua_pushboolean(L, value ? 1 : 0);
    }

    template<>
    inline void PushValue(lua_State* L, const int& value)
    {
        lua_pushinteger(L, value);
    }

    template<>
    inline void PushValue(lua_State* L, const lua::VarType& value)
    {
        lua_pushnil(L);
    }

    template<>
    inline void PushValue(lua_State* L, const float& value)
    {
        lua_pushnumber(L, value);
    }

    template<>
    inline void PushValue(lua_State* L, const size_t& value)
    {
        lua_pushinteger(L, value);
    }

    template<>
    inline void PushValue(lua_State* L, const double& value)
    {
        lua_pushnumber(L, value);
    }

    template<>
    inline void PushValue(lua_State* L, const long& value)
    {
        lua_pushinteger(L, value);
    }

    // templates for reading values from stack
    template<typename _Ty>
    _Ty ReadValue(lua_State* L, const int index)
    {
         return _Ty();
    }

    template<> inline
    bool ReadValue(lua_State* L, const int index)
    {
        return (1 == lua_toboolean(L, index));
    }

    template<> inline
    void * ReadValue(lua_State* L, const int index)
    {
        return lua_touserdata(L, index);
    }

    template<> inline
    int ReadValue(lua_State* L, const int index)
    {
        return static_cast<int>(lua_tointeger(L, index));
    }

    template<> inline
    double ReadValue(lua_State* L, const int index)
    {
        return lua_tonumber(L, index);
    }

    template<> inline
    float ReadValue(lua_State* L, const int index)
    {
        return static_cast<float>(lua_tonumber(L, index));
    }

    template<> inline
    size_t ReadValue(lua_State* L, const int index)
    {
        if (lua_isnil(L, index)) return size_t(-1);

        return static_cast<size_t>(lua_tointeger(L, index));
    }

    template<> inline
    const char * ReadValue(lua_State* L, const int index)
    {
        return (lua_isstring(L, index) ? lua_tostring(L, index) : "");
    }

    template<> inline
    std::string ReadValue(lua_State* L, const int index)
    {
        static std::string kEmptyString("");
        return (lua_isstring(L, index) ? lua_tostring(L, index) : kEmptyString);
    }

    // General type trait struct
    template<size_t, typename... _Targs>
    struct _pop
    {
        typedef std::tuple<_Targs... > type;

        // Base case - creates a tuple containing one element
        template <typename _Ty>
        static std::tuple<_Ty> worker(const int index)
        {
            return std::make_tuple(ReadValue<_Ty>(L, index));
        }

        // Inductive case
        template <typename _T1, typename _T2, typename... _Rest>
        static std::tuple<_T1, _T2, _Rest...> worker(const int index)
        {
            std::tuple<_T1> head = std::make_tuple(ReadValue<_T1>(L, index));
            return std::tuple_cat(head, worker<_T2, _Rest...>(index + 1));
        }

        static type apply(const int stack)
        {
            auto ret = worker<_Targs...>(1 + stack);
            lua_pop(L, (int)(sizeof...(_Targs)));
            lua_pop(L, stack);

            return ret;
        }
    };

    // Specialization for singular type
    template <typename _Ty>
    struct _pop <1, _Ty>
    {
        typedef _Ty type;
        static type apply(const int stack)
        {
            // Read the top element (negative indices count from the top)
            type ret = ReadValue<_Ty>(L, -1);

            // Remove it from the stack
            lua_pop(L, 1 + stack);
            return ret;
        }
    };

    // Partial specialization for when the size argument is 0
    template <typename... _Targs>
    struct _pop <0, _Targs... >
    {
        typedef void type;
        static type apply(const int stack) { if (stack > 0) lua_pop(L, stack); }
    };

    template <typename... _Targs>
    typename _pop<sizeof...(_Targs), _Targs...>::type Pop(const int stack = 0)
    {
        // Store the elements requested
        return _pop<sizeof...(_Targs), _Targs...>::apply(stack);
    }

    inline void GetField(const char* field_name)
    {
        lua_getglobal(L, field_name);
        if (lua_isnil(L, -1))
        {
            lua_pop(L, 1);
            DebugCrush("[LUA ERROR] %s is not a global field.\n", field_name);
        }
    }

    inline
    void GetField(const char* field_name, int depth)
    {
        if (depth < 1)
        {
            GetField(field_name);
            return;
        }

        lua_getfield(L, -1, field_name);
        if (lua_isnil(L, -1))
        {
            lua_pop(L, depth - 1);
            DebugCrush("[LUA ERROR] function %s was not found in table.\n", field_name);
        }
    }

    template <typename... _Ret, typename... _Targs>
    typename _pop<sizeof...(_Ret), _Ret... >::type Call(const char* func_path, const _Targs&... args)
    {
        int depth_count(0);
        const size_t buf_size(512);
        char buffer[buf_size] = { 0 };
        strncpy(buffer, func_path, buf_size);

        const char * field_name = strtok(buffer, ".");
        while (field_name)
        {
            GetField(field_name, depth_count++);
            field_name = strtok(nullptr, ".");
        }

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
            else
            {
                DebugCrush("[LUA ERROR] function %s call error\n", func_path);
            }
        }
        depth_count--;

        // Return all the results and remove them from the stack
        return Pop<_Ret...>(depth_count);
    }

    /**
    * Loads and process specified buffer
    *
    * @return a value that can be cast to Type _Ret (multiple values are wrapped into std::tupple).
    */
    template <typename... _Ret>
    typename _pop<sizeof...(_Ret), _Ret... >::type LoadBuffer(const char* buffer, bool & success)
    {
        const int num_ret = sizeof...(_Ret);

        luaL_loadstring(L, buffer);

        success = true;
        if (0 != lua_pcall(L, 0, num_ret, 0))
        {
            success = false;
            if (lua_isstring(L, -1))
            {
                const char* error_msg = lua_tostring(L, -1);                /* L: ... error */
                lua_pop(L, 1);                                              // remove error message from stack
            }
        }

        // Return all the results and remove them from the stack
        return Pop<_Ret...>(0);
    }
} // lua

#endif // LUA_SUPPORT_H
