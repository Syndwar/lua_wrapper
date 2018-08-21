#ifndef LUA_TABLE_H
#define LUA_TABLE_H

#include "lua_support.h"

extern lua_State *L;

namespace lua
{
    class Table
    {
    private:
        int ref = LUA_NOREF;

    public:
        Table();

        Table(int unique_ref);

        Table(const Table& table);

        Table(const std::string& table_id);

        virtual ~Table();

        const Table& operator=(const Table& table);

        int GetRef() const;

        size_t GetSize() const;

        template<typename _T>
        bool Has(const _T & key) const
        {
            lua_getref(L, ref);
            
            lua::PushValue(L, key);
            lua_rawget(L, -2);
            const bool has_value = !lua_isnil(L, -1);

            lua_pop(L, 2);

            CHECK_LUA_STACK_EMPTY(L);

            return has_value;
        }

        template<typename _T1, typename _T2>
        _T1 Get(const _T2 & key, const _T1 & def) const
        {
            lua_getref(L, ref);

            assert((1 == lua_istable(L, -1)) && "[lua] table not found");

            _T1 value(def);

            lua::PushValue(L, key);

            lua_rawget(L, -2);

            if (!lua_isnil(L, -1))
                value = ReadValue<_T1>(L, -1);

            lua_pop(L, 2);

            CHECK_LUA_STACK_EMPTY(L);

            return value;
        }

        template<typename _T1, typename _T2>
        _T1 Get(const _T2 & key) const
        {
            lua_getref(L, ref);

            assert((1 == lua_istable(L, -1)) && "[lua] table not found");

            _T1 value;

            lua::PushValue(L, key);

            lua_rawget(L, -2);

            value = ReadValue<_T1>(L, -1);

            lua_pop(L, 2);
            CHECK_LUA_STACK_EMPTY(L);

            return value;
        }

        std::string GetString(const std::string & key);

        Table Copy() const;

        bool IsValid() const { return (ref != LUA_NOREF) && (ref != LUA_REFNIL); }

        bool IsEmpty() const;

        template<typename _Value>
        void Fill(_Value& v1, _Value& v2)
        {
            lua_getref(L, ref);
            assert((1 == lua_istable(L, -1)) && "[lua] table not found");
            
            lua_rawgeti(L, -1, 1);

            v1 = lua_isnil(L, -1) ? 0 : ReadValue<_Value>(L, -1);

            lua_rawgeti(L, -2, 2);

            v2 = lua_isnil(L, -1) ? 0 : ReadValue<_Value>(L, -1);

            lua_pop(L, 3);

            CHECK_LUA_STACK_EMPTY(L);
        }

        template<typename _Value>
        void Fill(_Value& v1, _Value& v2, _Value & v3, _Value & v4)
        {
            lua_getref(L, ref);
            assert((1 == lua_istable(L, -1)) && "[lua] table not found");

            lua_rawgeti(L, -1, 1);
            v1 = lua_isnil(L, -1) ? 0 : ReadValue<_Value>(L, -1);

            lua_rawgeti(L, -2, 2);
            v2 = lua_isnil(L, -1) ? 0 : ReadValue<_Value>(L, -1);

            lua_rawgeti(L, -3, 3);
            v3 = lua_isnil(L, -1) ? 0 : ReadValue<_Value>(L, -1);

            lua_rawgeti(L, -4, 4);
            v4 = lua_isnil(L, -1) ? 0 : ReadValue<_Value>(L, -1);

            lua_pop(L, 5);

            CHECK_LUA_STACK_EMPTY(L);
        }

        template<typename _Value>
        void Fill(std::vector<_Value>& data)
        {
            data.clear();

            lua_getref(L, ref);
            assert((1 == lua_istable(L, -1)) && "[lua] table not found");

            data.resize(GetSize());
            for (int i = 1, i_end = data.size(); i <= i_end; ++i)
            {
                lua_rawgeti(L, -1, i);
                data[i-1] = ReadValue<_Value>(L, -1);
                lua_pop(L, 1);
            }
            lua_pop(L, 1);

            CHECK_LUA_STACK_EMPTY(L);
        }

        template<typename _Value>
        void Fill(std::set<_Value>& data)
        {
            data.clear();

            lua_getref(L, ref);
            assert((1 == lua_istable(L, -1)) && "[lua] table not found");

            for (int i = 1, i_end = GetSize(); i <= i_end; ++i)
            {
                lua_rawgeti(L, -1, i);
                data.insert(ReadValue<_Value>(L, -1));
                lua_pop(L, 1);
            }
            lua_pop(L, 1);

            CHECK_LUA_STACK_EMPTY(L);
        }

        template<typename _Key, typename _Value>
        void Fill(std::map<_Key, _Value>& data)
        {
            data.clear();

            lua_getref(L, ref);
            assert((1 == lua_istable(L, -1)) && "[lua] table not found");

            lua_pushnil(L);  /* first key */
            while (0 != lua_next(L, -2))
            {
                /* uses 'key' (at index -2) and 'value' (at index -1) */

                _Key key = ReadValue<_Key>(L, -2); // Peek<_Key>(-2);
                _Value value = ReadValue<_Value>(L, -1); // Peek<_Value>(-1);

                data[key] = value;

                /* removes 'value'; keeps 'key' for next iteration */
                lua_pop(L, 1);
            }
            lua_pop(L, 1);

            CHECK_LUA_STACK_EMPTY(L);
        }

        template<typename _Key>
        void Keys(std::vector<_Key>& keys)
        {
            keys.clear();

            lua_getref(L, ref);
            assert((1 == lua_istable(L, -1)) && "[lua] table not found");

            lua_pushnil(L);  /* first key */
            while (0 != lua_next(L, -2))
            {
                // uses 'key' (at index -2) and 'value' (at index -1)

                _Key key = ReadValue<_Key>(L, -2); // Peek<_Key>(-2);

                // store key
                keys.push_back(key);

                // removes 'value'; keeps 'key' for next iteration
                lua_pop(L, 1);
            }
            lua_pop(L, 1);

            CHECK_LUA_STACK_EMPTY(L);
        }

        template<typename _K, typename _V>
        void Set(_K key, _V value)
        {
            lua_getref(L, ref);

            assert((1 == lua_istable(L, -1)) && "[lua] table not found");

            lua::PushValue(L, key);
            lua::PushValue(L, value);

            lua_rawset(L, -3);

            lua_pop(L, 1);
            CHECK_LUA_STACK_EMPTY(L);
        }

    private:

        int Load(const char* table_name);
    };

    template<>
    inline Table ReadValue(lua_State* L, int index)
    {
        if (lua_istable(L, index))
        {
            int ref = lua_ref(L, LUA_REGISTRYINDEX);
            lua_pushnil(L);
            return Table(ref);
        }
        else
        {
            return Table(/*LUA_NOREF*/);
        }
    }
}

#endif
