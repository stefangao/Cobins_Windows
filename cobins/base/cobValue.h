/****************************************************************************
 Copyright (c) 2013-2017 Chukong Technologies
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-2020 Stefan Gao (stefan.gao@gmail.com)

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

#ifndef __COB_VALUE_H__
#define __COB_VALUE_H__

#include <string>
#include <vector>
#include <unordered_map>
#include <limits.h>
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
#include "cobMacros.h"
#include "cobUtils.h"

NS_COB_BEGIN

class Value;

typedef std::vector<Value> __ValueVector;
typedef std::unordered_map<std::string, Value> __ValueMap;
typedef std::unordered_map<int, Value> ValueMapIntKey;


class ValueMap;
class ValueVector;

class ValueMapUtil
{
public:
    static Value parseValueFromJsonValue(const rapidjson::Value& jvalue);
    static ValueMap getValueMapFromJsonFile(const std::string& filename);
    static ValueMap getValueMapFromJsonString(const std::string& content);
    static ValueVector getValueVectorFromJsonFile(const std::string& filename);
    static ValueVector getValueVectorFromJsonString(const std::string& content);
    static std::string makeJsonFromValueMap(const ValueMap& valueMap);
    static std::string makeJsonFromValueVector(const ValueVector& valueVector);
    static ValueVector convertValueVectorFromJsonString(const std::string& content);

public:
    static Value& at(ValueMap& valueMap, const std::string &path);
    static const Value& get(const ValueMap& valueMap, const std::string &path);
    static bool set(ValueMap& valueMap, const std::string &path, const Value& value);
    static bool replaceEmbeddedValue(ValueMap& valueMap);
    static bool replaceEmbeddedValue(ValueVector& valueVector);
};


/*
 * This class is provide as a wrapper of basic types, such as int and bool.
 */
class Value
{
public:
    /** A predefined Value that has not value. */
    static const Value Null;

    /** Default constructor. */
    Value();

    /** Create a Value by an unsigned char value. */
    explicit Value(unsigned char v);

    /** Create a Value by an integer value. */
    explicit Value(int v);

    /** Create a Value by an unsigned value. */
    explicit Value(unsigned int v);

    explicit Value(long v);
    explicit Value(unsigned long v);

    /** Create a Value by a float value. */
    explicit Value(float v);

    /** Create a Value by a double value. */
    explicit Value(double v);

    /** Create a Value by a bool value. */
    explicit Value(bool v);

    /** Create a Value by a char pointer. It will copy the chars internally. */
    explicit Value(const char* v);

    /** Create a Value by a string. */
    explicit Value(const std::string& v);

    /** Create a Value by a ValueVector object. */
    explicit Value(const ValueVector& v);
    /** Create a Value by a ValueVector object. It will use std::move internally. */
    explicit Value(ValueVector&& v);

    /** Create a Value by a ValueMap object. */
    explicit Value(const ValueMap& v);
    /** Create a Value by a ValueMap object. It will use std::move internally. */
    explicit Value(ValueMap&& v);

    /** Create a Value by a ValueMapIntKey object. */
    explicit Value(const ValueMapIntKey& v);
    /** Create a Value by a ValueMapIntKey object. It will use std::move internally. */
    explicit Value(ValueMapIntKey&& v);

    /** Create a Value by another Value object. */
    Value(const Value& other);
    /** Create a Value by a Value object. It will use std::move internally. */
    Value(Value&& other);

    /** Destructor. */
    ~Value();

    /** Assignment operator, assign from Value to Value. */
    Value& operator= (const Value& other);
    /** Assignment operator, assign from Value to Value. It will use std::move internally. */
    Value& operator= (Value&& other);

    /** Assignment operator, assign from unsigned char to Value. */
    Value& operator= (unsigned char v);
    /** Assignment operator, assign from integer to Value. */
    Value& operator= (int v);
    /** Assignment operator, assign from integer to Value. */
    Value& operator= (unsigned int v);

    Value& operator= (long v);
    Value& operator= (unsigned long v);

    /** Assignment operator, assign from float to Value. */
    Value& operator= (float v);
    /** Assignment operator, assign from double to Value. */
    Value& operator= (double v);
    /** Assignment operator, assign from bool to Value. */
    Value& operator= (bool v);
    /** Assignment operator, assign from char* to Value. */
    Value& operator= (const char* v);
    /** Assignment operator, assign from string to Value. */
    Value& operator= (const std::string& v);

    /** Assignment operator, assign from ValueVector to Value. */
    Value& operator= (const ValueVector& v);
    /** Assignment operator, assign from ValueVector to Value. */
    Value& operator= (ValueVector&& v);

    /** Assignment operator, assign from ValueMap to Value. */
    Value& operator= (const ValueMap& v);
    /** Assignment operator, assign from ValueMap to Value. It will use std::move internally. */
    Value& operator= (ValueMap&& v);

    /** Assignment operator, assign from ValueMapIntKey to Value. */
    Value& operator= (const ValueMapIntKey& v);
    /** Assignment operator, assign from ValueMapIntKey to Value. It will use std::move internally. */
    Value& operator= (ValueMapIntKey&& v);

    /** != operator overloading */
    bool operator!= (const Value& v);
    /** != operator overloading */
    bool operator!= (const Value& v) const;
    /** == operator overloading */
    bool operator== (const Value& v);
    /** == operator overloading */
    bool operator== (const Value& v) const;

    /** Gets as a byte value. Will convert to unsigned char if possible, or will trigger assert error. */
    unsigned char asByte() const;
    /** Gets as an integer value. Will convert to integer if possible, or will trigger assert error. */
    int asInt() const;
    /** Gets as an unsigned value. Will convert to unsigned if possible, or will trigger assert error. */
    unsigned int asUnsignedInt() const;

    long asLong() const;
    unsigned long asUlong() const;

    /** Gets as a float value. Will convert to float if possible, or will trigger assert error. */
    float asFloat() const;
    /** Gets as a double value. Will convert to double if possible, or will trigger assert error. */
    double asDouble() const;
    /** Gets as a bool value. Will convert to bool if possible, or will trigger assert error. */
    bool asBool() const;
    /** Gets as a string value. Will convert to string if possible, or will trigger assert error. */
    std::string asString() const;

    /** Gets as a ValueVector reference. Will convert to ValueVector if possible, or will trigger assert error. */
    ValueVector& asValueVector();
    /** Gets as a const ValueVector reference. Will convert to ValueVector if possible, or will trigger assert error. */
    const ValueVector& asValueVector() const;

    /** Gets as a ValueMap reference. Will convert to ValueMap if possible, or will trigger assert error. */
    ValueMap& asValueMap();
    /** Gets as a const ValueMap reference. Will convert to ValueMap if possible, or will trigger assert error. */
    const ValueMap& asValueMap() const;

    /** Gets as a ValueMapIntKey reference. Will convert to ValueMapIntKey if possible, or will trigger assert error. */
    ValueMapIntKey& asIntKeyMap();
    /** Gets as a const ValueMapIntKey reference. Will convert to ValueMapIntKey if possible, or will trigger assert error. */
    const ValueMapIntKey& asIntKeyMap() const;

    /**
     * Checks if the Value is null.
     * @return True if the Value is null, false if not.
     */
    bool isNull() const { return _type == Type::NONE; }

    /** Value type wrapped by Value. */
    enum class Type
    {
        /// no value is wrapped, an empty Value
        NONE = 0,
        /// wrap byte
        BYTE,
        /// wrap integer
        INTEGER,
        /// wrap unsigned
        UNSIGNED,
        /// wrap long
        LONG,
        /// wrap unsigned long
        ULONG,
        /// wrap float
        FLOAT,
        /// wrap double
        DOUBLE,
        /// wrap bool
        BOOLEAN,
        /// wrap string
        STRING,
        /// wrap vector
        VECTOR,
        /// wrap ValueMap
        MAP,
        /// wrap ValueMapIntKey
        INT_KEY_MAP
    };

    /** Gets the value type. */
    Type getType() const { return _type; }

    /** Gets the description of the class. */
    std::string getDescription() const;

private:
    void clear();
    void reset(Type type);

    union
    {
        unsigned char byteVal;
        int intVal;
        unsigned int unsignedVal;
        long longVal;
        long ulongVal;
        float floatVal;
        double doubleVal;
        bool boolVal;

        std::string* strVal;
        ValueVector* vectorVal;
        ValueMap* mapVal;
        ValueMapIntKey* intKeyMapVal;
    }_field;

    Type _type;
};

class ValueMap : public __ValueMap
{
public:
    ValueMap() {};
    explicit ValueMap(const __ValueMap& other)
    {
        *this = (ValueMap&)other;
    }

    const Value& get(const std::string &path) const;
    Value& at(const std::string &path);

    bool fromJson(const std::string& jsonContent);
    std::string toJson() const;

    bool get(const std::string &path, ValueVector& vv) const;
    bool set(const std::string &path, const Value& value);
    bool set(const std::string &path, const std::string& value);

    ValueMap& operator+= (const ValueMap& other);

    bool isExist(const std::string& key);
    void dump();
};

class ValueVector : public __ValueVector
{
public:
    void split(const std::string& str, char separator);
};

extern const ValueVector ValueVectorNull;
extern const ValueMap ValueMapNull;
extern const ValueMapIntKey ValueMapIntKeyNull;

NS_COB_END

#endif

