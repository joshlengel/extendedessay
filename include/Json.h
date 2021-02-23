#pragma once

#include<string>
#include<vector>
#include<ostream>

class JsonObject
{
public:
    JsonObject(const JsonObject &parent):
        m_num_members(0),
        m_os(parent.m_os)
    {}

    virtual void Begin() const
    {
        m_os << "{\n";
        ++num_tabs;
    }

    virtual void Write(const std::string &name)
    {
        if (m_num_members > 0)
            m_os << ",\n";
        
        for (uint32_t i = 0; i < num_tabs; ++i)
            m_os << '\t';
        
        m_os << '\"' << name << "\":";
        ++m_num_members;
    }

    virtual void End() const
    {
        m_os << '\n';
        --num_tabs;
        for (uint32_t i = 0; i < num_tabs; ++i)
            m_os << '\t';
        m_os << '}';
    }

protected:
    uint32_t m_num_members;

    std::ostream &m_os;

    static uint32_t num_tabs;

    JsonObject(std::ostream &os):
        m_num_members(0),
        m_os(os)
    {}
};

template <typename T>
class JsonPrimitive : public JsonObject
{
public:
    JsonPrimitive(const JsonObject &parent, T val):
        JsonObject(parent),
        m_val(val)
    {}

    virtual void Begin() const { m_os << m_val; }
    virtual void Write(const std::string &name) {}
    virtual void End() const { }

protected:
    T m_val;
};

class JsonArray : public JsonObject
{
public:
    JsonArray(const JsonObject &parent):
        JsonObject(parent)
    {}

    virtual void Begin() const
    {
        m_os << '[';
        ++num_tabs;
    }

    virtual void Write(const std::string &name) {}

    template <typename InItr>
    void WritePrimitiveArray(const InItr &begin, const InItr &end)
    {
        --num_tabs;
        if (begin == end) return;

        if (m_num_members == 0)
        {
            JsonPrimitive primitive(*this, *begin);
            primitive.Begin(); primitive.End();
            ++m_num_members;
        }

        auto itr = begin; ++itr;
        for (; itr != end; ++itr)
        {
            ++m_num_members;
            JsonPrimitive primitive(*this, *itr);
            m_os << ',';
            primitive.Begin(); primitive.End();
        }
    }

    void WriteElem()
    {
        if (m_num_members++ > 0)
            m_os << ',';
    }

    virtual void End() const
    {
        m_os << ']';
    }
};

class JsonWriter : public JsonObject
{
public:
    JsonWriter(std::ostream &os):
        JsonObject(os)
    {}
};