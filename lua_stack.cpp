#include "lua_stack.h"
#include "lua_value.h"
#include "utils.h"

namespace lua
{
lua_State * L = nullptr;

// class Stack
Stack::Stack(const int minStackSize)
{
    create();
    m_luaState = L;
    stren::assertMessage(getSize() >= minStackSize, "Wrong stack size");
}

void Stack::create()
{
    if (!L)
    {
        L = lua_open();
        stren::assertMessage(nullptr != L, "Failed to create lua virtual machine");
        luaL_openlibs(L);
    }
}

void Stack::destroy()
{
    if (m_luaState)
    {
        collectGarbage();
        lua_close(m_luaState);
        m_luaState = nullptr;
        L = nullptr;
    }
}

void Stack::loadLibs(const char * id, const luaL_reg * regs)
{
    if (m_luaState)
    {
        luaL_register(m_luaState, id, regs);
        pop(1);
    }
}

int Stack::getAllocatedMemory()
{
    return m_luaState ? lua_gc(m_luaState, LUA_GCCOUNT, 0) : 0;
}

void Stack::collectGarbage()
{
    if (m_luaState)
    {
        lua_gc(m_luaState, LUA_GCCOLLECT, 0);
    }
}

void Stack::loadScript(const char * name)
{
    if (!m_luaState) return;

    if (luaL_dofile(m_luaState, name))
    {
        std::string errorMsg;
        if (1 == lua_isstring(m_luaState, -1))
        {
            errorMsg = lua_tostring(m_luaState, -1);
        }
        else
        {
            errorMsg = name;
        }
        pop(1);
        stren::assertMessage(false, errorMsg.c_str());
    }
}

Value Stack::get(const int index)
{
    if (m_luaState)
    {
        if (1 == lua_isnumber(m_luaState, index))
        {
            const double numberValue = lua_tonumber(m_luaState, index);
            if (numberValue == (int)numberValue)
            {
                return Value(lua_tointeger(m_luaState, index));
            }
            else
            {
                return Value(numberValue);
            }
        }
        else if (lua_isboolean(m_luaState, index))
        {
            return Value(1 == lua_toboolean(m_luaState, index));
        }
        else if (1 == lua_isstring(m_luaState, index))
        {
            return Value(lua_tostring(m_luaState, index));
        }
        else if (1 == lua_isuserdata(m_luaState, index))
        {
            return Value(lua_touserdata(m_luaState, index));
        }
        else if (lua_istable(m_luaState, index))
        {
            lua_pushvalue(m_luaState, index);
            return Value(lua_ref(m_luaState, LUA_REGISTRYINDEX), true);
        }
        else if (lua_isfunction(m_luaState, index))
        {
            lua_pushvalue(m_luaState, index);
            return Value(lua_ref(m_luaState, LUA_REGISTRYINDEX), false);
        }
        else if (lua_isnil(m_luaState, index))
        {
            return Value();
        }
    }
    return Value();
}

int Stack::getSize() const
{
    return m_luaState ? lua_gettop(m_luaState) : 0;
}

bool Stack::isEmpty() const
{
    return 0 == lua_gettop(m_luaState);
}

void Stack::checkSizeOrDie(const int size) const
{
    stren::assertMessage(size == lua_gettop(m_luaState), "Wrong stack size");
}

void Stack::checkEmptyOrDie() const
{
    stren::assertMessage(0 == lua_gettop(m_luaState), "Stack is not empty");
}

int Stack::createTable()
{
    if (m_luaState)
    {
        lua_newtable(m_luaState);
        return lua_ref(m_luaState, LUA_REGISTRYINDEX);
    }
    return LUA_NOREF;
}

void Stack::makeTableGlobal(const int reference, const char * name)
{
    if (m_luaState)
    {
        lua_getref(m_luaState, reference);
        lua_setglobal(m_luaState, name);
    }
}

void Stack::deleteReference(const int reference)
{
    if (m_luaState)
    {
        lua_unref(m_luaState, reference);
    }
}

size_t Stack::getObjectSize(const int reference)
{
    if (m_luaState)
    {
        lua_getref(m_luaState, reference);
        const size_t len = lua_objlen(m_luaState, -1);
        pop(1);
        return len;
    }
    return 0;
}

void Stack::pop(const int n)
{
    if (m_luaState)
    {
        lua_pop(m_luaState, n);
    }
}

void Stack::clear()
{
    if (m_luaState)
    {
        lua_pop(m_luaState, lua_gettop(m_luaState));
    }
}

void Stack::push()
{
    if (m_luaState)
    {
        lua_pushnil(m_luaState);
    }
}

void Stack::push(const bool value)
{
    if (m_luaState)
    {
        lua_pushboolean(m_luaState, value);
    }
}

void Stack::push(void * value)
{
    if (m_luaState)
    {
        if (value)
        {
            lua_pushlightuserdata(m_luaState, value);
        }
        else
        {
            lua_pushnil(m_luaState);
        }
    }
}

void Stack::push(const std::string & value)
{
    push(value.c_str());
}

void Stack::push(const char * value)
{
    if (m_luaState)
    {
        lua_pushstring(m_luaState, value);
    }
}

void Stack::push(const int value)
{
    if (m_luaState)
    {
        lua_pushinteger(m_luaState, value);
    }
}

void Stack::push(const size_t value)
{
    if (m_luaState)
    {
        lua_pushinteger(m_luaState, (int)value);
    }
}

void Stack::push(const long value)
{
    if (m_luaState)
    {
        lua_pushinteger(m_luaState, (int)value);
    }
}

void Stack::push(const float value)
{
    if (m_luaState)
    {
        lua_pushnumber(m_luaState, (double)value);
    }
}

void Stack::push(const double value)
{
    if (m_luaState)
    {
        lua_pushnumber(m_luaState, value);
    }
}

void Stack::push(const Value & value)
{
    if (!m_luaState) return;

    if (value.isBool())
    {
        push(value.getBool());
    }
    else if (value.isString())
    {
        push(value.getString());
    }
    else if (value.isInt())
    {
        push(value.getInt());
    }
    else if (value.isDouble())
    {
        push(value.getDouble());
    }
    else if (value.isUserData())
    {
        push(value.getUserData());
    }
    else if (value.isNil())
    {
        push();
    }
}

bool Stack::callFunction(const int reference, const std::vector<Value> & params, std::vector<Value> & results)
{
    lua_getref(m_luaState, reference);
    stren::assertMessage(lua_isfunction(m_luaState, -1), "[lua] function not found");

    for (auto & param : params)
    {
        push(param);
    }

    // lua_pcall pops function and params from the stack
    if (0 != lua_pcall(m_luaState, params.size(), results.size(), 0))
    {
        std::string errorMsg;
        if (1 == lua_isstring(m_luaState, -1))
        {
            errorMsg = lua_tostring(m_luaState, -1);
        }
        else
        {
            errorMsg = "Lua function crashed";
        }
        pop(1);
        stren::assertMessage(false, errorMsg.c_str());
        return false;
    }

    for (Value & res : results)
    {
        res = get(-1);
        pop(1);
    }
    return true;
}

int Stack::copyTable(const int reference)
{
    if (m_luaState)
    {
        lua_newtable(m_luaState);            // create new table on top of the stack
        lua_getref(m_luaState, reference);   // pushes onto the stack the table from reference

        stren::assertMessage(lua_istable(m_luaState, -1), "[lua] table not found");

        // push first key
        push();
        while (0 != lua_next(m_luaState, -2))
        {
            lua_pushvalue(m_luaState, -2);   // create key copy
            lua_insert(m_luaState, -2);      // relocate key
            lua_settable(m_luaState, -5);    // removes key and value from stack, key copy remains
        }
        pop(1);

        return lua_ref(m_luaState, LUA_REGISTRYINDEX);
    }

    return LUA_NOREF;
}

int Stack::copyReference(const int reference)
{
    lua_getref(m_luaState, reference);
    return lua_ref(m_luaState, LUA_REGISTRYINDEX);
}

int Stack::createReference(const char * path)
{
    if (!m_luaState) return 0;

    std::vector<std::string> tokens;
    stren::tokenize(path, tokens, ".");

    const int stack_size = lua_gettop(m_luaState);
    for (size_t i = 0, iEnd = tokens.size(); i < iEnd; ++i)
    {
        const std::string & tokenStr = tokens[i];
        const char * token = tokenStr.c_str();
        if (!i)
        {
            lua_getglobal(m_luaState, token);
        }
        else
        {
            if (lua_istable(m_luaState, -1))
            {
                push(token);
                lua_gettable(m_luaState, -2);
            }
        }
    }
    int stackIncrement = lua_gettop(m_luaState) - stack_size;
    // if not all elements were loaded than something went wrong - clear the stack
    if (stackIncrement != tokens.size())
    {
        lua_pop(m_luaState, stackIncrement);
        stackIncrement = 0;
        
        stren::assertMessage(false, "Function loaded with error");
    }

    // create reference to the top object in the stack, pops object
    const int reference = lua_ref(m_luaState, LUA_REGISTRYINDEX);
    // remove all objects from the stack, top object already popped
    lua_pop(m_luaState, stackIncrement - 1);
    return reference;
}

void Stack::getTableKeys(const int reference, ValueVector & keys)
{
    lua_getref(m_luaState, reference);

    stren::assertMessage(lua_istable(m_luaState, -1), "[lua] table not found");

    // push first key
    push();
    while (0 != lua_next(m_luaState, -2))
    {
        // get and store key value
        keys.push_back(get(-2));
        // removes 'value'; keeps 'key' for next iteration
        pop(1);
    }
    pop(1);
}

void Stack::setTable(const int reference, const Value & key, const Value & value)
{
    lua_getref(m_luaState, reference);

    stren::assertMessage(lua_istable(m_luaState, -1), "[lua] table not found");

    push(key);
    push(value);
    lua_rawset(m_luaState, -3);
    pop(1);
}

Value Stack::getTable(const int reference, const Value & key)
{
    lua_getref(m_luaState, reference);

    stren::assertMessage(lua_istable(m_luaState, -1), "[lua] table not found");

    push(key);
    lua_rawget(m_luaState, -2);
    Value value = get(-1);
    lua_pop(m_luaState, 2);
    return value;
}

void Stack::tableToMap(const int reference, ValueMap & data)
{
    lua_getref(m_luaState, reference);

    stren::assertMessage(lua_istable(m_luaState, -1), "[lua] table not found");

    // first key
    push();
    while (0 != lua_next(m_luaState, -2))
    {
        // uses 'key' (at index -2) and 'value' (at index -1)
        Value key = get(-2);
        data[key] = get(-1);

        // removes 'value'; keeps 'key' for next iteration
        pop(1);
    }
    pop(1);
}

void Stack::tableToVector(const int reference, ValueVector & data)
{
    lua_getref(m_luaState, reference);

    stren::assertMessage(lua_istable(m_luaState, -1), "[lua] table not found");

    const size_t size = lua_objlen(m_luaState, -1);

    for (int i = 1, i_end = size; i <= i_end; ++i)
    {
        lua_rawgeti(m_luaState, -1, i);
        data.push_back(get(-1));
        pop(1);
    }
    pop(1);
}

bool Stack::isTableEmpty(const int reference)
{
    lua_getref(m_luaState, reference);

    stren::assertMessage(lua_istable(m_luaState, -1), "[lua] table not found");

    // first key
    push();
    const bool isEmpty(0 == lua_next(m_luaState, -2));
    const int stackSize = isEmpty ? 1 : 3; // if empty - only table in stack, else - table, key, value
    lua_pop(m_luaState, stackSize);
    return isEmpty;
}

} // lua
