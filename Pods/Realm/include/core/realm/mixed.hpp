/*************************************************************************
 *
 * Copyright 2016 Realm Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 **************************************************************************/

#ifndef REALM_MIXED_HPP
#define REALM_MIXED_HPP

#include <cstdint> // int64_t - not part of C++03, not even required by C++11 (see C++11 section 18.4.1)

#include <cstddef> // size_t
#include <cstring>

<<<<<<< HEAD
#include <realm/binary_data.hpp>
#include <realm/data_type.hpp>
#include <realm/olddatetime.hpp>
=======
#include <realm/keys.hpp>
#include <realm/binary_data.hpp>
#include <realm/data_type.hpp>
>>>>>>> origin/develop12
#include <realm/string_data.hpp>
#include <realm/timestamp.hpp>
#include <realm/util/assert.hpp>
#include <realm/utilities.hpp>

namespace realm {


/// This class represents a polymorphic Realm value.
///
/// At any particular moment an instance of this class stores a
/// definite value of a definite type. If, for instance, that is an
<<<<<<< HEAD
/// integer value, you may call get_int() to extract that value. You
/// may call get_type() to discover what type of value is currently
/// stored. Calling get_int() on an instance that does not store an
=======
/// integer value, you may call get<int64_t>() to extract that value. You
/// may call get_type() to discover what type of value is currently
/// stored. Calling get<int64_t>() on an instance that does not store an
>>>>>>> origin/develop12
/// integer, has undefined behavior, and likewise for all the other
/// types that can be stored.
///
/// It is crucial to understand that the act of extracting a value of
/// a particular type requires definite knowledge about the stored
/// type. Calling a getter method for any particular type, that is not
/// the same type as the stored value, has undefined behavior.
///
/// While values of numeric types are contained directly in a Mixed
/// instance, character and binary data are merely referenced. A Mixed
/// instance never owns the referenced data, nor does it in any other
/// way attempt to manage its lifetime.
///
/// For compatibility with C style strings, when a string (character
/// data) is stored in a Realm database, it is always followed by a
/// terminating null character. This is also true when strings are
/// stored in a mixed type column. This means that in the following
/// code, if the 'mixed' value of the 8th row stores a string, then \c
/// c_str will always point to a null-terminated string:
///
/// \code{.cpp}
///
///   const char* c_str = my_table[7].mixed.data(); // Always null-terminated
///
/// \endcode
///
/// Note that this assumption does not hold in general for strings in
/// instances of Mixed. Indeed there is nothing stopping you from
/// constructing a new Mixed instance that refers to a string without
/// a terminating null character.
///
/// At the present time no soultion has been found that would allow
/// for a Mixed instance to directly store a reference to a table. The
/// problem is roughly as follows: From most points of view, the
/// desirable thing to do, would be to store the table reference in a
/// Mixed instance as a plain pointer without any ownership
/// semantics. This would have no negative impact on the performance
/// of copying and destroying Mixed instances, and it would serve just
/// fine for passing a table as argument when setting the value of an
/// entry in a mixed column. In that case a copy of the referenced
/// table would be inserted into the mixed column.
///
/// On the other hand, when retrieving a table reference from a mixed
/// column, storing it as a plain pointer in a Mixed instance is no
/// longer an acceptable option. The complex rules for managing the
/// lifetime of a Table instance, that represents a subtable,
/// necessitates the use of a "smart pointer" such as
/// TableRef. Enhancing the Mixed class to be able to act as a
/// TableRef would be possible, but would also lead to several new
/// problems. One problem is the risk of a Mixed instance outliving a
/// stack allocated Table instance that it references. This would be a
/// fatal error. Another problem is the impact that the nontrivial
/// table reference has on the performance of copying and destroying
/// Mixed instances.
///
/// \sa StringData
class Mixed {
public:
<<<<<<< HEAD
    Mixed() noexcept;

    Mixed(bool) noexcept;
    Mixed(int64_t) noexcept;
    Mixed(float) noexcept;
    Mixed(double) noexcept;
    Mixed(StringData) noexcept;
    Mixed(BinaryData) noexcept;
    Mixed(OldDateTime) noexcept;
    Mixed(Timestamp) noexcept;
=======
    Mixed() noexcept
        : m_type(0)
    {
    }

    Mixed(util::None) noexcept
        : Mixed()
    {
    }

    Mixed(int i) noexcept
        : Mixed(int64_t(i))
    {
    }
    Mixed(int64_t) noexcept;
    Mixed(bool) noexcept;
    Mixed(float) noexcept;
    Mixed(double) noexcept;
    Mixed(util::Optional<int64_t>) noexcept;
    Mixed(util::Optional<bool>) noexcept;
    Mixed(util::Optional<float>) noexcept;
    Mixed(util::Optional<double>) noexcept;
    Mixed(StringData) noexcept;
    Mixed(BinaryData) noexcept;
    Mixed(Timestamp) noexcept;
    Mixed(ObjKey) noexcept;
>>>>>>> origin/develop12

    // These are shortcuts for Mixed(StringData(c_str)), and are
    // needed to avoid unwanted implicit conversion of char* to bool.
    Mixed(char* c_str) noexcept
<<<<<<< HEAD
    {
        set_string(c_str);
    }
    Mixed(const char* c_str) noexcept
    {
        set_string(c_str);
    }

    struct subtable_tag {
    };
    Mixed(subtable_tag) noexcept
        : m_type(type_Table)
=======
        : Mixed(StringData(c_str))
    {
    }
    Mixed(const char* c_str) noexcept
        : Mixed(StringData(c_str))
    {
    }
    Mixed(const std::string& s) noexcept
        : Mixed(StringData(s))
>>>>>>> origin/develop12
    {
    }

    ~Mixed() noexcept
    {
    }

    DataType get_type() const noexcept
    {
<<<<<<< HEAD
        return m_type;
=======
        REALM_ASSERT(m_type);
        return DataType(m_type - 1);
>>>>>>> origin/develop12
    }

    template <class T>
    T get() const noexcept;

<<<<<<< HEAD
    int64_t get_int() const noexcept;
    bool get_bool() const noexcept;
    float get_float() const noexcept;
    double get_double() const noexcept;
    StringData get_string() const noexcept;
    BinaryData get_binary() const noexcept;
    OldDateTime get_olddatetime() const noexcept;
    Timestamp get_timestamp() const noexcept;

    void set_int(int64_t) noexcept;
    void set_bool(bool) noexcept;
    void set_float(float) noexcept;
    void set_double(double) noexcept;
    void set_string(StringData) noexcept;
    void set_binary(BinaryData) noexcept;
    void set_binary(const char* data, size_t size) noexcept;
    void set_olddatetime(OldDateTime) noexcept;
    void set_timestamp(Timestamp) noexcept;

    template <class Ch, class Tr>
    friend std::basic_ostream<Ch, Tr>& operator<<(std::basic_ostream<Ch, Tr>&, const Mixed&);

private:
    DataType m_type;
    union {
        int64_t m_int;
        bool m_bool;
        float m_float;
        double m_double;
        const char* m_data;
        int_fast64_t m_date;
        Timestamp m_timestamp;
    };
    size_t m_size = 0;
};

// Note: We cannot compare two mixed values, since when the type of
// both is type_Table, we would have to compare the two tables, but
// the mixed values do not provide access to those tables.

// Note: The mixed values are specified as Wrap<Mixed>. If they were
// not, these operators would apply to simple comparisons, such as int
// vs int64_t, and cause ambiguity. This is because the constructors
// of Mixed are not explicit.

// Compare mixed with integer
template <class T>
bool operator==(Wrap<Mixed>, const T&) noexcept;
template <class T>
bool operator!=(Wrap<Mixed>, const T&) noexcept;
template <class T>
bool operator==(const T&, Wrap<Mixed>) noexcept;
template <class T>
bool operator!=(const T&, Wrap<Mixed>) noexcept;

// Compare mixed with boolean
bool operator==(Wrap<Mixed>, bool) noexcept;
bool operator!=(Wrap<Mixed>, bool) noexcept;
bool operator==(bool, Wrap<Mixed>) noexcept;
bool operator!=(bool, Wrap<Mixed>) noexcept;

// Compare mixed with float
bool operator==(Wrap<Mixed>, float);
bool operator!=(Wrap<Mixed>, float);
bool operator==(float, Wrap<Mixed>);
bool operator!=(float, Wrap<Mixed>);

// Compare mixed with double
bool operator==(Wrap<Mixed>, double);
bool operator!=(Wrap<Mixed>, double);
bool operator==(double, Wrap<Mixed>);
bool operator!=(double, Wrap<Mixed>);

// Compare mixed with string
bool operator==(Wrap<Mixed>, StringData) noexcept;
bool operator!=(Wrap<Mixed>, StringData) noexcept;
bool operator==(StringData, Wrap<Mixed>) noexcept;
bool operator!=(StringData, Wrap<Mixed>) noexcept;
bool operator==(Wrap<Mixed>, const char* c_str) noexcept;
bool operator!=(Wrap<Mixed>, const char* c_str) noexcept;
bool operator==(const char* c_str, Wrap<Mixed>) noexcept;
bool operator!=(const char* c_str, Wrap<Mixed>) noexcept;
bool operator==(Wrap<Mixed>, char* c_str) noexcept;
bool operator!=(Wrap<Mixed>, char* c_str) noexcept;
bool operator==(char* c_str, Wrap<Mixed>) noexcept;
bool operator!=(char* c_str, Wrap<Mixed>) noexcept;

// Compare mixed with binary data
bool operator==(Wrap<Mixed>, BinaryData) noexcept;
bool operator!=(Wrap<Mixed>, BinaryData) noexcept;
bool operator==(BinaryData, Wrap<Mixed>) noexcept;
bool operator!=(BinaryData, Wrap<Mixed>) noexcept;

// Compare mixed with date
bool operator==(Wrap<Mixed>, OldDateTime) noexcept;
bool operator!=(Wrap<Mixed>, OldDateTime) noexcept;
bool operator==(OldDateTime, Wrap<Mixed>) noexcept;
bool operator!=(OldDateTime, Wrap<Mixed>) noexcept;

// Compare mixed with Timestamp
bool operator==(Wrap<Mixed>, Timestamp) noexcept;
bool operator!=(Wrap<Mixed>, Timestamp) noexcept;
bool operator==(Timestamp, Wrap<Mixed>) noexcept;
bool operator!=(Timestamp, Wrap<Mixed>) noexcept;

// Implementation:

inline Mixed::Mixed() noexcept
{
    m_type = type_Int;
    m_int = 0;
}

inline Mixed::Mixed(int64_t v) noexcept
{
    m_type = type_Int;
    m_int = v;
=======
    // These functions are kept to be backwards compatible
    int64_t get_int() const;
    bool get_bool() const;
    float get_float() const;
    double get_double() const;
    StringData get_string() const;
    BinaryData get_binary() const;
    Timestamp get_timestamp() const;

    bool is_null() const;
    int compare(const Mixed& b) const;
    bool operator==(const Mixed& other) const
    {
        return compare(other) == 0;
    }
    bool operator!=(const Mixed& other) const
    {
        return compare(other) != 0;
    }

private:
    friend std::ostream& operator<<(std::ostream& out, const Mixed& m);

    uint32_t m_type;
    union {
        int32_t short_val;
        uint32_t ushort_val;
    };

    union {
        int64_t int_val;
        bool bool_val;
        float float_val;
        double double_val;
        const char* str_val;
    };
};

// Implementation:

inline Mixed::Mixed(int64_t v) noexcept
{
    m_type = type_Int + 1;
    int_val = v;
>>>>>>> origin/develop12
}

inline Mixed::Mixed(bool v) noexcept
{
<<<<<<< HEAD
    m_type = type_Bool;
    m_bool = v;
=======
    m_type = type_Bool + 1;
    bool_val = v;
>>>>>>> origin/develop12
}

inline Mixed::Mixed(float v) noexcept
{
<<<<<<< HEAD
    m_type = type_Float;
    m_float = v;
=======
    m_type = type_Float + 1;
    float_val = v;
>>>>>>> origin/develop12
}

inline Mixed::Mixed(double v) noexcept
{
<<<<<<< HEAD
    m_type = type_Double;
    m_double = v;
}

inline Mixed::Mixed(StringData v) noexcept
{
    m_type = type_String;
    m_data = v.data();
    m_size = v.size();
}

inline Mixed::Mixed(BinaryData v) noexcept
{
    m_type = type_Binary;
    m_data = v.data();
    m_size = v.size();
}

inline Mixed::Mixed(OldDateTime v) noexcept
{
    m_type = type_OldDateTime;
    m_date = v.get_olddatetime();
}

inline Mixed::Mixed(Timestamp v) noexcept
{
    m_type = type_Timestamp;
    m_timestamp = v;
}

inline int64_t Mixed::get_int() const noexcept
{
    REALM_ASSERT(m_type == type_Int);
    return m_int;
}

inline bool Mixed::get_bool() const noexcept
{
    REALM_ASSERT(m_type == type_Bool);
    return m_bool;
}

inline float Mixed::get_float() const noexcept
{
    REALM_ASSERT(m_type == type_Float);
    return m_float;
}

inline double Mixed::get_double() const noexcept
{
    REALM_ASSERT(m_type == type_Double);
    return m_double;
}

inline StringData Mixed::get_string() const noexcept
{
    REALM_ASSERT(m_type == type_String);
    return StringData(m_data, m_size);
}

inline BinaryData Mixed::get_binary() const noexcept
{
    REALM_ASSERT(m_type == type_Binary);
    return BinaryData(m_data, m_size);
}

inline OldDateTime Mixed::get_olddatetime() const noexcept
{
    REALM_ASSERT(m_type == type_OldDateTime);
    return m_date;
}

inline Timestamp Mixed::get_timestamp() const noexcept
{
    REALM_ASSERT(m_type == type_Timestamp);
    return m_timestamp;
}

template <>
inline int64_t Mixed::get<Int>() const noexcept
{
    return get_int();
}

template <>
inline bool Mixed::get<bool>() const noexcept
{
    return get_bool();
}

template <>
inline float Mixed::get<float>() const noexcept
{
    return get_float();
=======
    m_type = type_Double + 1;
    double_val = v;
}

inline Mixed::Mixed(util::Optional<int64_t> v) noexcept
{
    if (v) {
        m_type = type_Int + 1;
        int_val = *v;
    }
    else {
        m_type = 0;
    }
}

inline Mixed::Mixed(util::Optional<bool> v) noexcept
{
    if (v) {
        m_type = type_Bool + 1;
        bool_val = *v;
    }
    else {
        m_type = 0;
    }
}

inline Mixed::Mixed(util::Optional<float> v) noexcept
{
    if (v) {
        m_type = type_Float + 1;
        float_val = *v;
    }
    else {
        m_type = 0;
    }
}

inline Mixed::Mixed(util::Optional<double> v) noexcept
{
    if (v) {
        m_type = type_Double + 1;
        double_val = *v;
    }
    else {
        m_type = 0;
    }
}

inline Mixed::Mixed(StringData v) noexcept
{
    if (!v.is_null()) {
        m_type = type_String + 1;
        str_val = v.data();
        ushort_val = uint32_t(v.size());
    }
    else {
        m_type = 0;
    }
}

inline Mixed::Mixed(BinaryData v) noexcept
{
    if (!v.is_null()) {
        m_type = type_Binary + 1;
        str_val = v.data();
        ushort_val = uint32_t(v.size());
    }
    else {
        m_type = 0;
    }
}

inline Mixed::Mixed(Timestamp v) noexcept
{
    if (!v.is_null()) {
        m_type = type_Timestamp + 1;
        int_val = v.get_seconds();
        short_val = v.get_nanoseconds();
    }
    else {
        m_type = 0;
    }
}

inline Mixed::Mixed(ObjKey v) noexcept
{
    if (v) {
        m_type = type_Link + 1;
        int_val = v.value;
    }
    else {
        m_type = 0;
    }
}

template <>
inline int64_t Mixed::get<int64_t>() const noexcept
{
    REALM_ASSERT(get_type() == type_Int);
    return int_val;
}

inline int64_t Mixed::get_int() const
{
    return get<int64_t>();
}

template <>
inline bool Mixed::get<bool>() const noexcept
{
    REALM_ASSERT(get_type() == type_Bool);
    return bool_val;
}

inline bool Mixed::get_bool() const
{
    return get<bool>();
}

template <>
inline float Mixed::get<float>() const noexcept
{
    REALM_ASSERT(get_type() == type_Float);
    return float_val;
}

inline float Mixed::get_float() const
{
    return get<float>();
>>>>>>> origin/develop12
}

template <>
inline double Mixed::get<double>() const noexcept
{
<<<<<<< HEAD
    return get_double();
}

template <>
inline OldDateTime Mixed::get<OldDateTime>() const noexcept
{
    return get_olddatetime();
=======
    REALM_ASSERT(get_type() == type_Double);
    return double_val;
}

inline double Mixed::get_double() const
{
    return get<double>();
>>>>>>> origin/develop12
}

template <>
inline StringData Mixed::get<StringData>() const noexcept
{
<<<<<<< HEAD
    return get_string();
}

template <>
inline BinaryData Mixed::get<BinaryData>() const noexcept
{
    return get_binary();
}

template <>
inline Timestamp Mixed::get<Timestamp>() const noexcept
{
    return get_timestamp();
}


inline void Mixed::set_int(int64_t v) noexcept
{
    m_type = type_Int;
    m_int = v;
}

inline void Mixed::set_bool(bool v) noexcept
{
    m_type = type_Bool;
    m_bool = v;
}

inline void Mixed::set_float(float v) noexcept
{
    m_type = type_Float;
    m_float = v;
}

inline void Mixed::set_double(double v) noexcept
{
    m_type = type_Double;
    m_double = v;
}

inline void Mixed::set_string(StringData v) noexcept
{
    m_type = type_String;
    m_data = v.data();
    m_size = v.size();
}

inline void Mixed::set_binary(BinaryData v) noexcept
{
    set_binary(v.data(), v.size());
}

inline void Mixed::set_binary(const char* data, size_t size) noexcept
{
    m_type = type_Binary;
    m_data = data;
    m_size = size;
}

inline void Mixed::set_olddatetime(OldDateTime v) noexcept
{
    m_type = type_OldDateTime;
    m_date = v.get_olddatetime();
}

inline void Mixed::set_timestamp(Timestamp v) noexcept
{
    m_type = type_Timestamp;
    m_timestamp = v;
}

// LCOV_EXCL_START
template <class Ch, class Tr>
inline std::basic_ostream<Ch, Tr>& operator<<(std::basic_ostream<Ch, Tr>& out, const Mixed& m)
{
    out << "Mixed(";
    switch (m.m_type) {
        case type_Int:
            out << m.m_int;
            break;
        case type_Bool:
            out << m.m_bool;
            break;
        case type_Float:
            out << m.m_float;
            break;
        case type_Double:
            out << m.m_double;
            break;
        case type_String:
            out << StringData(m.m_data, m.m_size);
            break;
        case type_Binary:
            out << BinaryData(m.m_data, m.m_size);
            break;
        case type_OldDateTime:
            out << OldDateTime(m.m_date);
            break;
        case type_Timestamp:
            out << Timestamp(m.m_timestamp);
            break;
        case type_Table:
            out << "subtable";
            break;
        case type_Mixed:
        case type_Link:
        case type_LinkList:
            REALM_ASSERT(false);
    }
    out << ")";
    return out;
}
// LCOV_EXCL_STOP


// Compare mixed with integer

template <class T>
inline bool operator==(Wrap<Mixed> a, const T& b) noexcept
{
    return Mixed(a).get_type() == type_Int && Mixed(a).get_int() == b;
}

template <class T>
inline bool operator!=(Wrap<Mixed> a, const T& b) noexcept
{
    return Mixed(a).get_type() != type_Int || Mixed(a).get_int() != b;
}

template <class T>
inline bool operator==(const T& a, Wrap<Mixed> b) noexcept
{
    return type_Int == Mixed(b).get_type() && a == Mixed(b).get_int();
}

template <class T>
inline bool operator!=(const T& a, Wrap<Mixed> b) noexcept
{
    return type_Int != Mixed(b).get_type() || a != Mixed(b).get_int();
}


// Compare mixed with boolean

inline bool operator==(Wrap<Mixed> a, bool b) noexcept
{
    return Mixed(a).get_type() == type_Bool && Mixed(a).get_bool() == b;
}

inline bool operator!=(Wrap<Mixed> a, bool b) noexcept
{
    return Mixed(a).get_type() != type_Bool || Mixed(a).get_bool() != b;
}

inline bool operator==(bool a, Wrap<Mixed> b) noexcept
{
    return type_Bool == Mixed(b).get_type() && a == Mixed(b).get_bool();
}

inline bool operator!=(bool a, Wrap<Mixed> b) noexcept
{
    return type_Bool != Mixed(b).get_type() || a != Mixed(b).get_bool();
}


// Compare mixed with float

inline bool operator==(Wrap<Mixed> a, float b)
{
    return Mixed(a).get_type() == type_Float && Mixed(a).get_float() == b;
}

inline bool operator!=(Wrap<Mixed> a, float b)
{
    return Mixed(a).get_type() != type_Float || Mixed(a).get_float() != b;
}

inline bool operator==(float a, Wrap<Mixed> b)
{
    return type_Float == Mixed(b).get_type() && a == Mixed(b).get_float();
}

inline bool operator!=(float a, Wrap<Mixed> b)
{
    return type_Float != Mixed(b).get_type() || a != Mixed(b).get_float();
}


// Compare mixed with double

inline bool operator==(Wrap<Mixed> a, double b)
{
    return Mixed(a).get_type() == type_Double && Mixed(a).get_double() == b;
}

inline bool operator!=(Wrap<Mixed> a, double b)
{
    return Mixed(a).get_type() != type_Double || Mixed(a).get_double() != b;
}

inline bool operator==(double a, Wrap<Mixed> b)
{
    return type_Double == Mixed(b).get_type() && a == Mixed(b).get_double();
}

inline bool operator!=(double a, Wrap<Mixed> b)
{
    return type_Double != Mixed(b).get_type() || a != Mixed(b).get_double();
}


// Compare mixed with string

inline bool operator==(Wrap<Mixed> a, StringData b) noexcept
{
    return Mixed(a).get_type() == type_String && Mixed(a).get_string() == b;
}

inline bool operator!=(Wrap<Mixed> a, StringData b) noexcept
{
    return Mixed(a).get_type() != type_String || Mixed(a).get_string() != b;
}

inline bool operator==(StringData a, Wrap<Mixed> b) noexcept
{
    return type_String == Mixed(b).get_type() && a == Mixed(b).get_string();
}

inline bool operator!=(StringData a, Wrap<Mixed> b) noexcept
{
    return type_String != Mixed(b).get_type() || a != Mixed(b).get_string();
}

inline bool operator==(Wrap<Mixed> a, const char* b) noexcept
{
    return a == StringData(b);
}

inline bool operator!=(Wrap<Mixed> a, const char* b) noexcept
{
    return a != StringData(b);
}

inline bool operator==(const char* a, Wrap<Mixed> b) noexcept
{
    return StringData(a) == b;
}

inline bool operator!=(const char* a, Wrap<Mixed> b) noexcept
{
    return StringData(a) != b;
}

inline bool operator==(Wrap<Mixed> a, char* b) noexcept
{
    return a == StringData(b);
}

inline bool operator!=(Wrap<Mixed> a, char* b) noexcept
{
    return a != StringData(b);
}

inline bool operator==(char* a, Wrap<Mixed> b) noexcept
{
    return StringData(a) == b;
}

inline bool operator!=(char* a, Wrap<Mixed> b) noexcept
{
    return StringData(a) != b;
}


// Compare mixed with binary data

inline bool operator==(Wrap<Mixed> a, BinaryData b) noexcept
{
    return Mixed(a).get_type() == type_Binary && Mixed(a).get_binary() == b;
}

inline bool operator!=(Wrap<Mixed> a, BinaryData b) noexcept
{
    return Mixed(a).get_type() != type_Binary || Mixed(a).get_binary() != b;
}

inline bool operator==(BinaryData a, Wrap<Mixed> b) noexcept
{
    return type_Binary == Mixed(b).get_type() && a == Mixed(b).get_binary();
}

inline bool operator!=(BinaryData a, Wrap<Mixed> b) noexcept
{
    return type_Binary != Mixed(b).get_type() || a != Mixed(b).get_binary();
}


// Compare mixed with date

inline bool operator==(Wrap<Mixed> a, OldDateTime b) noexcept
{
    return Mixed(a).get_type() == type_OldDateTime && OldDateTime(Mixed(a).get_olddatetime()) == b;
}

inline bool operator!=(Wrap<Mixed> a, OldDateTime b) noexcept
{
    return Mixed(a).get_type() != type_OldDateTime || OldDateTime(Mixed(a).get_olddatetime()) != b;
}

inline bool operator==(OldDateTime a, Wrap<Mixed> b) noexcept
{
    return type_OldDateTime == Mixed(b).get_type() && a == OldDateTime(Mixed(b).get_olddatetime());
}

inline bool operator!=(OldDateTime a, Wrap<Mixed> b) noexcept
{
    return type_OldDateTime != Mixed(b).get_type() || a != OldDateTime(Mixed(b).get_olddatetime());
}

// Compare mixed with Timestamp

inline bool operator==(Wrap<Mixed> a, Timestamp b) noexcept
{
    return Mixed(a).get_type() == type_Timestamp && Timestamp(Mixed(a).get_timestamp()) == b;
}

inline bool operator!=(Wrap<Mixed> a, Timestamp b) noexcept
{
    return Mixed(a).get_type() != type_Timestamp || Timestamp(Mixed(a).get_timestamp()) != b;
}

inline bool operator==(Timestamp a, Wrap<Mixed> b) noexcept
{
    return type_Timestamp == Mixed(b).get_type() && a == Timestamp(Mixed(b).get_timestamp());
}

inline bool operator!=(Timestamp a, Wrap<Mixed> b) noexcept
{
    return type_Timestamp != Mixed(b).get_type() || a != Timestamp(Mixed(b).get_timestamp());
}

=======
    REALM_ASSERT(get_type() == type_String);
    return StringData(str_val, ushort_val);
}

inline StringData Mixed::get_string() const
{
    return get<StringData>();
}

template <>
inline BinaryData Mixed::get<BinaryData>() const noexcept
{
    REALM_ASSERT(get_type() == type_Binary);
    return BinaryData(str_val, ushort_val);
}

inline BinaryData Mixed::get_binary() const
{
    return get<BinaryData>();
}

template <>
inline Timestamp Mixed::get<Timestamp>() const noexcept
{
    REALM_ASSERT(get_type() == type_Timestamp);
    return Timestamp(int_val, short_val);
}

inline Timestamp Mixed::get_timestamp() const
{
    return get<Timestamp>();
}

template <>
inline ObjKey Mixed::get<ObjKey>() const noexcept
{
    REALM_ASSERT(get_type() == type_Link);
    return ObjKey(int_val);
}

inline bool Mixed::is_null() const
{
    return (m_type == 0);
}

std::ostream& operator<<(std::ostream& out, const Mixed& m);
>>>>>>> origin/develop12

} // namespace realm

#endif // REALM_MIXED_HPP
