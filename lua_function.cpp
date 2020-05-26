#include "lua_function.h"

#include "lua_wrapper.h"
#include "utils.h"

namespace lua
{
const std::vector<Value> Function::kEmptyParams;
std::vector<Value> Function::KEmptyResults;

Function::Function()
    : m_reference(LUA_NOREF)
{
}

Function::Function(const int reference)
    : m_reference(reference)
{
}

Function::Function(const std::string & path)
{
    Stack stack;
    m_reference = stack.createReference(path.c_str());
}

Function::Function(const char * path)
{
    Stack stack;
    m_reference = stack.createReference(path);
}

Function::Function(const Value & value)
{
    Stack stack;
    m_reference = stack.copyReference(value.getReference());
}

Function::Function(const Function & func)
{
    Stack stack;
    m_reference = stack.copyReference(func.m_reference);
}

Function::Function(Function && func)
    : m_reference(func.m_reference)
{
    func.m_reference = LUA_NOREF;
}

Function & Function::operator=(const Function & func)
{
    Stack stack;
    stack.deleteReference(m_reference);
    m_reference = stack.copyReference(func.m_reference);
    return *this;
}

Function & Function ::operator=(Function && func)
{
    m_reference = func.m_reference;
    func.m_reference = LUA_NOREF;
    return *this;
}

Function::~Function()
{
    if (m_reference != LUA_NOREF)
    {
        Stack stack;
        stack.deleteReference(m_reference);
    }
}

bool Function::call(const std::vector<Value> & params, std::vector<Value> & results)
{
    Stack stack;
    return stack.callFunction(m_reference, params, results);
}

} // lua