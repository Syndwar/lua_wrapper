#include "lua_table.h"
#include "lua_stack.h"
#include "lua_value.h"

#include "utils.h"

#define kTablePathSeparator "."

namespace lua
{
Table::Table()
    : m_reference(LUA_NOREF)
{
}

Table::Table(const int reference)
    : m_reference(reference)
{
}

Table::Table(const Table & tbl)
{
    Stack stack;
    m_reference = stack.copyReference(tbl.m_reference);
}

Table::Table(Table && tbl)
    : m_reference(tbl.m_reference)
{
    tbl.m_reference = LUA_NOREF;
}

Table::Table(const std::string & path)
{
    Stack stack;
    m_reference = stack.createReference(path.c_str());
}

Table::Table(const char * path)
{
    Stack stack;
    m_reference = stack.createReference(path);
}

Table::Table(const Value & value)
{
    Stack stack;
    m_reference = stack.copyReference(value.getReference());
}

Table::~Table()
{
    if (m_reference != LUA_NOREF)
    {
        Stack stack;
        stack.deleteReference(m_reference);
    }
}

void Table::create()
{
    Stack stack;
    m_reference = stack.createTable();
}

void Table::create(const char * name)
{
    Stack stack;
    m_reference = stack.createTable();
    stack.makeTableGlobal(m_reference, name);
}

Table & Table::operator=(const Table & tbl)
{
    Stack stack;
    stack.deleteReference(m_reference);
    m_reference = stack.copyReference(tbl.m_reference);
    return *this;
}


Table & Table ::operator=(Table && tbl)
{
    m_reference = tbl.m_reference;
    tbl.m_reference = LUA_NOREF;
    return *this;
}

size_t Table::getSize() const
{
    Stack stack;
    return stack.getObjectSize(m_reference);
}

bool Table::isEmpty() const
{
    Stack stack;
    return stack.isTableEmpty(m_reference);
}

bool Table::isValid() const
{
    return (m_reference != LUA_NOREF) && (m_reference != LUA_REFNIL);
}

Value Table::get(const Value & key) const
{
    Stack stack;
    return stack.getTable(m_reference, key);
}

Value Table::get(const Value & key, const Value & defaultValue) const
{
    const Value value = get(key);
    if (value.isNil())
    {
        return defaultValue;
    }
    return value;
}

bool Table::hasKey(const Value & key) const
{
    Stack stack;
    Value value = stack.getTable(m_reference, key);
    return !value.isNil();
}

void Table::getKeys(std::vector<Value> & keys)
{
    keys.clear();

    Stack stack;
    stack.getTableKeys(m_reference, keys);
}

void Table::set(const Value & key, const Value & value)
{
    Stack stack;
    stack.setTable(m_reference, key, value);
}

void Table::fill(std::vector<Value> & data) const
{
    data.clear();

    Stack stack;
    stack.tableToVector(m_reference, data);
}

void Table::fill(std::map<Value, Value> & data) const
{
    data.clear();

    Stack stack;
    stack.tableToMap(m_reference, data);
}

Table Table::copy() const
{
    Stack stack;
    return Table(stack.copyTable(m_reference));
}
} // lua
