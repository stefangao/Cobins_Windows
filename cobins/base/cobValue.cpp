/****************************************************************************
 Copyright (c) 2013-2017 Chukong Technologies
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
****************************************************************************/

#include <cmath>
#include <sstream>
#include <string.h>
#include <iomanip>
#include <float.h>
#include<stdlib.h>
#include "cobValue.h"
using namespace rapidjson;

NS_COB_BEGIN

const ValueVector ValueVectorNull;
const ValueMap ValueMapNull;
const ValueMapIntKey ValueMapIntKeyNull;
const Value Value::Null;

Value::Value()
: _type(Type::NONE)
{
    memset(&_field, 0, sizeof(_field));
}

Value::Value(unsigned char v)
: _type(Type::BYTE)
{
    _field.byteVal = v;
}

Value::Value(int v)
: _type(Type::INTEGER)
{
    _field.intVal = v;
}

Value::Value(long v)
        : _type(Type::LONG)
{
    _field.longVal = v;
}

Value::Value(unsigned long v)
        : _type(Type::ULONG)
{
    _field.ulongVal = v;
}

Value::Value(unsigned int v)
: _type(Type::UNSIGNED)
{
    _field.unsignedVal = v;
}

Value::Value(float v)
: _type(Type::FLOAT)
{
    _field.floatVal = v;
}

Value::Value(double v)
: _type(Type::DOUBLE)
{
    _field.doubleVal = v;
}

Value::Value(bool v)
: _type(Type::BOOLEAN)
{
    _field.boolVal = v;
}

Value::Value(const char* v)
: _type(Type::STRING)
{
    _field.strVal = new (std::nothrow) std::string();
    if (v)
    {
        *_field.strVal = v;
    }
}

Value::Value(const std::string& v)
: _type(Type::STRING)
{
    _field.strVal = new (std::nothrow) std::string();
    *_field.strVal = v;
}

Value::Value(const ValueVector& v)
: _type(Type::VECTOR)
{
    _field.vectorVal = new (std::nothrow) ValueVector();
    *_field.vectorVal = v;
}

Value::Value(ValueVector&& v)
: _type(Type::VECTOR)
{
    _field.vectorVal = new (std::nothrow) ValueVector();
    *_field.vectorVal = std::move(v);
}

Value::Value(const ValueMap& v)
: _type(Type::MAP)
{
    _field.mapVal = new (std::nothrow) ValueMap();
    *_field.mapVal = v;
}

Value::Value(ValueMap&& v)
: _type(Type::MAP)
{
    _field.mapVal = new (std::nothrow) ValueMap();
    *_field.mapVal = std::move(v);
}

Value::Value(const ValueMapIntKey& v)
: _type(Type::INT_KEY_MAP)
{
    _field.intKeyMapVal = new (std::nothrow) ValueMapIntKey();
    *_field.intKeyMapVal = v;
}

Value::Value(ValueMapIntKey&& v)
: _type(Type::INT_KEY_MAP)
{
    _field.intKeyMapVal = new (std::nothrow) ValueMapIntKey();
    *_field.intKeyMapVal = std::move(v);
}

Value::Value(const Value& other)
: _type(Type::NONE)
{
    *this = other;
}

Value::Value(Value&& other)
: _type(Type::NONE)
{
    *this = std::move(other);
}

Value::~Value()
{
    clear();
}

Value& Value::operator= (const Value& other)
{
    if (this != &other) {
        reset(other._type);

        switch (other._type) {
            case Type::BYTE:
                _field.byteVal = other._field.byteVal;
                break;
            case Type::INTEGER:
                _field.intVal = other._field.intVal;
                break;
            case Type::UNSIGNED:
                _field.unsignedVal = other._field.unsignedVal;
                break;
            case Type::LONG:
                _field.longVal = other._field.longVal;
                break;
            case Type::ULONG:
                _field.ulongVal = other._field.ulongVal;
                break;
            case Type::FLOAT:
                _field.floatVal = other._field.floatVal;
                break;
            case Type::DOUBLE:
                _field.doubleVal = other._field.doubleVal;
                break;
            case Type::BOOLEAN:
                _field.boolVal = other._field.boolVal;
                break;
            case Type::STRING:
                if (_field.strVal == nullptr)
                {
                    _field.strVal = new std::string();
                }
                *_field.strVal = *other._field.strVal;
                break;
            case Type::VECTOR:
                if (_field.vectorVal == nullptr)
                {
                    _field.vectorVal = new (std::nothrow) ValueVector();
                }
                *_field.vectorVal = *other._field.vectorVal;
                break;
            case Type::MAP:
                if (_field.mapVal == nullptr)
                {
                    _field.mapVal = new (std::nothrow) ValueMap();
                }
                *_field.mapVal = *other._field.mapVal;
                break;
            case Type::INT_KEY_MAP:
                if (_field.intKeyMapVal == nullptr)
                {
                    _field.intKeyMapVal = new (std::nothrow) ValueMapIntKey();
                }
                *_field.intKeyMapVal = *other._field.intKeyMapVal;
                break;
            default:
                break;
        }
    }
    return *this;
}

Value& Value::operator= (Value&& other)
{
    if (this != &other)
    {
        clear();
        switch (other._type)
        {
            case Type::BYTE:
                _field.byteVal = other._field.byteVal;
                break;
            case Type::INTEGER:
                _field.intVal = other._field.intVal;
                break;
            case Type::UNSIGNED:
                _field.unsignedVal = other._field.unsignedVal;
                break;
            case Type::LONG:
                _field.longVal = other._field.longVal;
                break;
            case Type::ULONG:
                _field.ulongVal = other._field.ulongVal;
                break;
            case Type::FLOAT:
                _field.floatVal = other._field.floatVal;
                break;
            case Type::DOUBLE:
                _field.doubleVal = other._field.doubleVal;
                break;
            case Type::BOOLEAN:
                _field.boolVal = other._field.boolVal;
                break;
            case Type::STRING:
                _field.strVal = other._field.strVal;
                break;
            case Type::VECTOR:
                _field.vectorVal = other._field.vectorVal;
                break;
            case Type::MAP:
                _field.mapVal = other._field.mapVal;
                break;
            case Type::INT_KEY_MAP:
                _field.intKeyMapVal = other._field.intKeyMapVal;
                break;
            default:
                break;
        }
        _type = other._type;

        memset(&other._field, 0, sizeof(other._field));
        other._type = Type::NONE;
    }

    return *this;
}

Value& Value::operator= (unsigned char v)
{
    reset(Type::BYTE);
    _field.byteVal = v;
    return *this;
}

Value& Value::operator= (int v)
{
    reset(Type::INTEGER);
    _field.intVal = v;
    return *this;
}

Value& Value::operator= (unsigned int v)
{
    reset(Type::UNSIGNED);
    _field.unsignedVal = v;
    return *this;
}

Value& Value::operator= (long v)
{
    reset(Type::LONG);
    _field.longVal = v;
    return *this;
}

Value& Value::operator= (unsigned long v)
{
    reset(Type::ULONG);
    _field.ulongVal = v;
    return *this;
}

Value& Value::operator= (float v)
{
    reset(Type::FLOAT);
    _field.floatVal = v;
    return *this;
}

Value& Value::operator= (double v)
{
    reset(Type::DOUBLE);
    _field.doubleVal = v;
    return *this;
}

Value& Value::operator= (bool v)
{
    reset(Type::BOOLEAN);
    _field.boolVal = v;
    return *this;
}

Value& Value::operator= (const char* v)
{
    reset(Type::STRING);
    *_field.strVal = v ? v : "";
    return *this;
}

Value& Value::operator= (const std::string& v)
{
    reset(Type::STRING);
    *_field.strVal = v;
    return *this;
}

Value& Value::operator= (const ValueVector& v)
{
    reset(Type::VECTOR);
    *_field.vectorVal = v;
    return *this;
}

Value& Value::operator= (ValueVector&& v)
{
    reset(Type::VECTOR);
    *_field.vectorVal = std::move(v);
    return *this;
}

Value& Value::operator= (const ValueMap& v)
{
    reset(Type::MAP);
    *_field.mapVal = v;
    return *this;
}

Value& Value::operator= (ValueMap&& v)
{
    reset(Type::MAP);
    *_field.mapVal = std::move(v);
    return *this;
}

Value& Value::operator= (const ValueMapIntKey& v)
{
    reset(Type::INT_KEY_MAP);
    *_field.intKeyMapVal = v;
    return *this;
}

Value& Value::operator= (ValueMapIntKey&& v)
{
    reset(Type::INT_KEY_MAP);
    *_field.intKeyMapVal = std::move(v);
    return *this;
}

bool Value::operator!= (const Value& v)
{
    return !(*this == v);
}
bool Value::operator!= (const Value& v) const
{
    return !(*this == v);
}

bool Value::operator== (const Value& v)
{
    const auto &t = *this;
    return t == v;
}
bool Value::operator== (const Value& v) const
{
    if (this == &v) return true;
    if (v._type != this->_type) return false;
    if (this->isNull()) return true;
    switch (_type)
    {
        case Type::BYTE:    return v._field.byteVal     == this->_field.byteVal;
        case Type::INTEGER: return v._field.intVal      == this->_field.intVal;
        case Type::UNSIGNED:return v._field.unsignedVal == this->_field.unsignedVal;
        case Type::LONG: return v._field.longVal == this->_field.longVal;
        case Type::ULONG:return v._field.ulongVal == this->_field.ulongVal;
        case Type::BOOLEAN: return v._field.boolVal     == this->_field.boolVal;
        case Type::STRING:  return *v._field.strVal     == *this->_field.strVal;
        case Type::FLOAT:   return std::abs(v._field.floatVal  - this->_field.floatVal)  <= FLT_EPSILON;
        case Type::DOUBLE:  return std::abs(v._field.doubleVal - this->_field.doubleVal) <= DBL_EPSILON;
        case Type::VECTOR:
        {
            const auto &v1 = *(this->_field.vectorVal);
            const auto &v2 = *(v._field.vectorVal);
            const auto size = v1.size();
            if (size == v2.size())
            {
                for (size_t i = 0; i < size; i++)
                {
                    if (v1[i] != v2[i]) return false;
                }
                return true;
            }
            return false;
        }
        case Type::MAP:
        {
            const auto &map1 = *(this->_field.mapVal);
            const auto &map2 = *(v._field.mapVal);
            for (const auto &kvp : map1)
            {
                auto it = map2.find(kvp.first);
                if (it == map2.end() || it->second != kvp.second)
                {
                    return false;
                }
            }
            return true;
        }
        case Type::INT_KEY_MAP:
        {
            const auto &map1 = *(this->_field.intKeyMapVal);
            const auto &map2 = *(v._field.intKeyMapVal);
            for (const auto &kvp : map1)
            {
                auto it = map2.find(kvp.first);
                if (it == map2.end() || it->second != kvp.second)
                {
                    return false;
                }
            }
            return true;
        }
        default:
            break;
    };

    return false;
}

/// Convert value to a specified type
unsigned char Value::asByte() const
{
    COBASSERT(_type != Type::VECTOR && _type != Type::MAP && _type != Type::INT_KEY_MAP, "Only base type (bool, string, float, double, int) could be converted");

    if (_type == Type::BYTE)
    {
        return _field.byteVal;
    }

    if (_type == Type::INTEGER)
    {
        return static_cast<unsigned char>(_field.intVal);
    }

    if (_type == Type::UNSIGNED)
    {
        return static_cast<unsigned char>(_field.unsignedVal);
    }

    if (_type == Type::LONG)
    {
        return static_cast<unsigned char>(_field.longVal);
    }

    if (_type == Type::ULONG)
    {
        return static_cast<unsigned char>(_field.ulongVal);
    }

    if (_type == Type::STRING)
    {
        return static_cast<unsigned char>(atoi(_field.strVal->c_str()));
    }

    if (_type == Type::FLOAT)
    {
        return static_cast<unsigned char>(_field.floatVal);
    }

    if (_type == Type::DOUBLE)
    {
        return static_cast<unsigned char>(_field.doubleVal);
    }

    if (_type == Type::BOOLEAN)
    {
        return _field.boolVal ? 1 : 0;
    }

    return 0;
}

int Value::asInt() const
{
    COBASSERT(_type != Type::VECTOR && _type != Type::MAP && _type != Type::INT_KEY_MAP, "Only base type (bool, string, float, double, int) could be converted");
    if (_type == Type::INTEGER)
    {
        return _field.intVal;
    }

    if (_type == Type::UNSIGNED)
    {
        COBASSERT(_field.unsignedVal < INT_MAX, "Can only convert values < INT_MAX");
        return (int)_field.unsignedVal;
    }

    if (_type == Type::LONG)
    {
        return static_cast<int>(_field.longVal);
    }

    if (_type == Type::ULONG)
    {
        return static_cast<int>(_field.ulongVal);
    }

    if (_type == Type::BYTE)
    {
        return _field.byteVal;
    }

    if (_type == Type::STRING)
    {
        return atoi(_field.strVal->c_str());
    }

    if (_type == Type::FLOAT)
    {
        return static_cast<int>(_field.floatVal);
    }

    if (_type == Type::DOUBLE)
    {
        return static_cast<int>(_field.doubleVal);
    }

    if (_type == Type::BOOLEAN)
    {
        return _field.boolVal ? 1 : 0;
    }

    return 0;
}


unsigned int Value::asUnsignedInt() const
{
    COBASSERT(_type != Type::VECTOR && _type != Type::MAP && _type != Type::INT_KEY_MAP, "Only base type (bool, string, float, double, int) could be converted");
    if (_type == Type::UNSIGNED)
    {
        return _field.unsignedVal;
    }

    if (_type == Type::INTEGER)
    {
        COBASSERT(_field.intVal >= 0, "Only values >= 0 can be converted to unsigned");
        return static_cast<unsigned int>(_field.intVal);
    }

    if (_type == Type::LONG)
    {
        return static_cast<unsigned int>(_field.longVal);
    }

    if (_type == Type::ULONG)
    {
        return static_cast<unsigned int>(_field.ulongVal);
    }

    if (_type == Type::BYTE)
    {
        return static_cast<unsigned int>(_field.byteVal);
    }

    if (_type == Type::STRING)
    {
        // NOTE: strtoul is required (need to augment on unsupported platforms)
        return static_cast<unsigned int>(strtoul(_field.strVal->c_str(), nullptr, 10));
    }

    if (_type == Type::FLOAT)
    {
        return static_cast<unsigned int>(_field.floatVal);
    }

    if (_type == Type::DOUBLE)
    {
        return static_cast<unsigned int>(_field.doubleVal);
    }

    if (_type == Type::BOOLEAN)
    {
        return _field.boolVal ? 1u : 0u;
    }

    return 0u;
}

long Value::asLong() const
{
    if (_type == Type::INTEGER)
    {
        return static_cast<long>(_field.intVal);
    }

    if (_type == Type::UNSIGNED)
    {
        return static_cast<long>(_field.unsignedVal);
    }

    if (_type == Type::LONG)
    {
        return _field.longVal;
    }

    if (_type == Type::ULONG)
    {
        return static_cast<long>(_field.ulongVal);
    }

    if (_type == Type::BYTE)
    {
        return _field.byteVal;
    }

    if (_type == Type::STRING)
    {
        return atol(_field.strVal->c_str());
    }

    if (_type == Type::FLOAT)
    {
        return static_cast<long>(_field.floatVal);
    }

    if (_type == Type::DOUBLE)
    {
        return static_cast<long>(_field.doubleVal);
    }

    if (_type == Type::BOOLEAN)
    {
        return _field.boolVal ? 1 : 0;
    }

    return 0;
}

unsigned long Value::asUlong() const
{
    if (_type == Type::INTEGER)
    {
        return static_cast<unsigned long>(_field.intVal);
    }

    if (_type == Type::UNSIGNED)
    {
        return static_cast<unsigned long>(_field.unsignedVal);
    }

    if (_type == Type::LONG)
    {
        return static_cast<unsigned long>(_field.longVal);
    }

    if (_type == Type::ULONG)
    {
        return _field.ulongVal;
    }

    if (_type == Type::BYTE)
    {
        return _field.byteVal;
    }

    if (_type == Type::STRING)
    {
        return atol(_field.strVal->c_str());
    }

    if (_type == Type::FLOAT)
    {
        return static_cast<unsigned long>(_field.floatVal);
    }

    if (_type == Type::DOUBLE)
    {
        return static_cast<unsigned long>(_field.doubleVal);
    }

    if (_type == Type::BOOLEAN)
    {
        return _field.boolVal ? 1 : 0;
    }

    return 0;
}

float Value::asFloat() const
{
    COBASSERT(_type != Type::VECTOR && _type != Type::MAP && _type != Type::INT_KEY_MAP, "Only base type (bool, string, float, double, int) could be converted");
    if (_type == Type::FLOAT)
    {
        return _field.floatVal;
    }

    if (_type == Type::BYTE)
    {
        return static_cast<float>(_field.byteVal);
    }

    if (_type == Type::STRING)
    {
        return (float)atof(_field.strVal->c_str());
    }

    if (_type == Type::INTEGER)
    {
        return static_cast<float>(_field.intVal);
    }

    if (_type == Type::UNSIGNED)
    {
        return static_cast<float>(_field.unsignedVal);
    }

    if (_type == Type::LONG)
    {
        return static_cast<float>(_field.longVal);
    }

    if (_type == Type::ULONG)
    {
        return static_cast<float>(_field.ulongVal);
    }

    if (_type == Type::DOUBLE)
    {
        return static_cast<float>(_field.doubleVal);
    }

    if (_type == Type::BOOLEAN)
    {
        return _field.boolVal ? 1.0f : 0.0f;
    }

    return 0.0f;
}

double Value::asDouble() const
{
    COBASSERT(_type != Type::VECTOR && _type != Type::MAP && _type != Type::INT_KEY_MAP, "Only base type (bool, string, float, double, int) could be converted");
    if (_type == Type::DOUBLE)
    {
        return _field.doubleVal;
    }

    if (_type == Type::BYTE)
    {
        return static_cast<double>(_field.byteVal);
    }

    if (_type == Type::STRING)
    {
        return static_cast<double>(atof(_field.strVal->c_str()));
    }

    if (_type == Type::INTEGER)
    {
        return static_cast<double>(_field.intVal);
    }

    if (_type == Type::UNSIGNED)
    {
        return static_cast<double>(_field.unsignedVal);
    }

    if (_type == Type::LONG)
    {
        return static_cast<double>(_field.longVal);
    }

    if (_type == Type::ULONG)
    {
        return static_cast<double>(_field.ulongVal);
    }

    if (_type == Type::FLOAT)
    {
        return static_cast<double>(_field.floatVal);
    }

    if (_type == Type::BOOLEAN)
    {
        return _field.boolVal ? 1.0 : 0.0;
    }

    return 0.0;
}

bool Value::asBool() const
{
    COBASSERT(_type != Type::VECTOR && _type != Type::MAP && _type != Type::INT_KEY_MAP, "Only base type (bool, string, float, double, int) could be converted");
    if (_type == Type::BOOLEAN)
    {
        return _field.boolVal;
    }

    if (_type == Type::BYTE)
    {
        return _field.byteVal == 0 ? false : true;
    }

    if (_type == Type::STRING)
    {
        return (*_field.strVal == "0" || *_field.strVal == "false") ? false : true;
    }

    if (_type == Type::INTEGER)
    {
        return _field.intVal == 0 ? false : true;
    }

    if (_type == Type::UNSIGNED)
    {
        return _field.unsignedVal == 0 ? false : true;
    }

    if (_type == Type::LONG)
    {
        return _field.longVal == 0 ? false : true;
    }

    if (_type == Type::ULONG)
    {
        return _field.ulongVal == 0 ? false : true;
    }

    if (_type == Type::FLOAT)
    {
        return _field.floatVal == 0.0f ? false : true;
    }

    if (_type == Type::DOUBLE)
    {
        return _field.doubleVal == 0.0 ? false : true;
    }

    return false;
}

std::string Value::asString() const
{
    COBASSERT(_type != Type::VECTOR && _type != Type::MAP && _type != Type::INT_KEY_MAP, "Only base type (bool, string, float, double, int) could be converted");

    if (_type == Type::STRING)
    {
        return *_field.strVal;
    }

    std::stringstream ret;

    switch (_type)
    {
        case Type::BYTE:
            ret << _field.byteVal;
            break;
        case Type::INTEGER:
            ret << _field.intVal;
            break;
        case Type::UNSIGNED:
            ret << _field.unsignedVal;
            break;
        case Type::LONG:
            ret << _field.longVal;
            break;
        case Type::ULONG:
            ret << _field.ulongVal;
            break;
        case Type::FLOAT:
            ret << std::fixed << std::setprecision( 7 )<< _field.floatVal;
            break;
        case Type::DOUBLE:
            ret << std::fixed << std::setprecision( 16 ) << _field.doubleVal;
            break;
        case Type::BOOLEAN:
            ret << (_field.boolVal ? "true" : "false");
            break;
        default:
            break;
    }
    return ret.str();
}

ValueVector& Value::asValueVector()
{
    COBASSERT(_type == Type::VECTOR, "The value type isn't Type::VECTOR");
    return *_field.vectorVal;
}

const ValueVector& Value::asValueVector() const
{
    COBASSERT(_type == Type::VECTOR, "The value type isn't Type::VECTOR");
    return *_field.vectorVal;
}

ValueMap& Value::asValueMap()
{
    COBASSERT(_type == Type::MAP, "The value type isn't Type::MAP");
    return *_field.mapVal;
}

const ValueMap& Value::asValueMap() const
{
    COBASSERT(_type == Type::MAP, "The value type isn't Type::MAP");
    return *_field.mapVal;
}

ValueMapIntKey& Value::asIntKeyMap()
{
    COBASSERT(_type == Type::INT_KEY_MAP, "The value type isn't Type::INT_KEY_MAP");
    return *_field.intKeyMapVal;
}

const ValueMapIntKey& Value::asIntKeyMap() const
{
    COBASSERT(_type == Type::INT_KEY_MAP, "The value type isn't Type::INT_KEY_MAP");
    return *_field.intKeyMapVal;
}

static std::string getTabs(int depth)
{
    std::string tabWidth;

    for (int i = 0; i < depth; ++i)
    {
        tabWidth += "\t";
    }

    return tabWidth;
}

static std::string visit(const Value& v, int depth);

static std::string visitVector(const ValueVector& v, int depth)
{
    std::stringstream ret;

    if (depth > 0)
        ret << "\n";

    ret << getTabs(depth) << "[\n";

    int i = 0;
    for (const auto& child : v)
    {
        ret << getTabs(depth+1) << i << ": " << visit(child, depth + 1);
        ++i;
    }

    ret << getTabs(depth) << "]\n";

    return ret.str();
}

template <class T>
static std::string visitMap(const T& v, int depth)
{
    std::stringstream ret;

    if (depth > 0)
        ret << "\n";

    ret << getTabs(depth) << "{\n";

    for (auto& iter : v)
    {
        ret << getTabs(depth + 1) << iter.first << ": ";
        ret << visit(iter.second, depth + 1);
    }

    ret << getTabs(depth) << "}\n";

    return ret.str();
}

static std::string visit(const Value& v, int depth)
{
    std::stringstream ret;

    switch (v.getType())
    {
        case Value::Type::NONE:
        case Value::Type::BYTE:
        case Value::Type::INTEGER:
        case Value::Type::UNSIGNED:
        case Value::Type::LONG:
        case Value::Type::ULONG:
        case Value::Type::FLOAT:
        case Value::Type::DOUBLE:
        case Value::Type::BOOLEAN:
        case Value::Type::STRING:
            ret << v.asString() << "\n";
            break;
        case Value::Type::VECTOR:
            ret << visitVector(v.asValueVector(), depth);
            break;
        case Value::Type::MAP:
            ret << visitMap(v.asValueMap(), depth);
            break;
        case Value::Type::INT_KEY_MAP:
            ret << visitMap(v.asIntKeyMap(), depth);
            break;
        default:
            COBASSERT(false, "Invalid type!");
            break;
    }

    return ret.str();
}

std::string Value::getDescription() const
{
    std::string ret("\n");
    ret += visit(*this, 0);
    return ret;
}

void Value::clear()
{
    // Free memory the old value allocated
    switch (_type)
    {
        case Type::BYTE:
            _field.byteVal = 0;
            break;
        case Type::INTEGER:
            _field.intVal = 0;
            break;
        case Type::UNSIGNED:
            _field.unsignedVal = 0u;
            break;
        case Type::LONG:
            _field.longVal = 0;
            break;
        case Type::ULONG:
            _field.ulongVal = 0u;
            break;
        case Type::FLOAT:
            _field.floatVal = 0.0f;
            break;
        case Type::DOUBLE:
            _field.doubleVal = 0.0;
            break;
        case Type::BOOLEAN:
            _field.boolVal = false;
            break;
        case Type::STRING:
            COB_SAFE_DELETE(_field.strVal);
            break;
        case Type::VECTOR:
            COB_SAFE_DELETE(_field.vectorVal);
            break;
        case Type::MAP:
            COB_SAFE_DELETE(_field.mapVal);
            break;
        case Type::INT_KEY_MAP:
            COB_SAFE_DELETE(_field.intKeyMapVal);
            break;
        default:
            break;
    }

    _type = Type::NONE;
}

void Value::reset(Type type)
{
    if (_type == type)
        return;

    clear();

    // Allocate memory for the new value
    switch (type)
    {
        case Type::STRING:
            _field.strVal = new (std::nothrow) std::string();
            break;
        case Type::VECTOR:
            _field.vectorVal = new (std::nothrow) ValueVector();
            break;
        case Type::MAP:
            _field.mapVal = new (std::nothrow) ValueMap();
            break;
        case Type::INT_KEY_MAP:
            _field.intKeyMapVal = new (std::nothrow) ValueMapIntKey();
            break;
        default:
            break;
    }

    _type = type;
}

//

Value ValueMapUtil::parseValueFromJsonValue(const rapidjson::Value& jvalue)
{
    // parse by type
    auto t = jvalue.GetType();

    if(t == rapidjson::Type::kNullType)
    {
        // don't add anything
        return Value();
    }

    if(t == rapidjson::Type::kFalseType) {
        return Value(false);
    }

    if(t == rapidjson::Type::kTrueType) {
        return Value(true);
    }

    if(t == rapidjson::Type::kStringType) {
        return Value(jvalue.GetString());
    }

    if(t == rapidjson::Type::kNumberType) {
        if(jvalue.IsDouble()) {
            return Value(jvalue.GetDouble());
        } else if(jvalue.IsUint()) {
            int temp = int(jvalue.GetUint());
            return Value(temp);
        } else if(jvalue.IsInt()) {
            return Value(jvalue.GetInt());
        } else if(jvalue.IsInt64()) {
            return Value((long)jvalue.GetInt64());
        } else if(jvalue.IsUint64()) {
            return Value((unsigned long)jvalue.GetUint64());
        }
    }

    if(t == rapidjson::Type::kObjectType) {
        ValueMap dict;
        for (rapidjson::Value::ConstMemberIterator itr = jvalue.MemberBegin(); itr != jvalue.MemberEnd(); ++itr)
        {
            auto jsonKey = itr->name.GetString();
            auto el = parseValueFromJsonValue(itr->value);
            dict[jsonKey] = el;
            //dict.insert(std::make_pair(jsonKey, el));
        }
        return Value(dict);
    }
    if(t == rapidjson::Type::kArrayType) {
        ValueVector arr;
        // rapidjson uses SizeType instead of size_t.
        for (rapidjson::SizeType i = 0; i < jvalue.Size(); i++) {
            auto el = parseValueFromJsonValue(jvalue[i]);
            arr.push_back(el);
        }
        return Value(arr);
    }

    // none
    return Value();
}

ValueMap ValueMapUtil::getValueMapFromJsonFile(const std::string& filename)
{
    auto path = std::string("HomePath_Temp") + filename;
    auto content = std::string("Temp...");//FileUtils::getInstance()->getStringFromFile(path);

    rapidjson::Document doc;
    doc.Parse<0>(content.c_str());
    if (! doc.HasParseError())
    {
        // check that root is object not array
        auto val = parseValueFromJsonValue(doc);
        if(val.getType() == Value::Type::MAP) {
            return val.asValueMap();
        }
        COBLOG("JSON wasn't a ValueMap/Dict");
    }

    COBLOG("JSON Parse Error: %d\n", doc.GetParseError());

    ValueMap ret;
    return ret;
}

ValueMap ValueMapUtil::getValueMapFromJsonString(const std::string& content)
{
    rapidjson::Document doc;
    doc.Parse<0>(content.c_str());
    if (! doc.HasParseError())
    {
        // check that root is object not array
        auto val = parseValueFromJsonValue(doc);
        if(val.getType() == Value::Type::MAP) {
            return val.asValueMap();
        }
        COBLOG("JSON wasn't a ValueMap/Dict");
    }

    COBLOG("JSON Parse Error: %d\n", doc.GetParseError());

    ValueMap ret;
    return ret;
}

ValueVector ValueMapUtil::getValueVectorFromJsonFile(const std::string& filename)
{
    //auto content = FileUtils::getInstance()->getStringFromFile(filename);
	auto content = std::string("Temp...");

    rapidjson::Document doc;
    doc.Parse<0>(content.c_str());
    if (! doc.HasParseError())
    {
        // check that root is object not array
        auto val = parseValueFromJsonValue(doc);
        if(val.getType() == Value::Type::VECTOR) {
            return val.asValueVector();
        }
        COBLOG("JSON wasn't a ValueVector/Array");
    }

    COBLOG("JSON Parse Error: %d\n", doc.GetParseError());

    ValueVector ret;
    return ret;
}

ValueVector ValueMapUtil::getValueVectorFromJsonString(const std::string& content)
{
    rapidjson::Document doc;
    doc.Parse<0>(content.c_str());
    if (! doc.HasParseError())
    {
        // check that root is object not array
        auto val = parseValueFromJsonValue(doc);
        if(val.getType() == Value::Type::VECTOR) {
            return val.asValueVector();
        }
        COBLOG("JSON wasn't a ValueVector/Array");
    }

    COBLOG("JSON Parse Error: %d\n", doc.GetParseError());

    return ValueVectorNull;
}

static bool isNumeric(std::string str)
{
    std::stringstream sin(str);
    double d;
    char c;
    if(!(sin >> d))
        return false;
    if (sin >> c)
        return false;
    return true;
}

ValueVector ValueMapUtil::convertValueVectorFromJsonString(const std::string& content)
{
    ValueVector ret;
    rapidjson::Document doc;
    doc.Parse<0>(content.c_str());
    if (! doc.HasParseError())
    {
        // check that root is object not array
        auto val = parseValueFromJsonValue(doc);
        if (val.getType() == Value::Type::VECTOR) {
            return val.asValueVector();
        }
        else if (val.getType() == Value::Type::MAP) {
            auto &vm = val.asValueMap();
            ret.resize(vm.size());
            for (auto iter = vm.begin(); iter != vm.end(); iter++)
            {
                auto key = iter->first;
                if (!isNumeric(key))
                    return ValueVectorNull;

                auto index = atoi(key.c_str());
                if (index >= (int)ret.size())
                    ret.resize(index + 1);

                ret[index] = iter->second;
            }
        }
    }

    return ret;
}

static bool parserJsonValueFromValue(const Value& value, rapidjson::Value& jvalue, rapidjson::Document::AllocatorType& allocator)
{
    bool ret = true;
    switch (value.getType())
    {
        case Value::Type::BYTE:
        {
            jvalue.SetUint(value.asByte());
        }
            break;

        case Value::Type::INTEGER:
        {
            jvalue.SetInt(value.asInt());
        }
            break;

        case Value::Type::UNSIGNED:
        {
            jvalue.SetUint(value.asUnsignedInt());
        }
            break;

        case Value::Type::LONG:
        {
            jvalue.SetInt(value.asLong());
        }
            break;

        case Value::Type::ULONG:
        {
            jvalue.SetUint(value.asUlong());
        }
            break;

        case Value::Type::FLOAT:
        {
            jvalue.SetDouble(value.asFloat());
        }
            break;

        case Value::Type::DOUBLE:
        {
            jvalue.SetDouble(value.asDouble());
        }
            break;

        case Value::Type::BOOLEAN:
        {
            jvalue.SetBool(value.asBool());
        }
            break;

        case Value::Type::STRING:
        {
            jvalue.SetString(value.asString().c_str(), allocator);
        }
            break;

        case Value::Type::VECTOR:
        {
            jvalue.SetArray();
            const ValueVector &valueVector = value.asValueVector();
            for (auto iter = valueVector.begin(); iter != valueVector.end(); iter++)
            {
                rapidjson::Value jchildValue;
                if (parserJsonValueFromValue(*iter, jchildValue, allocator))
                    jvalue.PushBack(jchildValue, allocator);
            }
        }
            break;

        case Value::Type::MAP:
        {
            jvalue.SetObject();
            const ValueMap &valueMap = value.asValueMap();
            for (auto iter = valueMap.begin(); iter != valueMap.end(); iter++)
            {
                const std::string &key = (*iter).first;
                rapidjson::Value jchildValue;
                if (parserJsonValueFromValue((*iter).second, jchildValue, allocator))
                {
                    jvalue.AddMember(StringRef(key.c_str()), jchildValue, allocator);
                    jchildValue.SetString("abc");
                }
            }
        }
            break;

        default:
            ret = false;
            break;
    }
    COBASSERT(ret, "parserJsonValueFromValue: type is unknown");
    return ret;
}

std::string ValueMapUtil::makeJsonFromValueMap(const ValueMap& valueMap)
{
    rapidjson::Document document;
    document.SetObject();
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

    for (auto iter = valueMap.begin(); iter != valueMap.end(); iter++)
    {
        const std::string &key = (*iter).first;
        rapidjson::Value jvalue;
        if (parserJsonValueFromValue((*iter).second, jvalue, allocator))
            document.AddMember(StringRef(key.c_str()), jvalue, allocator);
    }

    StringBuffer buffer;
    rapidjson::Writer<StringBuffer> writer(buffer);
    document.Accept(writer);

    return buffer.GetString();
}

std::string ValueMapUtil::makeJsonFromValueVector(const ValueVector& valueVector)
{
    rapidjson::Document document;
    document.SetArray();
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

    for (auto iter = valueVector.begin(); iter != valueVector.end(); iter++)
    {
        rapidjson::Value jvalue;
        if (parserJsonValueFromValue(*iter, jvalue, allocator))
            document.PushBack(jvalue, allocator);
    }

    StringBuffer buffer;
    rapidjson::Writer<StringBuffer> writer(buffer);
    document.Accept(writer);

    return buffer.GetString();
}

Value& ValueMapUtil::at(ValueMap& valueMap, const std::string &path)
{
    size_t pos1 = 0;
    size_t pos2 = 0;
    Value* temp = nullptr;
    while ((pos2 = path.find_first_of('/', pos1)) != std::string::npos)
    {
        std::string section = path.substr(pos1, pos2 - pos1);
        if (!section.empty())
        {
            if (temp)
                temp = &temp->asValueMap()[section];
            else
                temp = &valueMap[section];

            if (temp->getType() != Value::Type::MAP)
            {
                *temp = ValueMapNull;
            }
        }
        pos1 = pos2 + 1;
    }

    std::string section = path.substr(pos1, path.size() - pos1);
    if (!section.empty())
    {
        if (temp)
            temp = &(*temp).asValueMap()[section];
        else
            temp = &valueMap[section];
    }
    return *temp;
}


const Value& ValueMapUtil::get(const ValueMap& valueMap, const std::string &path)
{
    const ValueMap* tempMap = &valueMap;
    const Value *value = (Value*)&Value::Null;

    size_t pos1 = 0;
    size_t pos2 = 0;
    while ((pos2 = path.find_first_of('/', pos1)) != std::string::npos)
    {
        std::string section = path.substr(pos1, pos2 - pos1);
        if (!section.empty())
        {
            auto iter = tempMap->end();
            if ((iter = tempMap->find(section)) == tempMap->end())
                return (Value&)Value::Null;

            value = &iter->second;
            if (iter->second.getType() == Value::Type::MAP)
                tempMap = &value->asValueMap();
            else
                tempMap = (ValueMap*)&ValueMapNull;
        }
        pos1 = pos2 + 1;
    }

    std::string section = path.substr(pos1, path.size() - pos1);
    if (!section.empty())
    {
        auto iter = tempMap->end();
        if ((iter = tempMap->find(section)) == tempMap->end())
            return (Value&)Value::Null;

        value = &iter->second;
    }

    return *value;
}

bool ValueMapUtil::set(ValueMap& valueMap, const std::string &path, const Value& value)
{
    size_t pos1 = 0;
    size_t pos2 = 0;
    Value* temp = nullptr;
    while ((pos2 = path.find_first_of('/', pos1)) != std::string::npos)
    {
        std::string section = path.substr(pos1, pos2 - pos1);
        if (!section.empty())
        {
            if (temp)
                temp = &temp->asValueMap()[section];
            else
                temp = &valueMap[section];

            if (temp->getType() != Value::Type::MAP)
            {
                *temp = ValueMapNull;
            }
        }
        pos1 = pos2 + 1;
    }

    std::string section = path.substr(pos1, path.size() - pos1);
    if (!section.empty())
    {
        if (temp)
            (*temp).asValueMap()[section] = value;
        else
            valueMap[section] = value;
    }
    return true;
}

bool ValueMapUtil::replaceEmbeddedValue(ValueMap& valueMap)
{
    auto iter = valueMap.begin();
    while (iter != valueMap.end())
    {
        std::string key = (*iter).first;
        Value &value = (*iter).second;
        if (key.at(0) == '@')
        {
            std::string& realKey = key.erase(0, 1);
            COBASSERT(value.getType() == Value::Type::STRING, "@value should be string");

            std::string path = value.asString();
            size_t pos1 = 0;
            size_t pos2 = 0;
            if ((pos2 = path.find(".plist", pos1)) != std::string::npos)
            {
                std::string plist = path.substr(pos1, pos2 - pos1 + 6);
                std::string ppath = path.substr(pos2 + 7, path.size() - pos2 - 7);
                if (!plist.empty())
                {
                    std::string plistFile = "";//FileUtils::getInstance()->fullPathForFilename(plist); //gwas
                    ValueMap dict;//FileUtils::getInstance()->getValueMapFromFile(plistFile.c_str());
                    COBASSERT(!dict.empty(), "HeroDefs: file is empty");

                    auto& value2 = at(dict, ppath);
                    COBASSERT(!value2.isNull(), "value is null");

                    valueMap.erase(iter++);
                    valueMap[realKey] = value2;
                    continue;
                }
            }
        }
        else
        {
            Value::Type type = value.getType();
            if (type == Value::Type::MAP)
            {
                replaceEmbeddedValue(value.asValueMap());
            }
            else if (type == Value::Type::VECTOR)
            {
                replaceEmbeddedValue(value.asValueVector());
            }
        }
        iter++;
    }

    return true;
}

bool ValueMapUtil::replaceEmbeddedValue(ValueVector& valueVector)
{
    auto iter = valueVector.begin();
    while (iter != valueVector.end())
    {
        Value &value = *iter;
        Value::Type type = value.getType();
        if (type == Value::Type::MAP)
        {
            replaceEmbeddedValue(value.asValueMap());
        }
        else if (type == Value::Type::VECTOR)
        {
            replaceEmbeddedValue(value.asValueVector());
        }
        iter++;
    }

    return true;
}

const Value& ValueMap::get(const std::string &path) const
{
    return ValueMapUtil::get(*this, path);
}

Value& ValueMap::at(const std::string &path)
{
    return ValueMapUtil::at(*this, path);
}

bool ValueMap::get(const std::string &path, ValueVector& vv) const
{
    auto &val = get(path);
    if (val.isNull())
        return false;

    vv.clear();
    auto str = val.asString();
    char* head = (char*)malloc(str.size() + 1);
    strcpy(head, str.c_str());
    char* p = strtok(head, ",");
    while (p)
    {
        vv.push_back(Value(p));
        p = strtok(NULL, ",");
    }
    free(head);
    return true;
}

bool ValueMap::set(const std::string &path, const Value& value)
{
    return ValueMapUtil::set(*this, path, value);
}

bool ValueMap::set(const std::string &path, const std::string& value)
{
    return ValueMapUtil::set(*this, path, Value(value));
}

bool ValueMap::isExist(const std::string& key)
{
    return this->find(key) != this->end();
}

void ValueMap::dump()
{
    for (auto iter = begin(); iter != end(); iter++)
    {
        auto &key = iter->first;
        auto &value = iter->second;
        if (value.getType() == Value::Type::MAP)
        {
            COBLOG("HVM::dump: key=%s,type=%d", key.c_str(), (int)value.getType());
            auto& vm = (ValueMap&)value.asValueMap();
            vm.dump();
        }
        else if (value.getType() != Value::Type::VECTOR && value.getType() != Value::Type::INT_KEY_MAP)
        {
            COBLOG("HVM::dump: key=%s,type=%d,val=%s", key.c_str(), (int)value.getType(), value.asString().c_str());
        }
    }
}

ValueMap& ValueMap::operator+= (const ValueMap& other)
{
    for (auto it2 = other.begin(); it2 != other.end(); it2++)
    {
        auto &key = it2->first;
        auto it1 = this->find(key);
        if (it1 == this->end())
        {
            (*this)[key] = it2->second;
        }
        else
        {
            Value& val1 = it1->second;
            Value& val2 = (Value&)it2->second;
            if (val1.getType() == Value::Type::MAP &&
                val2.getType() == Value::Type::MAP)
            {
                auto &map1 = (ValueMap&)val1.asValueMap();
                map1 += val2.asValueMap();
            }
        }
    }
    return *this;
}

bool ValueMap::fromJson(const std::string& jsonContent)
{
    rapidjson::Document doc;
    doc.Parse<0>(jsonContent.c_str());
    if (! doc.HasParseError())
    {
        // check that root is object not array
        auto val = ValueMapUtil::parseValueFromJsonValue(doc);
        if(val.getType() == Value::Type::MAP) {
            *this = val.asValueMap();
            return true;
        }
        COBLOG("JSON wasn't a ValueMap/Dict");
    }

    COBLOG("JSON Parse Error: %d\n", doc.GetParseError());

    return false;
}

std::string ValueMap::toJson() const
{
	return ValueMapUtil::makeJsonFromValueMap(*this);
}

void ValueVector::split(const std::string& str, char separator)
 {
     size_t last = 0;
     size_t index = str.find_first_of(separator, last);
     size_t size = str.size();
     while (index != std::string::npos)
     {
         if (index - last > 0)
         {
             Value val(str.substr(last, index - last));
             push_back(val);
         }

         if (index >= size - 1)
         {
             last = index + 1;
             break;
         }

         last = index + 1;
         index = str.find_first_not_of(separator, last);
         if (index != std::string::npos)
         {
             last = index;
             index = str.find_first_of(separator, last);
         }
     }

     index = str.size();
     if (index - last > 0)
     {
         Value val(str.substr(last, index - last));
         push_back(val);
     }
 }

NS_COB_END
