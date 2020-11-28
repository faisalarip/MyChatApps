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

#ifndef REALM_ARRAY_STRING_HPP
#define REALM_ARRAY_STRING_HPP

<<<<<<< HEAD
#include <realm/array.hpp>

namespace realm {

/*
ArrayString stores strings as a concecutive list of fixed-length blocks of m_width bytes. The
longest string it can store is (m_width - 1) bytes before it needs to expand.

An example of the format for m_width = 4 is following sequence of bytes, where x is payload:

xxx0 xx01 x002 0003 0004 (strings "xxx",. "xx", "x", "", realm::null())

So each string is 0 terminated, and the last byte in a block tells how many 0s are present, except
for a realm::null() which has the byte set to m_width (4). The byte is used to compute the length of a string
in various functions.

New: If m_witdh = 0, then all elements are realm::null(). So to add an empty string we must expand m_width
New: StringData is null() if-and-only-if StringData::data() == 0.
*/

class ArrayString : public Array {
public:
    static const size_t max_width = 64;

    typedef StringData value_type;
    // Constructor defaults to non-nullable because we use non-nullable ArrayString so many places internally in core
    // (data which isn't user payload) where null isn't needed.
    explicit ArrayString(Allocator&, bool nullable = false) noexcept;
    ~ArrayString() noexcept override
    {
    }

    bool is_null(size_t ndx) const;
    void set_null(size_t ndx);
    StringData get(size_t ndx) const noexcept;
    StringData get_string(size_t ndx) const noexcept { return get(ndx); }
    void add();
    void add(StringData value);
    void set(size_t ndx, StringData value);
    void insert(size_t ndx, StringData value);
    void erase(size_t ndx);

    size_t count(StringData value, size_t begin = 0, size_t end = npos) const noexcept;
    size_t find_first(StringData value, size_t begin = 0, size_t end = npos) const noexcept;
    void find_all(IntegerColumn& result, StringData value, size_t add_offset = 0, size_t begin = 0,
                  size_t end = npos);

    /// Compare two string arrays for equality.
    bool compare_string(const ArrayString&) const noexcept;
=======
#include <realm/array_string_short.hpp>
#include <realm/array_blobs_small.hpp>
#include <realm/array_blobs_big.hpp>

namespace realm {

class Spec;

class ArrayString : public ArrayPayload {
public:
    using value_type = StringData;

    explicit ArrayString(Allocator&);

    static StringData default_value(bool nullable)
    {
        return nullable ? StringData{} : StringData{""};
    }

    // This is only used in the upgrade process
    void set_nullability(bool n)
    {
        m_nullable = n;
    }
    void create();

    bool is_attached() const
    {
        return m_arr->is_attached();
    }

    ref_type get_ref() const
    {
        return m_arr->get_ref();
    }
    ArrayParent* get_parent() const
    {
        return m_arr->get_parent();
    }
    size_t get_ndx_in_parent() const
    {
        return m_arr->get_ndx_in_parent();
    }
    void set_parent(ArrayParent* p, size_t n) noexcept override
    {
        m_arr->set_parent(p, n);
    }
    bool need_spec() const override
    {
        return true;
    }
    void set_spec(Spec* spec, size_t col_ndx) const override
    {
        m_spec = spec;
        m_col_ndx = col_ndx;
    }

    void update_parent()
    {
        m_arr->update_parent();
    }

    void init_from_mem(MemRef mem) noexcept;
    void init_from_ref(ref_type ref) noexcept override
    {
        init_from_mem(MemRef(m_alloc.translate(ref), ref, m_alloc));
    }
    void init_from_parent();
    void destroy();

    size_t size() const;

    void add(StringData value);
    void set(size_t ndx, StringData value);
    void set_null(size_t ndx)
    {
        set(ndx, StringData{});
    }
    void insert(size_t ndx, StringData value);
    StringData get(size_t ndx) const;
    StringData get_legacy(size_t ndx) const;
    bool is_null(size_t ndx) const;
    void erase(size_t ndx);
    void move(ArrayString& dst, size_t ndx);
    void clear();

    size_t find_first(StringData value, size_t begin, size_t end) const noexcept;

    size_t lower_bound(StringData value);
>>>>>>> origin/develop12

    /// Get the specified element without the cost of constructing an
    /// array instance. If an array instance is already available, or
    /// you need to get multiple values, then this method will be
    /// slower.
<<<<<<< HEAD
    static StringData get(const char* header, size_t ndx, bool nullable) noexcept;

    ref_type bptree_leaf_insert(size_t ndx, StringData, TreeInsertBase& state);

    /// Construct a string array of the specified size and return just
    /// the reference to the underlying memory. All elements will be
    /// initialized to the empty string.
    static MemRef create_array(size_t size, Allocator&);

    /// Create a new empty string array and attach this accessor to
    /// it. This does not modify the parent reference information of
    /// this accessor.
    ///
    /// Note that the caller assumes ownership of the allocated
    /// underlying node. It is not owned by the accessor.
    void create();

    /// Construct a copy of the specified slice of this string array
    /// using the specified target allocator.
    MemRef slice(size_t offset, size_t slice_size, Allocator& target_alloc) const;

#ifdef REALM_DEBUG
    void string_stats() const;
    void to_dot(std::ostream&, StringData title = StringData()) const;
#endif

private:
    size_t calc_byte_len(size_t num_items, size_t width) const override;
    size_t calc_item_count(size_t bytes, size_t width) const noexcept override;

    bool m_nullable;
};


// Implementation:

// Creates new array (but invalid, call init_from_ref() to init)
inline ArrayString::ArrayString(Allocator& allocator, bool nullable) noexcept
    : Array(allocator)
    , m_nullable(nullable)
{
}

inline void ArrayString::create()
{
    size_t init_size = 0;
    MemRef mem = create_array(init_size, get_alloc()); // Throws
    init_from_mem(mem);
}

inline MemRef ArrayString::create_array(size_t init_size, Allocator& allocator)
{
    bool context_flag = false;
    int_fast64_t value = 0;
    return Array::create(type_Normal, context_flag, wtype_Multiply, init_size, value, allocator); // Throws
}

inline StringData ArrayString::get(size_t ndx) const noexcept
{
    REALM_ASSERT_3(ndx, <, m_size);
    if (m_width == 0)
        return m_nullable ? realm::null() : StringData("");

    const char* data = m_data + (ndx * m_width);
    size_t array_size = (m_width - 1) - data[m_width - 1];

    if (array_size == static_cast<size_t>(-1))
        return m_nullable ? realm::null() : StringData("");

    REALM_ASSERT_EX(data[array_size] == 0, data[array_size],
                    array_size); // Realm guarantees 0 terminated return strings
    return StringData(data, array_size);
}

inline void ArrayString::add(StringData value)
{
    REALM_ASSERT(!(!m_nullable && value.is_null()));
    insert(m_size, value); // Throws
}

inline void ArrayString::add()
{
    add(m_nullable ? realm::null() : StringData("")); // Throws
}

inline StringData ArrayString::get(const char* header, size_t ndx, bool nullable) noexcept
{
    REALM_ASSERT(ndx < get_size_from_header(header));
    uint_least8_t width = get_width_from_header(header);
    const char* data = get_data_from_header(header) + (ndx * width);

    if (width == 0)
        return nullable ? realm::null() : StringData("");

    size_t size = (width - 1) - data[width - 1];

    if (size == static_cast<size_t>(-1))
        return nullable ? realm::null() : StringData("");

    return StringData(data, size);
}


} // namespace realm

#endif // REALM_ARRAY_STRING_HPP
=======
    static StringData get(const char* header, size_t ndx, Allocator& alloc) noexcept;

    void verify() const;

private:
    static constexpr size_t small_string_max_size = 15;  // ArrayStringShort
    static constexpr size_t medium_string_max_size = 63; // ArrayStringLong
    union Storage {
        std::aligned_storage<sizeof(ArrayStringShort), alignof(ArrayStringShort)>::type m_string_short;
        std::aligned_storage<sizeof(ArraySmallBlobs), alignof(ArraySmallBlobs)>::type m_string_long;
        std::aligned_storage<sizeof(ArrayBigBlobs), alignof(ArrayBigBlobs)>::type m_big_blobs;
        std::aligned_storage<sizeof(ArrayInteger), alignof(ArrayInteger)>::type m_enum;
    };
    enum class Type { small_strings, medium_strings, big_strings, enum_strings };

    Type m_type = Type::small_strings;

    Allocator& m_alloc;
    Storage m_storage;
    Array* m_arr;
    mutable Spec* m_spec = nullptr;
    mutable size_t m_col_ndx = realm::npos;
    bool m_nullable = true;

    std::unique_ptr<ArrayString> m_string_enum_values;

    Type upgrade_leaf(size_t value_size);
};

inline StringData ArrayString::get(const char* header, size_t ndx, Allocator& alloc) noexcept
{
    bool long_strings = Array::get_hasrefs_from_header(header);
    if (!long_strings) {
        return ArrayStringShort::get(header, ndx, true);
    }
    else {
        bool is_big = Array::get_context_flag_from_header(header);
        if (!is_big) {
            return ArraySmallBlobs::get_string(header, ndx, alloc);
        }
        else {
            return ArrayBigBlobs::get_string(header, ndx, alloc);
        }
    }
}

template <>
class QueryState<StringData> : public QueryStateBase {
public:
    StringData m_state;

    template <Action action>
    bool uses_val()
    {
        return (action == act_Count);
    }

    QueryState(Action, Array* = nullptr, size_t limit = -1)
        : QueryStateBase(limit)
    {
    }

    template <Action action, bool pattern>
    inline bool match(size_t, uint64_t, StringData)
    {
        if (pattern)
            return false;

        if (action == act_Count) {
            ++m_match_count;
        }
        else {
            REALM_ASSERT_DEBUG(false);
        }

        return (m_limit > m_match_count);
    }
};
}

#endif /* REALM_ARRAY_STRING_HPP */
>>>>>>> origin/develop12
