#ifndef STREN_LUA_VALUE_H
#define STREN_LUA_VALUE_H

#include "string_ext.h"
#include "assert.h"

#include "lua_ext.h"

namespace lua
{
///
/// class Value
///
class Value
{
private:
    enum class Type
    {
        Nil,
        Bool,
        Int,
        Double,
        String,
        UserData,
        Table,
        Function
    };                          ///< possible lua values list

    Type        m_type;         ///< value type
    int         m_iValue;       ///< lua integer value or boolean or table, function reference
    void *      m_userData;     ///< pointer to light user data
    double      m_dValue;       ///< lua number value
    std::string m_strValue;     ///< lua string value
public:
    ///
    /// Create nil value
    ///
    Value();
    ///
    /// Create bool value
    ///
    Value(const bool value);
    ///
    /// Create number value
    ///
    Value(const int value);
    ///
    /// Create reference to the table or function
    ///
    Value(const int value, const bool isTable);
    ///
    /// Create number value
    ///
    Value(const size_t value);

    /// Create number value
    Value(const long value);

    /// Create number value
    Value(const double value);

    /// Create number value
    Value(const float value);

    /// Create string value
    Value(const char * value);

    /// Create string value
    Value(const std::string & value);

    /// Create value with user data
    Value(void * userdata);

    /// check if value is nil
    inline bool isNil() const { return Type::Nil == m_type; }

    /// check if value is boolean
    inline bool isBool() const { return Type::Bool == m_type; }

    /// check if value is number
    inline bool isNumber() const { return Type::Double == m_type || Type::Int == m_type; }

    /// check if value is string
    inline bool isString() const { return Type::String == m_type; }

    /// check if value is user data
    inline bool isUserData() const { return Type::UserData == m_type; }

    /// check if value is table
    inline bool isTable() const { return Type::Table == m_type; }

    /// check if value is function
    inline bool isFunction() const { return Type::Function == m_type; }

    /// check if value is integer
    inline bool isInt() const { return Type::Int == m_type; }

    /// check if value is double
    inline bool isDouble() const { return Type::Double == m_type; }

    /// return boolean value
    inline bool getBool() const { return 1 == m_iValue; }

    /// return integer value
    inline int getInt() const { return m_iValue; }

    /// return double value
    inline double getDouble() const { return m_dValue; }

    /// return string value
    inline const std::string & getString() const { return m_strValue; }
    ///
    /// return pointer to user data
    ///
    inline void * getUserData() const { return m_userData; }
    ///
    /// return reference to a table or to a function
    ///
    int getReference() const { return m_iValue; }
    ///
    /// compare operator
    ///
    bool operator<(const Value & other) const;
};
} // lua

#endif STREN_LUA_VALUE_H