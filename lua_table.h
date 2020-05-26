#ifndef STREN_LUA_TABLE_H
#define STREN_LUA_TABLE_H

#include "lua_stack.h"

#include <vector>
#include <map>

namespace lua
{
class Value;

///
/// class Table
///
class Table
{
private:
    int m_reference; ///< WRITEME
public:
    ///
    /// create a new table
    ///
    Table();
    ///
    /// create a table using reference to another table
    ///
    Table(const int reference);
    ///
    /// create table using another table
    ///
    Table(const Table & tbl);
    ///
    /// move given table into the current one
    ///
    Table(Table && tbl);
    ///
    /// load table using path to table in lua
    ///
    Table(const char * path);
    ///
    /// load table using path to table in lua
    ///
    Table(const std::string & path);
    ///
    /// create table using value
    ///
    Table(const Value & value);
    ///
    /// check if table is empty
    ///
    bool isEmpty() const;
    ///
    /// copy table recursively
    ///
    Table copy() const;
    ///
    /// create new table
    ///
    void create();
    ///
    /// create new global table
    ///
    void create(const char * name);
    ///
    /// destroy the table
    ///
    ~Table();
    ///
    /// copy one table into another
    ///
    Table & operator=(const Table & tbl);
    ///
    /// move one table into another
    ///
    Table & operator=(Table && tbl);
    ///
    /// get reference of the table
    ///
    inline int getRef() const { return m_reference; }
    ///
    /// get table size
    ///
    size_t getSize() const;
    ///
    /// check if table has proper reference
    ///
    bool isValid() const;
    ///
    /// get keys from table
    ///
    void getKeys(std::vector<Value> & keys);
    ///
    /// get value from table using key
    ///
    Value get(const Value & key) const;
    ///
    /// get value from table using key, if the value does not exist - use default_value
    ///
    Value get(const Value & key, const Value & defaultValue) const;
    ///
    /// check if table has key
    ///
    bool hasKey(const Value & key) const;
    ///
    /// set t[key] = value into the table
    ///
    void set(const Value & key, const Value & value);
    ///
    /// fill vector with table values
    ///
    void fill(std::vector<Value> & data) const;
    ///
    /// fill map with table keys and values
    ///
    void fill(std::map<Value, Value> & data) const;
};
} // lua

#endif // STREN_LUA_TABLE_H