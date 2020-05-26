#ifndef STREN_LUA_VM_H
#define STREN_LUA_VM_H

#include "lua_ext.h"
#include <vector>
#include <map>

namespace lua
{
class Value;
typedef std::vector<Value> ValueVector;
typedef std::map<Value, Value> ValueMap;
///
/// class Stack
///
class Stack
{
private:
    lua_State * m_luaState; ///< @todo
public:
    ///
    /// Constructor
    ///
    Stack(const int minStackSize = 0);
    ///
    /// copy table from reference into a new table and return reference to the new table
    ///
    int copyTable(const int reference);
    ///
    /// destroy lua virtual machine
    ///
    void destroy();
    ///
    /// load libs
    ///
    void loadLibs(const char * id, const luaL_reg * regs);
    ///
    /// get memory usage of lua virtual machine
    ///
    int getAllocatedMemory();
    ///
    /// collect garbage in lua
    ///
    void collectGarbage();
    ///
    /// load lua script from file
    ///
    void loadScript(const char * name);
    ///
    /// copy reference
    ///
    int copyReference(const int reference);
    ///
    /// create reference from path
    ///
    int createReference(const char * path);
    ///
    /// get value from stack
    ///
    Value get(const int index);
    ///
    /// get keys from table
    ///
    void getTableKeys(const int reference, ValueVector & keys);
    ///
    /// set value to the table
    ///
    void setTable(const int reference, const Value & key, const Value & value);
    ///
    /// get value from the table
    ///
    Value getTable(const int reference, const Value & key);
    ///
    /// check if table is empty
    ///
    bool isTableEmpty(const int reference);
    ///
    /// convert table to map
    ///
    void tableToMap(const int reference, ValueMap & data);
    ///
    /// convert table to vector
    ///
    void tableToVector(const int reference, ValueVector & data);
    ///
    /// pop n elements from stack
    ///
    void pop(const int n);
    ///
    /// delete all from stack
    ///
    void clear();
    ///
    /// push nil
    ///
    void push();
    ///
    /// push boolean value
    ///
    void push(const bool value);
    ///
    /// push light user data
    ///
    void push(void * value);
    ///
    /// push string
    ///
    void push(const char * value);
    ///
    /// push string
    ///
    void push(const std::string & value);
    ///
    /// push integer
    ///
    void push(const int value);
    ///
    /// push size_t as integer
    ///
    void push(const size_t value);
    ///
    /// push long as integer
    ///
    void push(const long value);
    ///
    /// push float as number
    ///
    void push(const float value);
    ///
    /// push double as number
    ///
    void push(const double value);
    ///
    /// push lua value
    ///
    void push(const Value & value);
    ///
    /// get amount of elements in the stack
    ///
    int getSize() const;
    ///
    /// check if stack has no elements
    ///
    bool isEmpty() const;
    ///
    /// check stack size and crash if it is not equal to "size"
    ///
    void checkSizeOrDie(const int size) const;
    ///
    /// check stack size and crash if it is not empty
    ///
    void checkEmptyOrDie() const;
    ///
    /// create table and return reference to it
    ///
    int createTable();
    ///
    /// make table from reference global
    ///
    void makeTableGlobal(const int reference, const char * name);
    ///
    /// delete reference from registry
    ///
    void deleteReference(const int reference);
    ///
    /// get object size
    ///
    size_t getObjectSize(const int reference);
    ///
    /// call lua function
    ///
    bool callFunction(const int reference, const std::vector<Value> & params, std::vector<Value> & results);
private:
    ///
    /// create lua virtual machine
    ///
    void create();
};
} // lua

#endif // STREN_LUA_VM_H
