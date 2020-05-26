#ifndef STREN_LUA_FUNCTION_H
#define STREN_LUA_FUNCTION_H

#include <vector>

#include "lua_ext.h"

#include "string_ext.h"

namespace lua
{
class Value;
class Stack;
///
/// class Function
///
class Function
{

public:
    static const std::vector<Value> kEmptyParams;       ///< @todo
private:
    static std::vector<Value> KEmptyResults;            ///< @todo
    int                       m_reference;              ///< @todo
public:
    ///
    /// Default Constructor
    ///
    Function();
    ///
    /// Constructor using reference to function
    ///
    Function(const int reference);
    ///
    /// Constructor using path to function in lua table
    ///
    Function(const std::string & path);
    ///
    /// Constructor using path to function in lua table
    ///
    Function(const char * path);
    ///
    /// Constructor using Value
    ///
    Function(const Value & value);
    ///
    /// Copy Constructor
    ///
    Function(const Function & func);
    ///
    /// Move Constructor
    ///
    Function(Function && fuc);
    ///
    /// copy one function into another
    ///
    Function & operator=(const Function & func);
    ///
    /// move one table into another
    ///
    Function & operator=(Function && func);
    ///
    /// Destructor
    ///
    ~Function();
    ///
    /// call lua function with the given parameters and a vector for the results
    ///
    bool call(const std::vector<Value> & params = kEmptyParams, std::vector<Value> & results = KEmptyResults);
};
} // lua

#endif // STREN_LUA_FUNCTION_H
