#include "lua_value.h"

namespace lua
{
Value::Value()
    : m_type(Type::Nil)
    , m_iValue(0)
    , m_dValue(0.f)
    , m_strValue("nil")
    , m_userData(nullptr)
{
}

Value::Value(const bool value)
    : m_type(Type::Bool)
    , m_iValue(value ? 1 : 0)
    , m_dValue(0.f)
    , m_strValue(value ? "true" : "false")
    , m_userData(nullptr)
{
}

Value::Value(const int value)
    : m_type(Type::Int)
    , m_iValue(value)
    , m_dValue(value)
    , m_strValue(std::to_string(value))
    , m_userData(nullptr)
{
}

Value::Value(const size_t value)
    : m_type(Type::Int)
    , m_iValue(static_cast<int>(value))
    , m_dValue(static_cast<int>(value))
    , m_strValue(std::to_string(value))
    , m_userData(nullptr)
{
}

Value::Value(const long value)
    : m_type(Type::Int)
    , m_iValue(static_cast<int>(value))
    , m_dValue(static_cast<int>(value))
    , m_strValue(std::to_string(value))
    , m_userData(nullptr)
{
}

Value::Value(const double value)
    : m_type(Type::Double)
    , m_iValue(static_cast<int>(value))
    , m_dValue(value)
    , m_strValue(std::to_string(value))
    , m_userData(nullptr)
{
}

Value::Value(const float value)
    : m_type(Type::Double)
    , m_iValue(static_cast<int>(value))
    , m_dValue(static_cast<double>(value))
    , m_strValue(std::to_string(value))
    , m_userData(nullptr)
{
}

Value::Value(const char * value)
    : m_type(Type::String)
    , m_iValue(LUA_NOREF)
    , m_dValue(0.f)
    , m_strValue(value)
    , m_userData(nullptr)
{
}

Value::Value(const std::string & value)
    : m_type(Type::String)
    , m_iValue(LUA_NOREF)
    , m_dValue(0.f)
    , m_strValue(value)
    , m_userData(nullptr)
{
}

Value::Value(void * userdata)
    : m_type(Type::UserData)
    , m_iValue(LUA_NOREF)
    , m_dValue(0.f)
    , m_strValue("userdata")
    , m_userData(userdata)
{
}

Value::Value(const int value, const bool isTable)
    : m_type(isTable ? Type::Table : Type::Function)
    , m_iValue(value)
    , m_dValue(0.f)
    , m_strValue(isTable ? "table" : "function")
    , m_userData(nullptr)
{
}

bool Value::operator<(const Value & other) const
{
    return m_type < other.m_type ||
        m_iValue < other.m_iValue ||
        m_dValue < other.m_dValue ||
        m_userData < other.m_userData ||
        m_strValue < other.m_strValue;
}

} // lua
