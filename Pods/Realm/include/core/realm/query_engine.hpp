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

/*
A query consists of node objects, one for each query condition. Each node contains pointers to all other nodes:

node1        node2         node3
------       -----         -----
node2*       node1*        node1*
node3*       node3*        node2*

The construction of all this takes part in query.cpp. Each node has two important functions:

    aggregate(start, end)
    aggregate_local(start, end)

The aggregate() function executes the aggregate of a query. You can call the method on any of the nodes
(except children nodes of OrNode and SubtableNode) - it has the same behaviour. The function contains
scheduling that calls aggregate_local(start, end) on different nodes with different start/end ranges,
depending on what it finds is most optimal.

The aggregate_local() function contains a tight loop that tests the condition of its own node, and upon match
it tests all other conditions at that index to report a full match or not. It will remain in the tight loop
after a full match.

So a call stack with 2 and 9 being local matches of a node could look like this:

aggregate(0, 10)
    node1->aggregate_local(0, 3)
        node2->find_first_local(2, 3)
        node3->find_first_local(2, 3)
    node3->aggregate_local(3, 10)
        node1->find_first_local(4, 5)
        node2->find_first_local(4, 5)
        node1->find_first_local(7, 8)
        node2->find_first_local(7, 8)

find_first_local(n, n + 1) is a function that can be used to test a single row of another condition. Note that
this is very simplified. There are other statistical arguments to the methods, and also, find_first_local() can be
called from a callback function called by an integer Array.


Template arguments in methods:
----------------------------------------------------------------------------------------------------

TConditionFunction: Each node has a condition from query_conditions.c such as Equal, GreaterEqual, etc

TConditionValue:    Type of values in condition column. That is, int64_t, float, int, bool, etc

TAction:            What to do with each search result, from the enums act_ReturnFirst, act_Count, act_Sum, etc

TResult:            Type of result of actions - float, double, int64_t, etc. Special notes: For act_Count it's
                    int64_t, for RLM_FIND_ALL it's int64_t which points at destination array.

TSourceColumn:      Type of source column used in actions, or *ignored* if no source column is used (like for
                    act_Count, act_ReturnFirst)


There are two important classes used in queries:
----------------------------------------------------------------------------------------------------
SequentialGetter    Column iterator used to get successive values with leaf caching. Used both for condition columns
                    and aggregate source column

AggregateState      State of the aggregate - contains a state variable that stores intermediate sum, max, min,
                    etc, etc.

*/

#ifndef REALM_QUERY_ENGINE_HPP
#define REALM_QUERY_ENGINE_HPP

#include <algorithm>
#include <functional>
#include <sstream>
#include <string>
#include <array>

#include <realm/array_basic.hpp>
<<<<<<< HEAD
#include <realm/array_string.hpp>
#include <realm/column_binary.hpp>
#include <realm/column_fwd.hpp>
#include <realm/column_link.hpp>
#include <realm/column_linklist.hpp>
#include <realm/column_mixed.hpp>
#include <realm/column_string.hpp>
#include <realm/column_string_enum.hpp>
#include <realm/column_table.hpp>
#include <realm/column_timestamp.hpp>
#include <realm/column_type_traits.hpp>
#include <realm/column_type_traits.hpp>
#include <realm/impl/sequential_getter.hpp>
#include <realm/link_view.hpp>
#include <realm/metrics/query_info.hpp>
#include <realm/query_conditions.hpp>
#include <realm/query_operators.hpp>
#include <realm/table.hpp>
=======
#include <realm/array_key.hpp>
#include <realm/array_string.hpp>
#include <realm/array_binary.hpp>
#include <realm/array_timestamp.hpp>
#include <realm/array_list.hpp>
#include <realm/array_backlink.hpp>
#include <realm/column_type_traits.hpp>
#include <realm/metrics/query_info.hpp>
#include <realm/query_conditions.hpp>
#include <realm/table.hpp>
#include <realm/column_integer.hpp>
>>>>>>> origin/develop12
#include <realm/unicode.hpp>
#include <realm/util/miscellaneous.hpp>
#include <realm/util/serializer.hpp>
#include <realm/util/shared_ptr.hpp>
#include <realm/util/string_buffer.hpp>
#include <realm/utilities.hpp>
<<<<<<< HEAD
=======
#include <realm/index_string.hpp>
>>>>>>> origin/develop12

#include <map>
#include <unordered_set>

#if REALM_X86_OR_X64_TRUE && defined(_MSC_FULL_VER) && _MSC_FULL_VER >= 160040219
#include <immintrin.h>
#endif

namespace realm {

// Number of matches to find in best condition loop before breaking out to probe other conditions. Too low value gives
// too many constant time overheads everywhere in the query engine. Too high value makes it adapt less rapidly to
// changes in match frequencies.
const size_t findlocals = 64;

// Average match distance in linear searches where further increase in distance no longer increases query speed
// (because time spent on handling each match becomes insignificant compared to time spent on the search).
const size_t bestdist = 512;

// Minimum number of matches required in a certain condition before it can be used to compute statistics. Too high
// value can spent too much time in a bad node (with high match frequency). Too low value gives inaccurate statistics.
const size_t probe_matches = 4;

const size_t bitwidth_time_unit = 64;

typedef bool (*CallbackDummy)(int64_t);
<<<<<<< HEAD
=======
using Evaluator = util::FunctionRef<bool(ConstObj& obj)>;
>>>>>>> origin/develop12

class ParentNode {
    typedef ParentNode ThisType;

public:
    ParentNode() = default;
    virtual ~ParentNode() = default;

<<<<<<< HEAD
=======
    virtual bool has_search_index() const
    {
        return false;
    }
    virtual void index_based_aggregate(size_t, Evaluator) {}

>>>>>>> origin/develop12
    void gather_children(std::vector<ParentNode*>& v)
    {
        m_children.clear();
        size_t i = v.size();
        v.push_back(this);

        if (m_child)
            m_child->gather_children(v);

        m_children = v;
        m_children.erase(m_children.begin() + i);
        m_children.insert(m_children.begin(), this);
    }

    double cost() const
    {
        return 8 * bitwidth_time_unit / m_dD +
               m_dT; // dt = 1/64 to 1. Match dist is 8 times more important than bitwidth
    }

    size_t find_first(size_t start, size_t end);

<<<<<<< HEAD
    virtual void init()
    {
        // Verify that the cached column accessor is still valid
        verify_column(); // throws

        if (m_child)
            m_child->init();
=======
    bool match(ConstObj& obj);

    virtual void init(bool will_query_ranges)
    {
        if (m_child)
            m_child->init(will_query_ranges);
>>>>>>> origin/develop12

        m_column_action_specializer = nullptr;
    }

<<<<<<< HEAD
    void set_table(const Table& table)
    {
        if (&table == m_table)
            return;

        m_table.reset(&table);
=======
    void get_link_dependencies(std::vector<TableKey>& tables) const
    {
        collect_dependencies(tables);
        if (m_child)
            m_child->get_link_dependencies(tables);
    }

    void set_table(ConstTableRef table)
    {
        if (table == m_table)
            return;

        m_table = table;
        if (m_condition_column_key != ColKey()) {
            m_condition_column_name = m_table->get_column_name(m_condition_column_key);
        }
>>>>>>> origin/develop12
        if (m_child)
            m_child->set_table(table);
        table_changed();
    }

<<<<<<< HEAD
=======
    void set_cluster(const Cluster* cluster)
    {
        m_cluster = cluster;
        if (m_child)
            m_child->set_cluster(cluster);
        cluster_changed();
    }

    virtual void collect_dependencies(std::vector<TableKey>&) const
    {
    }

>>>>>>> origin/develop12
    virtual size_t find_first_local(size_t start, size_t end) = 0;

    virtual void aggregate_local_prepare(Action TAction, DataType col_id, bool nullable);

<<<<<<< HEAD
    template <Action TAction, class TSourceColumn>
    bool column_action_specialization(QueryStateBase* st, SequentialGetterBase* source_column, size_t r)
    {
        // TResult: type of query result
        // TSourceValue: type of aggregate source
        using TSourceValue = typename TSourceColumn::value_type;
        using TResult = typename ColumnTypeTraitsSum<TSourceValue, TAction>::sum_type;

        // Sum of float column must accumulate in double
        static_assert(!(TAction == act_Sum &&
                        (std::is_same<TSourceColumn, float>::value && !std::is_same<TResult, double>::value)),
=======
    template <Action TAction, class LeafType>
    bool column_action_specialization(QueryStateBase* st, ArrayPayload* source_column, size_t r)
    {
        // TResult: type of query result
        // TSourceValue: type of aggregate source
        using TSourceValue = typename LeafType::value_type;
        using TResult = typename AggregateResultType<TSourceValue, TAction>::result_type;

        // Sum of float column must accumulate in double
        static_assert(!(TAction == act_Sum &&
                        (std::is_same<TSourceValue, float>::value && !std::is_same<TResult, double>::value)),
>>>>>>> origin/develop12
                      "");

        TSourceValue av{};
        // uses_val test because compiler cannot see that IntegerColumn::get has no side effect and result is
        // discarded
        if (static_cast<QueryState<TResult>*>(st)->template uses_val<TAction>() && source_column != nullptr) {
<<<<<<< HEAD
            REALM_ASSERT_DEBUG(dynamic_cast<SequentialGetter<TSourceColumn>*>(source_column) != nullptr);
            av = static_cast<SequentialGetter<TSourceColumn>*>(source_column)->get_next(r);
=======
            REALM_ASSERT_DEBUG(dynamic_cast<LeafType*>(source_column) != nullptr);
            av = static_cast<LeafType*>(source_column)->get(r);
>>>>>>> origin/develop12
        }
        REALM_ASSERT_DEBUG(dynamic_cast<QueryState<TResult>*>(st) != nullptr);
        bool cont = static_cast<QueryState<TResult>*>(st)->template match<TAction, 0>(r, 0, av);
        return cont;
    }

    virtual size_t aggregate_local(QueryStateBase* st, size_t start, size_t end, size_t local_limit,
<<<<<<< HEAD
                                   SequentialGetterBase* source_column);
=======
                                   ArrayPayload* source_column);
>>>>>>> origin/develop12


    virtual std::string validate()
    {
        if (error_code != "")
            return error_code;
        if (m_child == nullptr)
            return "";
        else
            return m_child->validate();
    }

<<<<<<< HEAD
    ParentNode(const ParentNode& from)
        : ParentNode(from, nullptr)
    {
    }

    ParentNode(const ParentNode& from, QueryNodeHandoverPatches* patches)
        : m_child(from.m_child ? from.m_child->clone(patches) : nullptr)
        , m_condition_column_idx(from.m_condition_column_idx)
        , m_dD(from.m_dD)
        , m_dT(from.m_dT)
        , m_probes(from.m_probes)
        , m_matches(from.m_matches)
        , m_table(patches ? ConstTableRef{} : from.m_table)
    {
    }
=======
    ParentNode(const ParentNode& from);
>>>>>>> origin/develop12

    void add_child(std::unique_ptr<ParentNode> child)
    {
        if (m_child)
            m_child->add_child(std::move(child));
        else
            m_child = std::move(child);
    }

<<<<<<< HEAD
    virtual std::unique_ptr<ParentNode> clone(QueryNodeHandoverPatches* = nullptr) const = 0;

    virtual void apply_handover_patch(QueryNodeHandoverPatches& patches, Group& group)
    {
        if (m_child)
            m_child->apply_handover_patch(patches, group);
    }

    virtual void verify_column() const = 0;

=======
    virtual std::unique_ptr<ParentNode> clone() const = 0;

    ColKey get_column_key(StringData column_name) const
    {
        ColKey column_key;
        if (column_name.size() > 0) {
            column_key = m_table.unchecked_ptr()->get_column_key(column_name);
            if (column_key == ColKey()) {
                throw LogicError(LogicError::column_does_not_exist);
            }
        }
        return column_key;
    }

>>>>>>> origin/develop12
    virtual std::string describe(util::serializer::SerialisationState&) const
    {
        return "";
    }

    virtual std::string describe_condition() const
    {
        return "matches";
    }

    virtual std::string describe_expression(util::serializer::SerialisationState& state) const
    {
        std::string s;
        s = describe(state);
        if (m_child) {
            s = s + " and " + m_child->describe_expression(state);
        }
        return s;
    }

<<<<<<< HEAD
    std::unique_ptr<ParentNode> m_child;
    std::vector<ParentNode*> m_children;
    size_t m_condition_column_idx = npos; // Column of search criteria
=======
    bool consume_condition(ParentNode& other, bool ignore_indexes)
    {
        // We can only combine conditions if they're the same operator on the
        // same column and there's no additional conditions ANDed on
        if (m_condition_column_key != other.m_condition_column_key)
            return false;
        if (m_child || other.m_child)
            return false;
        if (typeid(*this) != typeid(other))
            return false;

        // If a search index is present, don't try to combine conditions since index search is most likely faster.
        // Assuming N elements to search and M conditions to check:
        // 1) search index present:                     O(log(N)*M)
        // 2) no search index, combine conditions:      O(N)
        // 3) no search index, conditions not combined: O(N*M)
        // In practice N is much larger than M, so if we have a search index, choose 1, otherwise if possible
        // choose 2. The exception is if we're inside a Not group or if the query is restricted to a view, as in those
        // cases end will always be start+1 and we'll have O(N*M) runtime even with a search index, so we want to
        // combine even with an index.
        if (has_search_index() && !ignore_indexes)
            return false;
        return do_consume_condition(other);
    }

    std::unique_ptr<ParentNode> m_child;
    std::vector<ParentNode*> m_children;
    std::string m_condition_column_name;
    mutable ColKey m_condition_column_key = ColKey(); // Column of search criteria
>>>>>>> origin/develop12

    double m_dD;       // Average row distance between each local match at current position
    double m_dT = 0.0; // Time overhead of testing index i + 1 if we have just tested index i. > 1 for linear scans, 0
    // for index/tableview

    size_t m_probes = 0;
    size_t m_matches = 0;

protected:
<<<<<<< HEAD
    typedef bool (ParentNode::*Column_action_specialized)(QueryStateBase*, SequentialGetterBase*, size_t);
    Column_action_specialized m_column_action_specializer;
    ConstTableRef m_table;
    std::string error_code;

    const ColumnBase& get_column_base(size_t ndx)
    {
        return m_table->get_column_base(ndx);
    }

    template <class ColType>
    const ColType& get_column(size_t ndx)
    {
        auto& col = m_table->get_column_base(ndx);
        REALM_ASSERT_DEBUG(dynamic_cast<const ColType*>(&col));
        return static_cast<const ColType&>(col);
    }

    ColumnType get_real_column_type(size_t ndx)
    {
        return m_table->get_real_column_type(ndx);
    }

    template <class ColType>
    void copy_getter(SequentialGetter<ColType>& dst, size_t& dst_idx, const SequentialGetter<ColType>& src,
                     const QueryNodeHandoverPatches* patches)
    {
        if (src.m_column) {
            if (patches)
                dst_idx = src.m_column->get_column_index();
            else
                dst.init(src.m_column);
        }
    }

    void do_verify_column(const ColumnBase* col, size_t col_ndx = npos) const
    {
        if (col_ndx == npos)
            col_ndx = m_condition_column_idx;
        if (m_table && col_ndx != npos) {
            m_table->verify_column(col_ndx, col);
        }
    }

private:
    virtual void table_changed() = 0;
};

// For conditions on a subtable (encapsulated in subtable()...end_subtable()). These return the parent row as match if
// and only if one or more subtable rows match the condition.
class SubtableNode : public ParentNode {
public:
    SubtableNode(size_t column, std::unique_ptr<ParentNode> condition)
        : m_condition(std::move(condition))
    {
        m_dT = 100.0;
        m_condition_column_idx = column;
    }

    void init() override
    {
        ParentNode::init();

        m_dD = 10.0;

        // m_condition is first node in condition of subtable query.
        if (m_condition) {
            // Can't call init() here as usual since the subtable can be degenerate
            // m_condition->init(table);
            std::vector<ParentNode*> v;
            m_condition->gather_children(v);
        }
    }

    void table_changed() override
    {
        m_col_type = m_table->get_real_column_type(m_condition_column_idx);
        REALM_ASSERT(m_col_type == col_type_Table || m_col_type == col_type_Mixed);
        if (m_col_type == col_type_Table)
            m_column = &m_table->get_column_table(m_condition_column_idx);
        else // Mixed
            m_column = &m_table->get_column_mixed(m_condition_column_idx);
    }

    void verify_column() const override
    {
        if (m_table)
            m_table->verify_column(m_condition_column_idx, m_column);
    }

    std::string validate() override
    {
        if (error_code != "")
            return error_code;
        if (m_condition == nullptr)
            return "Unbalanced subtable/end_subtable block";
        else
            return m_condition->validate();
    }

    std::string describe(util::serializer::SerialisationState&) const override
    {
        throw SerialisationError("Serialising a query which contains a subtable expression is currently unsupported.");
    }


    size_t find_first_local(size_t start, size_t end) override
    {
        REALM_ASSERT(m_table);
        REALM_ASSERT(m_condition);

        for (size_t s = start; s < end; ++s) {
            ConstTableRef subtable; // TBD: optimize this back to Table*
            if (m_col_type == col_type_Table)
                subtable = static_cast<const SubtableColumn*>(m_column)->get_subtable_tableref(s);
            else {
                subtable = static_cast<const MixedColumn*>(m_column)->get_subtable_tableref(s);
                if (!subtable)
                    continue;
            }

            if (subtable->is_degenerate())
                return not_found;

            m_condition->set_table(*subtable);
            m_condition->init();
            const size_t subsize = subtable->size();
            const size_t sub = m_condition->find_first(0, subsize);

            if (sub != not_found)
                return s;
        }
        return not_found;
    }

    std::unique_ptr<ParentNode> clone(QueryNodeHandoverPatches* patches) const override
    {
        return std::unique_ptr<ParentNode>(new SubtableNode(*this, patches));
    }

    SubtableNode(const SubtableNode& from, QueryNodeHandoverPatches* patches)
        : ParentNode(from, patches)
        , m_condition(from.m_condition ? from.m_condition->clone(patches) : nullptr)
        , m_column(from.m_column)
        , m_col_type(from.m_col_type)
    {
        if (m_column && patches)
            m_condition_column_idx = m_column->get_column_index();
    }

    void apply_handover_patch(QueryNodeHandoverPatches& patches, Group& group) override
    {
        m_condition->apply_handover_patch(patches, group);
        ParentNode::apply_handover_patch(patches, group);
    }

    std::unique_ptr<ParentNode> m_condition;
    const ColumnBase* m_column = nullptr;
    ColumnType m_col_type;
};

namespace _impl {

template <class ColType>
struct CostHeuristic;

template <>
struct CostHeuristic<IntegerColumn> {
=======
    typedef bool (ParentNode::*Column_action_specialized)(QueryStateBase*, ArrayPayload*, size_t);
    Column_action_specialized m_column_action_specializer = nullptr;
    ConstTableRef m_table = ConstTableRef();
    const Cluster* m_cluster = nullptr;
    QueryStateBase* m_state = nullptr;
    std::string error_code;

    ColumnType get_real_column_type(ColKey key)
    {
        return m_table.unchecked_ptr()->get_real_column_type(key);
    }

private:
    virtual void table_changed()
    {
    }
    virtual void cluster_changed()
    {
        // TODO: Should eventually be pure
    }
    virtual bool do_consume_condition(ParentNode&)
    {
        return false;
    }
};


namespace _impl {

template <class LeafType>
struct CostHeuristic;

template <>
struct CostHeuristic<ArrayInteger> {
>>>>>>> origin/develop12
    static constexpr double dD()
    {
        return 100.0;
    }
    static constexpr double dT()
    {
        return 1.0 / 4.0;
    }
};

template <>
<<<<<<< HEAD
struct CostHeuristic<IntNullColumn> {
=======
struct CostHeuristic<ArrayIntNull> {
>>>>>>> origin/develop12
    static constexpr double dD()
    {
        return 100.0;
    }
    static constexpr double dT()
    {
        return 1.0 / 4.0;
    }
};

// FIXME: Add AdaptiveStringColumn, BasicColumn, etc.
}

class ColumnNodeBase : public ParentNode {
protected:
<<<<<<< HEAD
    ColumnNodeBase(size_t column_idx)
    {
        m_condition_column_idx = column_idx;
    }

    ColumnNodeBase(const ColumnNodeBase& from, QueryNodeHandoverPatches* patches)
        : ParentNode(from, patches)
=======
    ColumnNodeBase(ColKey column_key)
    {
        m_condition_column_key = column_key;
    }

    ColumnNodeBase(const ColumnNodeBase& from)
        : ParentNode(from)
>>>>>>> origin/develop12
        , m_last_local_match(from.m_last_local_match)
        , m_local_matches(from.m_local_matches)
        , m_local_limit(from.m_local_limit)
        , m_fastmode_disabled(from.m_fastmode_disabled)
        , m_action(from.m_action)
        , m_state(from.m_state)
        , m_source_column(from.m_source_column)
    {
    }

<<<<<<< HEAD
    template <Action TAction, class ColType>
    bool match_callback(int64_t v)
    {
        using TSourceValue = typename ColType::value_type;
        using QueryStateType = typename ColumnTypeTraitsSum<TSourceValue, TAction>::sum_type;
=======
    template <Action TAction, class LeafType>
    bool match_callback(int64_t v)
    {
        using TSourceValue = typename LeafType::value_type;
        using ResultType = typename AggregateResultType<TSourceValue, TAction>::result_type;
>>>>>>> origin/develop12

        size_t i = to_size_t(v);
        m_last_local_match = i;
        m_local_matches++;

<<<<<<< HEAD
        auto state = static_cast<QueryState<QueryStateType>*>(m_state);
        auto source_column = static_cast<SequentialGetter<ColType>*>(m_source_column);
=======
        auto state = static_cast<QueryState<ResultType>*>(m_state);
        auto source_column = static_cast<LeafType*>(m_source_column);
>>>>>>> origin/develop12

        // Test remaining sub conditions of this node. m_children[0] is the node that called match_callback(), so skip
        // it
        for (size_t c = 1; c < m_children.size(); c++) {
            m_children[c]->m_probes++;
            size_t m = m_children[c]->find_first_local(i, i + 1);
            if (m != i)
                return true;
        }

        bool b;
        if (state->template uses_val<TAction>()) { // Compiler cannot see that IntegerColumn::Get has no side effect
            // and result is discarded
<<<<<<< HEAD
            TSourceValue av = source_column->get_next(i);
=======
            TSourceValue av = source_column->get(i);
>>>>>>> origin/develop12
            b = state->template match<TAction, false>(i, 0, av);
        }
        else {
            b = state->template match<TAction, false>(i, 0, TSourceValue{});
        }

        return b;
    }

    // Aggregate bookkeeping
    size_t m_last_local_match = npos;
    size_t m_local_matches = 0;
    size_t m_local_limit = 0;
    bool m_fastmode_disabled = false;
    Action m_action;
    QueryStateBase* m_state = nullptr;
<<<<<<< HEAD
    SequentialGetterBase* m_source_column =
        nullptr; // Column of values used in aggregate (act_FindAll, actReturnFirst, act_Sum, etc)
};

template <class ColType>
class IntegerNodeBase : public ColumnNodeBase {
    using ThisType = IntegerNodeBase<ColType>;

public:
    using TConditionValue = typename ColType::value_type;
    static const bool nullable = ColType::nullable;

    template <class TConditionFunction, Action TAction, DataType TDataType, bool Nullable>
    bool find_callback_specialization(size_t s, size_t end_in_leaf)
    {
        using AggregateColumnType = typename GetColumnType<TDataType, Nullable>::type;
        bool cont;
        size_t start_in_leaf = s - this->m_leaf_start;
        cont = this->m_leaf_ptr->template find<TConditionFunction, act_CallbackIdx>(
            m_value, start_in_leaf, end_in_leaf, this->m_leaf_start, nullptr,
            std::bind(std::mem_fn(&ThisType::template match_callback<TAction, AggregateColumnType>), this,
                      std::placeholders::_1));
        return cont;
    }

protected:
    using LeafType = typename ColType::LeafType;
    using LeafInfo = typename ColType::LeafInfo;

    size_t aggregate_local_impl(QueryStateBase* st, size_t start, size_t end, size_t local_limit,
                                SequentialGetterBase* source_column, int c)
    {
=======
    // Column of values used in aggregate (act_FindAll, actReturnFirst, act_Sum, etc)
    ArrayPayload* m_source_column = nullptr;
};

template <class LeafType>
class IntegerNodeBase : public ColumnNodeBase {
    using ThisType = IntegerNodeBase<LeafType>;

public:
    using TConditionValue = typename LeafType::value_type;
    // static const bool nullable = ColType::nullable;

    template <class TConditionFunction, Action TAction, DataType TDataType, bool Nullable>
    bool find_callback_specialization(size_t start_in_leaf, size_t end_in_leaf)
    {
        using AggregateLeafType = typename GetLeafType<TDataType, Nullable>::type;
        auto cb = std::bind(std::mem_fn(&ThisType::template match_callback<TAction, AggregateLeafType>), this,
                            std::placeholders::_1);
        return this->m_leaf_ptr->template find<TConditionFunction, act_CallbackIdx>(m_value, start_in_leaf,
                                                                                    end_in_leaf, 0, nullptr, cb);
    }

protected:
    size_t aggregate_local_impl(QueryStateBase* st, size_t start, size_t end, size_t local_limit,
                                ArrayPayload* source_column, int c)
    {
        m_table.check();
        REALM_ASSERT(m_cluster);
>>>>>>> origin/develop12
        REALM_ASSERT(m_children.size() > 0);
        m_local_matches = 0;
        m_local_limit = local_limit;
        m_last_local_match = start - 1;
        m_state = st;

        // If there are no other nodes than us (m_children.size() == 1) AND the column used for our condition is
        // the same as the column used for the aggregate action, then the entire query can run within scope of that
        // column only, with no references to other columns:
        bool fastmode = should_run_in_fastmode(source_column);
<<<<<<< HEAD
        for (size_t s = start; s < end;) {
            cache_leaf(s);

            size_t end_in_leaf;
            if (end > m_leaf_end)
                end_in_leaf = m_leaf_end - m_leaf_start;
            else
                end_in_leaf = end - m_leaf_start;

            if (fastmode) {
                bool cont;
                size_t start_in_leaf = s - m_leaf_start;
                cont = m_leaf_ptr->find(c, m_action, m_value, start_in_leaf, end_in_leaf, m_leaf_start,
                                        static_cast<QueryState<int64_t>*>(st));
                if (!cont)
                    return not_found;
            }
            // Else, for each match in this node, call our IntegerNodeBase::match_callback to test remaining nodes
            // and/or extract
            // aggregate payload from aggregate column:
            else {
                m_source_column = source_column;
                bool cont = (this->*m_find_callback_specialized)(s, end_in_leaf);
                if (!cont)
                    return not_found;
            }

            if (m_local_matches == m_local_limit)
                break;

            s = end_in_leaf + m_leaf_start;
=======
        if (fastmode) {
            bool cont;
            cont = m_leaf_ptr->find(c, m_action, m_value, start, end, 0, static_cast<QueryState<int64_t>*>(st));
            if (!cont)
                return not_found;
        }
        // Else, for each match in this node, call our IntegerNodeBase::match_callback to test remaining nodes
        // and/or extract
        // aggregate payload from aggregate column:
        else {
            m_source_column = source_column;
            bool cont = (this->*m_find_callback_specialized)(start, end);
            if (!cont)
                return not_found;
>>>>>>> origin/develop12
        }

        if (m_local_matches == m_local_limit) {
            m_dD = (m_last_local_match + 1 - start) / (m_local_matches + 1.0);
            return m_last_local_match + 1;
        }
        else {
            m_dD = (end - start) / (m_local_matches + 1.0);
            return end;
        }
    }

<<<<<<< HEAD
    IntegerNodeBase(TConditionValue value, size_t column_idx)
        : ColumnNodeBase(column_idx)
=======
    IntegerNodeBase(TConditionValue value, ColKey column_key)
        : ColumnNodeBase(column_key)
>>>>>>> origin/develop12
        , m_value(std::move(value))
    {
    }

<<<<<<< HEAD
    IntegerNodeBase(const ThisType& from, QueryNodeHandoverPatches* patches)
        : ColumnNodeBase(from, patches)
        , m_value(from.m_value)
        , m_condition_column(from.m_condition_column)
        , m_find_callback_specialized(from.m_find_callback_specialized)
    {
        if (m_condition_column && patches)
            m_condition_column_idx = m_condition_column->get_column_index();
    }

    void table_changed() override
    {
        m_condition_column = &get_column<ColType>(m_condition_column_idx);
    }

    void verify_column() const override
    {
        do_verify_column(m_condition_column);
    }

    void init() override
    {
        ColumnNodeBase::init();

        m_dT = _impl::CostHeuristic<ColType>::dT();
        m_dD = _impl::CostHeuristic<ColType>::dD();

        // Clear leaf cache
        m_leaf_end = 0;
        m_array_ptr.reset(); // Explicitly destroy the old one first, because we're reusing the memory.
        m_array_ptr.reset(new (&m_leaf_cache_storage) LeafType(m_table->get_alloc()));
    }

    void get_leaf(const ColType& col, size_t ndx)
    {
        size_t ndx_in_leaf;
        LeafInfo leaf_info{&m_leaf_ptr, m_array_ptr.get()};
        col.get_leaf(ndx, ndx_in_leaf, leaf_info);
        m_leaf_start = ndx - ndx_in_leaf;
        m_leaf_end = m_leaf_start + m_leaf_ptr->size();
    }

    void cache_leaf(size_t s)
    {
        if (s >= m_leaf_end || s < m_leaf_start) {
            get_leaf(*m_condition_column, s);
            size_t w = m_leaf_ptr->get_width();
            m_dT = (w == 0 ? 1.0 / REALM_MAX_BPNODE_SIZE : w / float(bitwidth_time_unit));
        }
    }

    bool should_run_in_fastmode(SequentialGetterBase* source_column) const
    {
        return (m_children.size() == 1 &&
                (source_column == nullptr ||
                 (!m_fastmode_disabled &&
                  static_cast<SequentialGetter<ColType>*>(source_column)->m_column == m_condition_column)));
=======
    IntegerNodeBase(const ThisType& from)
        : ColumnNodeBase(from)
        , m_value(from.m_value)
        , m_find_callback_specialized(from.m_find_callback_specialized)
    {
    }

    void cluster_changed() override
    {
        // Assigning nullptr will cause the Leaf destructor to be called. Must
        // be done before assigning a new one. Otherwise the destructor will be
        // called after the constructor is called and that is unfortunate if
        // the object has the same address. (As in this case)
        m_array_ptr = nullptr;
        // Create new Leaf
        m_array_ptr = LeafPtr(new (&m_leaf_cache_storage) LeafType(m_table.unchecked_ptr()->get_alloc()));
        m_cluster->init_leaf(this->m_condition_column_key, m_array_ptr.get());
        m_leaf_ptr = m_array_ptr.get();
    }

    void init(bool will_query_ranges) override
    {
        ColumnNodeBase::init(will_query_ranges);

        m_dT = _impl::CostHeuristic<LeafType>::dT();
        m_dD = _impl::CostHeuristic<LeafType>::dD();
    }

    bool should_run_in_fastmode(ArrayPayload* source_leaf) const
    {
        if (m_children.size() > 1 || m_fastmode_disabled)
            return false;
        if (source_leaf == nullptr)
            return true;
        // Compare leafs to see if they are the same
        auto leaf = dynamic_cast<LeafType*>(source_leaf);
        return leaf ? leaf->get_ref() == m_leaf_ptr->get_ref() : false;
>>>>>>> origin/develop12
    }

    // Search value:
    TConditionValue m_value;

<<<<<<< HEAD
    // Column on which search criteria are applied
    const ColType* m_condition_column = nullptr;

    // Leaf cache
    using LeafCacheStorage = typename std::aligned_storage<sizeof(LeafType), alignof(LeafType)>::type;
    LeafCacheStorage m_leaf_cache_storage;
    std::unique_ptr<LeafType, PlacementDelete> m_array_ptr;
    const LeafType* m_leaf_ptr = nullptr;
    size_t m_leaf_start = npos;
    size_t m_leaf_end = 0;
    size_t m_local_end;
=======
    // Leaf cache
    using LeafCacheStorage = typename std::aligned_storage<sizeof(LeafType), alignof(LeafType)>::type;
    using LeafPtr = std::unique_ptr<LeafType, PlacementDelete>;
    LeafCacheStorage m_leaf_cache_storage;
    LeafPtr m_array_ptr;
    const LeafType* m_leaf_ptr = nullptr;
>>>>>>> origin/develop12

    // Aggregate optimization
    using TFind_callback_specialized = bool (ThisType::*)(size_t, size_t);
    TFind_callback_specialized m_find_callback_specialized = nullptr;
<<<<<<< HEAD
};


template <class ColType, class TConditionFunction>
class IntegerNode : public IntegerNodeBase<ColType> {
    using BaseType = IntegerNodeBase<ColType>;
    using ThisType = IntegerNode<ColType, TConditionFunction>;

public:
    static const bool special_null_node = false;
    using TConditionValue = typename BaseType::TConditionValue;

    IntegerNode(TConditionValue value, size_t column_ndx)
        : BaseType(value, column_ndx)
    {
    }
    IntegerNode(const IntegerNode& from, QueryNodeHandoverPatches* patches)
        : BaseType(from, patches)
    {
    }

    void aggregate_local_prepare(Action action, DataType col_id, bool is_nullable) override
    {
        this->m_fastmode_disabled = (col_id == type_Float || col_id == type_Double);
        this->m_action = action;
        this->m_find_callback_specialized = get_specialized_callback(action, col_id, is_nullable);
    }

    size_t aggregate_local(QueryStateBase* st, size_t start, size_t end, size_t local_limit,
                           SequentialGetterBase* source_column) override
    {
        constexpr int cond = TConditionFunction::condition;
        return this->aggregate_local_impl(st, start, end, local_limit, source_column, cond);
    }

    size_t find_first_local(size_t start, size_t end) override
    {
        REALM_ASSERT(this->m_table);

        while (start < end) {

            // Cache internal leaves
            if (start >= this->m_leaf_end || start < this->m_leaf_start) {
                this->get_leaf(*this->m_condition_column, start);
            }

            // FIXME: Create a fast bypass when you just need to check 1 row, which is used alot from within core.
            // It should just call array::get and save the initial overhead of find_first() which has become quite
            // big. Do this when we have cleaned up core a bit more.

            size_t end2;
            if (end > this->m_leaf_end)
                end2 = this->m_leaf_end - this->m_leaf_start;
            else
                end2 = end - this->m_leaf_start;

            size_t s;
            s = this->m_leaf_ptr->template find_first<TConditionFunction>(this->m_value, start - this->m_leaf_start,
                                                                          end2);

            if (s == not_found) {
                start = this->m_leaf_end;
                continue;
            }
            else
                return s + this->m_leaf_start;
        }

        return not_found;
    }

    std::string describe(util::serializer::SerialisationState& state) const override
    {
        return state.describe_column(ParentNode::m_table, IntegerNodeBase<ColType>::m_condition_column->get_column_index())
            + " " + describe_condition() + " " + util::serializer::print_value(IntegerNodeBase<ColType>::m_value);
    }

    std::string describe_condition() const override
    {
        return TConditionFunction::description();
    }

    std::unique_ptr<ParentNode> clone(QueryNodeHandoverPatches* patches) const override
    {
        return std::unique_ptr<ParentNode>(new IntegerNode<ColType, TConditionFunction>(*this, patches));
    }

protected:
    using TFind_callback_specialized = typename BaseType::TFind_callback_specialized;

=======

    template <class TConditionFunction>
>>>>>>> origin/develop12
    static TFind_callback_specialized get_specialized_callback(Action action, DataType col_id, bool is_nullable)
    {
        switch (action) {
            case act_Count:
<<<<<<< HEAD
                return get_specialized_callback_2_int<act_Count>(col_id, is_nullable);
            case act_Sum:
                return get_specialized_callback_2<act_Sum>(col_id, is_nullable);
            case act_Max:
                return get_specialized_callback_2<act_Max>(col_id, is_nullable);
            case act_Min:
                return get_specialized_callback_2<act_Min>(col_id, is_nullable);
            case act_FindAll:
                return get_specialized_callback_2_int<act_FindAll>(col_id, is_nullable);
            case act_CallbackIdx:
                return get_specialized_callback_2_int<act_CallbackIdx>(col_id, is_nullable);
=======
                return get_specialized_callback_2_int<act_Count, TConditionFunction>(col_id, is_nullable);
            case act_Sum:
                return get_specialized_callback_2<act_Sum, TConditionFunction>(col_id, is_nullable);
            case act_Max:
                return get_specialized_callback_2<act_Max, TConditionFunction>(col_id, is_nullable);
            case act_Min:
                return get_specialized_callback_2<act_Min, TConditionFunction>(col_id, is_nullable);
            case act_FindAll:
                return get_specialized_callback_2_int<act_FindAll, TConditionFunction>(col_id, is_nullable);
            case act_CallbackIdx:
                return get_specialized_callback_2_int<act_CallbackIdx, TConditionFunction>(col_id, is_nullable);
>>>>>>> origin/develop12
            default:
                break;
        }
        REALM_ASSERT(false); // Invalid aggregate function
        return nullptr;
    }

<<<<<<< HEAD
    template <Action TAction>
=======
    template <Action TAction, class TConditionFunction>
>>>>>>> origin/develop12
    static TFind_callback_specialized get_specialized_callback_2(DataType col_id, bool is_nullable)
    {
        switch (col_id) {
            case type_Int:
<<<<<<< HEAD
                return get_specialized_callback_3<TAction, type_Int>(is_nullable);
            case type_Float:
                return get_specialized_callback_3<TAction, type_Float>(is_nullable);
            case type_Double:
                return get_specialized_callback_3<TAction, type_Double>(is_nullable);
=======
                return get_specialized_callback_3<TAction, type_Int, TConditionFunction>(is_nullable);
            case type_Float:
                return get_specialized_callback_3<TAction, type_Float, TConditionFunction>(is_nullable);
            case type_Double:
                return get_specialized_callback_3<TAction, type_Double, TConditionFunction>(is_nullable);
            case type_Timestamp:
                return get_specialized_callback_3<TAction, type_Timestamp, TConditionFunction>(is_nullable);
>>>>>>> origin/develop12
            default:
                break;
        }
        REALM_ASSERT(false); // Invalid aggregate source column
        return nullptr;
    }

<<<<<<< HEAD
    template <Action TAction>
    static TFind_callback_specialized get_specialized_callback_2_int(DataType col_id, bool is_nullable)
    {
        if (col_id == type_Int) {
            return get_specialized_callback_3<TAction, type_Int>(is_nullable);
=======
    template <Action TAction, class TConditionFunction>
    static TFind_callback_specialized get_specialized_callback_2_int(DataType col_id, bool is_nullable)
    {
        if (col_id == type_Int) {
            return get_specialized_callback_3<TAction, type_Int, TConditionFunction>(is_nullable);
>>>>>>> origin/develop12
        }
        REALM_ASSERT(false); // Invalid aggregate source column
        return nullptr;
    }

<<<<<<< HEAD
    template <Action TAction, DataType TDataType>
    static TFind_callback_specialized get_specialized_callback_3(bool is_nullable)
    {
        if (is_nullable) {
            return &BaseType::template find_callback_specialization<TConditionFunction, TAction, TDataType, true>;
        }
        else {
            return &BaseType::template find_callback_specialization<TConditionFunction, TAction, TDataType, false>;
=======
    template <Action TAction, DataType TDataType, class TConditionFunction>
    static TFind_callback_specialized get_specialized_callback_3(bool is_nullable)
    {
        if (is_nullable) {
            return &IntegerNodeBase<LeafType>::template find_callback_specialization<TConditionFunction, TAction,
                                                                                     TDataType, true>;
        }
        else {
            return &IntegerNodeBase<LeafType>::template find_callback_specialization<TConditionFunction, TAction,
                                                                                     TDataType, false>;
>>>>>>> origin/develop12
        }
    }
};

<<<<<<< HEAD
template <class ColType>
class IntegerNode<ColType, Equal> : public IntegerNodeBase<ColType> {
public:
    using BaseType = IntegerNodeBase<ColType>;
    using TConditionValue = typename BaseType::TConditionValue;

    IntegerNode(TConditionValue value, size_t column_ndx)
        : BaseType(value, column_ndx)
    {
    }
    ~IntegerNode()
    {
        if (m_result) {
            m_result->destroy();
        }
    }

    void init() override
    {
        BaseType::init();
        m_nb_needles = m_needles.size();

        if (has_search_index()) {
            if (m_result) {
                m_result->clear();
            }
            else {
                ref_type ref = IntegerColumn::create(Allocator::get_default());
                m_result = std::make_unique<IntegerColumn>();
                m_result->init_from_ref(Allocator::get_default(), ref);
            }

            IntegerNodeBase<ColType>::m_condition_column->find_all(*m_result, this->m_value, 0, realm::npos);
            m_index_get = 0;
            m_index_end = m_result->size();
            IntegerNodeBase<ColType>::m_dT = 0;
        }
    }

    void consume_condition(IntegerNode<ColType, Equal>* other)
    {
        REALM_ASSERT(this->m_condition_column == other->m_condition_column);
        REALM_ASSERT(other->m_needles.empty());
        if (m_needles.empty()) {
            m_needles.insert(this->m_value);
        }
        m_needles.insert(other->m_value);
    }

    bool has_search_index() const
    {
        return IntegerNodeBase<ColType>::m_condition_column->has_search_index();
    }

    size_t find_first_local(size_t start, size_t end) override
    {
        REALM_ASSERT(this->m_table);

        if (has_search_index()) {
            if (m_index_end == 0)
                return not_found;

            if (start <= m_index_last_start)
                m_index_get = 0;
            else
                m_index_last_start = start;

            REALM_ASSERT(m_result);
            while (m_index_get < m_index_end) {
                // m_results are stored in sorted ascending order, guaranteed by the string index
                size_t ndx = size_t(m_result->get(m_index_get));
                if (ndx >= end) {
                    break;
                }
                m_index_get++;
                if (ndx >= start) {
                    return ndx;
                }
            }
            return not_found;
        }


        while (start < end) {
            // Cache internal leaves
            this->cache_leaf(start);

            size_t end2;
            if (end > this->m_leaf_end)
                end2 = this->m_leaf_end - this->m_leaf_start;
            else
                end2 = end - this->m_leaf_start;

            auto start2 = start - this->m_leaf_start;
            size_t s = realm::npos;
            if (m_nb_needles) {
                s = find_first_haystack(start2, end2);
            }
            else if (end2 - start2 == 1) {
                if (this->m_leaf_ptr->get(start2) == this->m_value) {
                    s = start2;
                }
            }
            else {
                s = this->m_leaf_ptr->template find_first<Equal>(this->m_value, start2, end2);
            }

            if (s == not_found) {
                start = this->m_leaf_end;
                continue;
            }
            else
                return s + this->m_leaf_start;
        }

        return not_found;
=======

template <class LeafType, class TConditionFunction>
class IntegerNode : public IntegerNodeBase<LeafType> {
    using BaseType = IntegerNodeBase<LeafType>;
    using ThisType = IntegerNode<LeafType, TConditionFunction>;

public:
    static const bool special_null_node = false;
    using TConditionValue = typename BaseType::TConditionValue;

    IntegerNode(TConditionValue value, ColKey column_key)
        : BaseType(value, column_key)
    {
    }
    IntegerNode(const IntegerNode& from)
        : BaseType(from)
    {
    }

    void aggregate_local_prepare(Action action, DataType col_id, bool is_nullable) override
    {
        this->m_fastmode_disabled = (col_id == type_Float || col_id == type_Double);
        this->m_action = action;
        this->m_find_callback_specialized =
            IntegerNodeBase<LeafType>::template get_specialized_callback<TConditionFunction>(action, col_id,
                                                                                             is_nullable);
    }

    size_t aggregate_local(QueryStateBase* st, size_t start, size_t end, size_t local_limit,
                           ArrayPayload* source_column) override
    {
        constexpr int cond = TConditionFunction::condition;
        return this->aggregate_local_impl(st, start, end, local_limit, source_column, cond);
    }

    size_t find_first_local(size_t start, size_t end) override
    {
        return this->m_leaf_ptr->template find_first<TConditionFunction>(this->m_value, start, end);
    }

    std::string describe(util::serializer::SerialisationState& state) const override
    {
        return state.describe_column(ParentNode::m_table, ColumnNodeBase::m_condition_column_key) + " " +
               describe_condition() + " " + util::serializer::print_value(this->m_value);
    }

    std::string describe_condition() const override
    {
        return TConditionFunction::description();
    }

    std::unique_ptr<ParentNode> clone() const override
    {
        return std::unique_ptr<ParentNode>(new ThisType(*this));
    }
};

template <size_t linear_search_threshold, class LeafType, class NeedleContainer>
static size_t find_first_haystack(LeafType& leaf, NeedleContainer& needles, size_t start, size_t end)
{
    // for a small number of conditions, it is faster to do a linear search than to compute the hash
    // the exact thresholds were found experimentally
    if (needles.size() < linear_search_threshold) {
        for (size_t i = start; i < end; ++i) {
            auto element = leaf.get(i);
            if (std::find(needles.begin(), needles.end(), element) != needles.end())
                return i;
        }
    }
    else {
        for (size_t i = start; i < end; ++i) {
            auto element = leaf.get(i);
            if (needles.count(element))
                return i;
        }
    }
    return realm::npos;
}

template <class LeafType>
class IntegerNode<LeafType, Equal> : public IntegerNodeBase<LeafType> {
public:
    using BaseType = IntegerNodeBase<LeafType>;
    using TConditionValue = typename BaseType::TConditionValue;
    using ThisType = IntegerNode<LeafType, Equal>;

    IntegerNode(TConditionValue value, ColKey column_key)
        : BaseType(value, column_key)
    {
    }
    ~IntegerNode()
    {
    }

    void init(bool will_query_ranges) override
    {
        BaseType::init(will_query_ranges);
        m_nb_needles = m_needles.size();

        if (has_search_index()) {
            // _search_index_init();
            m_result.clear();
            auto index = ParentNode::m_table->get_search_index(ParentNode::m_condition_column_key);
            index->find_all(m_result, BaseType::m_value);
            m_result_get = 0;
            m_last_start_key = ObjKey();
            IntegerNodeBase<LeafType>::m_dT = 0;
        }
    }

    bool do_consume_condition(ParentNode& node) override
    {
        auto& other = static_cast<ThisType&>(node);
        REALM_ASSERT(this->m_condition_column_key == other.m_condition_column_key);
        REALM_ASSERT(other.m_needles.empty());
        if (m_needles.empty()) {
            m_needles.insert(this->m_value);
        }
        m_needles.insert(other.m_value);
        return true;
    }

    bool has_search_index() const override
    {
        return this->m_table->has_search_index(IntegerNodeBase<LeafType>::m_condition_column_key);
    }

    void index_based_aggregate(size_t limit, Evaluator evaluator) override
    {
        for (size_t t = 0; t < m_result.size() && limit > 0; ++t) {
            auto obj = this->m_table->get_object(m_result[t]);
            if (evaluator(obj)) {
                --limit;
            }
        }
    }

    void aggregate_local_prepare(Action action, DataType col_id, bool is_nullable) override
    {
        this->m_fastmode_disabled = (col_id == type_Float || col_id == type_Double);
        this->m_action = action;
        this->m_find_callback_specialized =
            IntegerNodeBase<LeafType>::template get_specialized_callback<Equal>(action, col_id, is_nullable);
    }

    size_t aggregate_local(QueryStateBase* st, size_t start, size_t end, size_t local_limit,
                           ArrayPayload* source_column) override
    {
        constexpr int cond = Equal::condition;
        return this->aggregate_local_impl(st, start, end, local_limit, source_column, cond);
    }
 
    size_t find_first_local(size_t start, size_t end) override
    {
        REALM_ASSERT(this->m_table);
        size_t s = realm::npos;

        if (start < end) {
            if (m_nb_needles) {
                s = find_first_haystack<22>(*this->m_leaf_ptr, m_needles, start, end);
            }
            else if (has_search_index()) {
                ObjKey first_key = BaseType::m_cluster->get_real_key(start);
                if (first_key < m_last_start_key) {
                    // We are not advancing through the clusters. We basically don't know where we are,
                    // so just start over from the beginning.
                    auto it = std::lower_bound(m_result.begin(), m_result.end(), first_key);
                    m_result_get = (it == m_result.end()) ? realm::npos : (it - m_result.begin());
                }
                m_last_start_key = first_key;

                if (m_result_get < m_result.size()) {
                    auto actual_key = m_result[m_result_get];
                    // skip through keys which are in "earlier" leafs than the one selected by start..end:
                    while (first_key > actual_key) {
                        m_result_get++;
                        if (m_result_get == m_result.size())
                            return not_found;
                        actual_key = m_result[m_result_get];
                    }

                    // if actual key is bigger than last key, it is not in this leaf
                    ObjKey last_key = BaseType::m_cluster->get_real_key(end - 1);
                    if (actual_key > last_key)
                        return not_found;

                    // key is known to be in this leaf, so find key whithin leaf keys
                    return BaseType::m_cluster->lower_bound_key(
                        ObjKey(actual_key.value - BaseType::m_cluster->get_offset()));
                }
                return not_found;
            }
            else if (end - start == 1) {
                if (this->m_leaf_ptr->get(start) == this->m_value) {
                    s = start;
                }
            }
            else {
                s = this->m_leaf_ptr->template find_first<Equal>(this->m_value, start, end);
            }
        }

        return s;
>>>>>>> origin/develop12
    }

    std::string describe(util::serializer::SerialisationState& state) const override
    {
<<<<<<< HEAD
        REALM_ASSERT(this->m_condition_column != nullptr);
        std::string col_descr = state.describe_column(this->m_table, this->m_condition_column->get_column_index());

        if (m_needles.empty()) {
            return col_descr + " " + Equal::description() + " " +
                   util::serializer::print_value(IntegerNodeBase<ColType>::m_value);
=======
        REALM_ASSERT(this->m_condition_column_key);
        std::string col_descr = state.describe_column(this->m_table, this->m_condition_column_key);

        if (m_needles.empty()) {
            return col_descr + " " + Equal::description() + " " +
                   util::serializer::print_value(IntegerNodeBase<LeafType>::m_value);
>>>>>>> origin/develop12
        }

        // FIXME: once the parser supports it, print something like "column IN {n1, n2, n3}"
        std::string desc = "(";
        bool is_first = true;
        for (auto it : m_needles) {
            if (!is_first)
                desc += " or ";
            desc +=
                col_descr + " " + Equal::description() + " " + util::serializer::print_value(it); // "it" may be null
            is_first = false;
        }
        desc += ")";
        return desc;
    }
<<<<<<< HEAD
    std::unique_ptr<ParentNode> clone(QueryNodeHandoverPatches* patches) const override
    {
        return std::unique_ptr<ParentNode>(new IntegerNode<ColType, Equal>(*this, patches));
=======

    std::unique_ptr<ParentNode> clone() const override
    {
        return std::unique_ptr<ParentNode>(new ThisType(*this));
>>>>>>> origin/develop12
    }

private:
    std::unordered_set<TConditionValue> m_needles;
<<<<<<< HEAD
    std::unique_ptr<IntegerColumn> m_result;
    size_t m_nb_needles = 0;
    size_t m_index_get = 0;
    size_t m_index_last_start = 0;
    size_t m_index_end = 0;

    IntegerNode(const IntegerNode<ColType, Equal>& from, QueryNodeHandoverPatches* patches)
        : BaseType(from, patches)
        , m_needles(from.m_needles)
    {
    }
    size_t find_first_haystack(size_t start, size_t end)
    {
        const auto not_in_set = m_needles.end();
        // for a small number of conditions, it is faster to do a linear search than to compute the hash
        // the decision threshold was determined experimentally to be 22 conditions
        bool search = m_nb_needles < 22;
        auto cmp_fn = [this, search, not_in_set](const auto& v) {
            if (search) {
                for (auto it = m_needles.begin(); it != not_in_set; ++it) {
                    if (*it == v)
                        return true;
                }
                return false;
            }
            else {
                return (m_needles.find(v) != not_in_set);
            }
        };
        for (size_t i = start; i < end; ++i) {
            auto val = this->m_leaf_ptr->get(i);
            if (cmp_fn(val)) {
                return i;
            }
        }
        return realm::npos;
    }
=======
    std::vector<ObjKey> m_result;
    size_t m_nb_needles = 0;
    size_t m_result_get = 0;
    ObjKey m_last_start_key;

    IntegerNode(const IntegerNode<LeafType, Equal>& from)
        : BaseType(from)
        , m_needles(from.m_needles)
    {
    }
>>>>>>> origin/develop12
};


// This node is currently used for floats and doubles only
<<<<<<< HEAD
template <class ColType, class TConditionFunction>
class FloatDoubleNode : public ParentNode {
public:
    using TConditionValue = typename ColType::value_type;
    static const bool special_null_node = false;

    FloatDoubleNode(TConditionValue v, size_t column_ndx)
        : m_value(v)
    {
        m_condition_column_idx = column_ndx;
        m_dT = 1.0;
    }
    FloatDoubleNode(null, size_t column_ndx)
        : m_value(null::get_null_float<TConditionValue>())
    {
        m_condition_column_idx = column_ndx;
        m_dT = 1.0;
    }

    void table_changed() override
    {
        m_condition_column.init(&get_column<ColType>(m_condition_column_idx));
    }

    void verify_column() const override
    {
        do_verify_column(m_condition_column.m_column);
    }

    void init() override
    {
        ParentNode::init();
=======
template <class LeafType, class TConditionFunction>
class FloatDoubleNode : public ParentNode {
public:
    using TConditionValue = typename LeafType::value_type;
    static const bool special_null_node = false;

    FloatDoubleNode(TConditionValue v, ColKey column_key)
        : m_value(v)
    {
        m_condition_column_key = column_key;
        m_dT = 1.0;
    }
    FloatDoubleNode(null, ColKey column_key)
        : m_value(null::get_null_float<TConditionValue>())
    {
        m_condition_column_key = column_key;
        m_dT = 1.0;
    }

    void cluster_changed() override
    {
        // Assigning nullptr will cause the Leaf destructor to be called. Must
        // be done before assigning a new one. Otherwise the destructor will be
        // called after the constructor is called and that is unfortunate if
        // the object has the same address. (As in this case)
        m_array_ptr = nullptr;
        // Create new Leaf
        m_array_ptr = LeafPtr(new (&m_leaf_cache_storage) LeafType(m_table.unchecked_ptr()->get_alloc()));
        m_cluster->init_leaf(this->m_condition_column_key, m_array_ptr.get());
        m_leaf_ptr = m_array_ptr.get();
    }

    void init(bool will_query_ranges) override
    {
        ParentNode::init(will_query_ranges);
>>>>>>> origin/develop12
        m_dD = 100.0;
    }

    size_t find_first_local(size_t start, size_t end) override
    {
        TConditionFunction cond;

        auto find = [&](bool nullability) {
            bool m_value_nan = nullability ? null::is_null_float(m_value) : false;
            for (size_t s = start; s < end; ++s) {
<<<<<<< HEAD
                TConditionValue v = m_condition_column.get_next(s);
=======
                TConditionValue v = m_leaf_ptr->get(s);
>>>>>>> origin/develop12
                REALM_ASSERT(!(null::is_null_float(v) && !nullability));
                if (cond(v, m_value, nullability ? null::is_null_float<TConditionValue>(v) : false, m_value_nan))
                    return s;
            }
            return not_found;
        };

        // This will inline the second case but no the first. Todo, use templated lambda when switching to c++14
<<<<<<< HEAD
        if (m_table->is_nullable(m_condition_column_idx))
=======
        if (m_table->is_nullable(m_condition_column_key))
>>>>>>> origin/develop12
            return find(true);
        else
            return find(false);
    }

<<<<<<< HEAD
    std::string describe(util::serializer::SerialisationState& state) const override
    {
        REALM_ASSERT(m_condition_column.m_column != nullptr);
        return state.describe_column(ParentNode::m_table, m_condition_column.m_column->get_column_index())
            + " " + describe_condition() + " " + util::serializer::print_value(FloatDoubleNode::m_value);
    }
    std::string describe_condition() const override
    {
        return TConditionFunction::description();
    }

    std::unique_ptr<ParentNode> clone(QueryNodeHandoverPatches* patches) const override
    {
        return std::unique_ptr<ParentNode>(new FloatDoubleNode(*this, patches));
    }

    FloatDoubleNode(const FloatDoubleNode& from, QueryNodeHandoverPatches* patches)
        : ParentNode(from, patches)
        , m_value(from.m_value)
    {
        copy_getter(m_condition_column, m_condition_column_idx, from.m_condition_column, patches);
    }

protected:
    TConditionValue m_value;
    SequentialGetter<ColType> m_condition_column;
};

template <class ColType, class TConditionFunction>
class SizeNode : public ParentNode {
public:
    SizeNode(int64_t v, size_t column)
        : m_value(v)
    {
        m_condition_column_idx = column;
    }

    void table_changed() override
    {
        m_condition_column = &get_column<ColType>(m_condition_column_idx);
    }

    void verify_column() const override
    {
        do_verify_column(m_condition_column);
    }

    void init() override
    {
        ParentNode::init();
        m_dD = 10.0;
=======
    std::string describe(util::serializer::SerialisationState& state) const override
    {
        REALM_ASSERT(m_condition_column_key);
        return state.describe_column(ParentNode::m_table, m_condition_column_key) + " " + describe_condition() + " " +
               util::serializer::print_value(FloatDoubleNode::m_value);
    }
    std::string describe_condition() const override
    {
        return TConditionFunction::description();
    }

    std::unique_ptr<ParentNode> clone() const override
    {
        return std::unique_ptr<ParentNode>(new FloatDoubleNode(*this));
    }

    FloatDoubleNode(const FloatDoubleNode& from)
        : ParentNode(from)
        , m_value(from.m_value)
    {
    }

protected:
    TConditionValue m_value;
    // Leaf cache
    using LeafCacheStorage = typename std::aligned_storage<sizeof(LeafType), alignof(LeafType)>::type;
    using LeafPtr = std::unique_ptr<LeafType, PlacementDelete>;
    LeafCacheStorage m_leaf_cache_storage;
    LeafPtr m_array_ptr;
    const LeafType* m_leaf_ptr = nullptr;
};

template <class T, class TConditionFunction>
class SizeNode : public ParentNode {
public:
    SizeNode(int64_t v, ColKey column)
        : m_value(v)
    {
        m_condition_column_key = column;
    }

    void cluster_changed() override
    {
        // Assigning nullptr will cause the Leaf destructor to be called. Must
        // be done before assigning a new one. Otherwise the destructor will be
        // called after the constructor is called and that is unfortunate if
        // the object has the same address. (As in this case)
        m_array_ptr = nullptr;
        m_array_ptr = LeafPtr(new (&m_leaf_cache_storage) LeafType(m_table.unchecked_ptr()->get_alloc()));
        m_cluster->init_leaf(this->m_condition_column_key, m_array_ptr.get());
        m_leaf_ptr = m_array_ptr.get();
    }

    void init(bool will_query_ranges) override
    {
        ParentNode::init(will_query_ranges);
        m_dD = 10.0;
    }

    size_t find_first_local(size_t start, size_t end) override
    {
        for (size_t s = start; s < end; ++s) {
            T v = m_leaf_ptr->get(s);
            if (v) {
                int64_t sz = v.size();
                if (TConditionFunction()(sz, m_value))
                    return s;
            }
        }
        return not_found;
    }

    std::unique_ptr<ParentNode> clone() const override
    {
        return std::unique_ptr<ParentNode>(new SizeNode(*this));
    }

    SizeNode(const SizeNode& from)
        : ParentNode(from)
        , m_value(from.m_value)
    {
    }

private:
    // Leaf cache
    using LeafType = typename ColumnTypeTraits<T>::cluster_leaf_type;
    using LeafCacheStorage = typename std::aligned_storage<sizeof(LeafType), alignof(LeafType)>::type;
    using LeafPtr = std::unique_ptr<LeafType, PlacementDelete>;
    LeafCacheStorage m_leaf_cache_storage;
    LeafPtr m_array_ptr;
    const LeafType* m_leaf_ptr = nullptr;

    int64_t m_value;
};


template <class T, class TConditionFunction>
class SizeListNode : public ParentNode {
public:
    SizeListNode(int64_t v, ColKey column)
        : m_value(v)
    {
        m_condition_column_key = column;
    }

    void cluster_changed() override
    {
        // Assigning nullptr will cause the Leaf destructor to be called. Must
        // be done before assigning a new one. Otherwise the destructor will be
        // called after the constructor is called and that is unfortunate if
        // the object has the same address. (As in this case)
        m_array_ptr = nullptr;
        m_array_ptr = LeafPtr(new (&m_leaf_cache_storage) ArrayList(m_table.unchecked_ptr()->get_alloc()));
        m_cluster->init_leaf(this->m_condition_column_key, m_array_ptr.get());
        m_leaf_ptr = m_array_ptr.get();
    }

    void init(bool will_query_ranges) override
    {
        ParentNode::init(will_query_ranges);
        m_dD = 50.0;
>>>>>>> origin/develop12
    }

    size_t find_first_local(size_t start, size_t end) override
    {
        for (size_t s = start; s < end; ++s) {
<<<<<<< HEAD
            TConditionValue v = m_condition_column->get(s);
            if (v) {
                int64_t sz = m_size_operator(v);
=======
            ref_type ref = m_leaf_ptr->get(s);
            if (ref) {
                ListType list(m_table.unchecked_ptr()->get_alloc());
                list.init_from_ref(ref);
                int64_t sz = list.size();
>>>>>>> origin/develop12
                if (TConditionFunction()(sz, m_value))
                    return s;
            }
        }
        return not_found;
    }

<<<<<<< HEAD
    std::unique_ptr<ParentNode> clone(QueryNodeHandoverPatches* patches) const override
    {
        return std::unique_ptr<ParentNode>(new SizeNode(*this, patches));
    }

    SizeNode(const SizeNode& from, QueryNodeHandoverPatches* patches)
        : ParentNode(from, patches)
        , m_value(from.m_value)
        , m_condition_column(from.m_condition_column)
    {
        if (m_condition_column && patches)
            m_condition_column_idx = m_condition_column->get_column_index();
    }

private:
    using TConditionValue = typename ColType::value_type;

    int64_t m_value;
    const ColType* m_condition_column = nullptr;
    Size<TConditionValue> m_size_operator;
=======
    std::unique_ptr<ParentNode> clone() const override
    {
        return std::unique_ptr<ParentNode>(new SizeListNode(*this));
    }

    SizeListNode(const SizeListNode& from)
        : ParentNode(from)
        , m_value(from.m_value)
    {
    }

private:
    // Leaf cache
    using ListType = BPlusTree<T>;
    using LeafCacheStorage = typename std::aligned_storage<sizeof(ArrayList), alignof(ArrayList)>::type;
    using LeafPtr = std::unique_ptr<ArrayList, PlacementDelete>;
    LeafCacheStorage m_leaf_cache_storage;
    LeafPtr m_array_ptr;
    const ArrayList* m_leaf_ptr = nullptr;

    int64_t m_value;
>>>>>>> origin/develop12
};


template <class TConditionFunction>
class BinaryNode : public ParentNode {
public:
    using TConditionValue = BinaryData;
    static const bool special_null_node = false;

<<<<<<< HEAD
    BinaryNode(BinaryData v, size_t column)
        : m_value(v)
    {
        m_dT = 100.0;
        m_condition_column_idx = column;
    }

    BinaryNode(null, size_t column)
=======
    BinaryNode(BinaryData v, ColKey column)
        : m_value(v)
    {
        m_dT = 100.0;
        m_condition_column_key = column;
    }

    BinaryNode(null, ColKey column)
>>>>>>> origin/develop12
        : BinaryNode(BinaryData{}, column)
    {
    }

<<<<<<< HEAD
    void table_changed() override
    {
        m_condition_column = &get_column<BinaryColumn>(m_condition_column_idx);
    }

    void verify_column() const override
    {
        do_verify_column(m_condition_column);
    }

    void init() override
    {
        ParentNode::init();
=======
    void cluster_changed() override
    {
        m_array_ptr = nullptr;
        m_array_ptr = LeafPtr(new (&m_leaf_cache_storage) ArrayBinary(m_table.unchecked_ptr()->get_alloc()));
        m_cluster->init_leaf(this->m_condition_column_key, m_array_ptr.get());
        m_leaf_ptr = m_array_ptr.get();
    }

    void init(bool will_query_ranges) override
    {
        ParentNode::init(will_query_ranges);
>>>>>>> origin/develop12

        m_dD = 100.0;
    }

    size_t find_first_local(size_t start, size_t end) override
    {
        TConditionFunction condition;
        for (size_t s = start; s < end; ++s) {
<<<<<<< HEAD
            BinaryData value = m_condition_column->get(s);
=======
            BinaryData value = m_leaf_ptr->get(s);
>>>>>>> origin/develop12
            if (condition(m_value.get(), value))
                return s;
        }
        return not_found;
    }

    virtual std::string describe(util::serializer::SerialisationState& state) const override
    {
<<<<<<< HEAD
        REALM_ASSERT(m_condition_column != nullptr);
        return state.describe_column(ParentNode::m_table, m_condition_column->get_column_index())
            + " " + TConditionFunction::description() + " "
            + util::serializer::print_value(BinaryNode::m_value.get());
    }

    std::unique_ptr<ParentNode> clone(QueryNodeHandoverPatches* patches) const override
    {
        return std::unique_ptr<ParentNode>(new BinaryNode(*this, patches));
    }

    BinaryNode(const BinaryNode& from, QueryNodeHandoverPatches* patches)
        : ParentNode(from, patches)
        , m_value(from.m_value)
        , m_condition_column(from.m_condition_column)
    {
        if (m_condition_column && patches)
            m_condition_column_idx = m_condition_column->get_column_index();
=======
        REALM_ASSERT(m_condition_column_key);
        return state.describe_column(ParentNode::m_table, m_condition_column_key) + " " +
               TConditionFunction::description() + " " + util::serializer::print_value(BinaryNode::m_value.get());
    }

    std::unique_ptr<ParentNode> clone() const override
    {
        return std::unique_ptr<ParentNode>(new BinaryNode(*this));
    }

    BinaryNode(const BinaryNode& from)
        : ParentNode(from)
        , m_value(from.m_value)
    {
>>>>>>> origin/develop12
    }

private:
    OwnedBinaryData m_value;
<<<<<<< HEAD
    const BinaryColumn* m_condition_column;
};


class TimestampNodeBase : public ParentNode {
public:
    using TConditionValue = Timestamp;
    static const bool special_null_node = false;
    using LeafTypeSeconds = typename IntNullColumn::LeafType;
    using LeafInfoSeconds = typename IntNullColumn::LeafInfo;
    using LeafTypeNanos = typename IntegerColumn::LeafType;
    using LeafInfoNanos = typename IntegerColumn::LeafInfo;


    TimestampNodeBase(Timestamp v, size_t column)
        : m_value(v)
        , m_needle_seconds(m_value.is_null() ? util::none : util::make_optional(m_value.get_seconds()))
    {
        m_condition_column_idx = column;
    }

    TimestampNodeBase(null, size_t column)
        : TimestampNodeBase(Timestamp{}, column)
    {
    }

    void table_changed() override
    {
        m_condition_column = &get_column<TimestampColumn>(m_condition_column_idx);
    }

    void verify_column() const override
    {
        do_verify_column(m_condition_column);
    }

    void init() override
    {
        ParentNode::init();

        m_dD = 100.0;

        // Clear leaf cache
        m_leaf_end_seconds = 0;
        m_array_ptr_seconds.reset(); // Explicitly destroy the old one first, because we're reusing the memory.
        m_array_ptr_seconds.reset(new (&m_leaf_cache_storage_seconds) LeafTypeSeconds(m_table->get_alloc()));
        m_leaf_end_nanos = 0;
        m_array_ptr_nanos.reset(); // Explicitly destroy the old one first, because we're reusing the memory.
        m_array_ptr_nanos.reset(new (&m_leaf_cache_storage_nanos) LeafTypeNanos(m_table->get_alloc()));
        m_condition_column_is_nullable = m_condition_column->is_nullable();
    }

protected:
    void get_leaf_seconds(const TimestampColumn& col, size_t ndx)
    {
        size_t ndx_in_leaf;
        LeafInfoSeconds leaf_info_seconds{&m_leaf_ptr_seconds, m_array_ptr_seconds.get()};
        col.get_seconds_leaf(ndx, ndx_in_leaf, leaf_info_seconds);
        m_leaf_start_seconds = ndx - ndx_in_leaf;
        m_leaf_end_seconds = m_leaf_start_seconds + m_leaf_ptr_seconds->size();
    }

    void get_leaf_nanos(const TimestampColumn& col, size_t ndx)
    {
        size_t ndx_in_leaf;
        LeafInfoNanos leaf_info_nanos{&m_leaf_ptr_nanos, m_array_ptr_nanos.get()};
        col.get_nanoseconds_leaf(ndx, ndx_in_leaf, leaf_info_nanos);
        m_leaf_start_nanos = ndx - ndx_in_leaf;
        m_leaf_end_nanos = m_leaf_start_nanos + m_leaf_ptr_nanos->size();
    }

    util::Optional<int64_t> get_seconds_and_cache(size_t ndx)
    {
        // Cache internal leaves
        if (ndx >= this->m_leaf_end_seconds || ndx < this->m_leaf_start_seconds) {
            this->get_leaf_seconds(*this->m_condition_column, ndx);
        }
        const size_t ndx_in_leaf = ndx - m_leaf_start_seconds;
        return this->m_leaf_ptr_seconds->get(ndx_in_leaf);
    }

    int32_t get_nanoseconds_and_cache(size_t ndx)
    {
        // Cache internal leaves
        if (ndx >= this->m_leaf_end_nanos || ndx < this->m_leaf_start_nanos) {
            this->get_leaf_nanos(*this->m_condition_column, ndx);
        }
        return int32_t(this->m_leaf_ptr_nanos->get(ndx - this->m_leaf_start_nanos));
    }

    TimestampNodeBase(const TimestampNodeBase& from, QueryNodeHandoverPatches* patches)
        : ParentNode(from, patches)
        , m_value(from.m_value)
        , m_needle_seconds(from.m_needle_seconds)
        , m_condition_column(from.m_condition_column)
        , m_condition_column_is_nullable(from.m_condition_column_is_nullable)
    {
        if (m_condition_column && patches)
            m_condition_column_idx = m_condition_column->get_column_index();
    }

    Timestamp m_value;
    util::Optional<int64_t> m_needle_seconds;
    const TimestampColumn* m_condition_column;
    bool m_condition_column_is_nullable = false;

    // Leaf cache seconds
    using LeafCacheStorageSeconds =
        typename std::aligned_storage<sizeof(LeafTypeSeconds), alignof(LeafTypeSeconds)>::type;
    LeafCacheStorageSeconds m_leaf_cache_storage_seconds;
    std::unique_ptr<LeafTypeSeconds, PlacementDelete> m_array_ptr_seconds;
    const LeafTypeSeconds* m_leaf_ptr_seconds = nullptr;
    size_t m_leaf_start_seconds = npos;
    size_t m_leaf_end_seconds = 0;

    // Leaf cache nanoseconds
    using LeafCacheStorageNanos = typename std::aligned_storage<sizeof(LeafTypeNanos), alignof(LeafTypeNanos)>::type;
    LeafCacheStorageNanos m_leaf_cache_storage_nanos;
    std::unique_ptr<LeafTypeNanos, PlacementDelete> m_array_ptr_nanos;
    const LeafTypeNanos* m_leaf_ptr_nanos = nullptr;
    size_t m_leaf_start_nanos = npos;
    size_t m_leaf_end_nanos = 0;
=======
    using LeafCacheStorage = typename std::aligned_storage<sizeof(ArrayBinary), alignof(ArrayBinary)>::type;
    using LeafPtr = std::unique_ptr<ArrayBinary, PlacementDelete>;
    LeafCacheStorage m_leaf_cache_storage;
    LeafPtr m_array_ptr;
    const ArrayBinary* m_leaf_ptr = nullptr;
};

template <class TConditionFunction>
class BoolNode : public ParentNode {
public:
    using TConditionValue = bool;

    BoolNode(util::Optional<bool> v, ColKey column)
        : m_value(v)
    {
        m_condition_column_key = column;
    }

    BoolNode(const BoolNode& from)
        : ParentNode(from)
        , m_value(from.m_value)
    {
    }

    void cluster_changed() override
    {
        m_array_ptr = nullptr;
        m_array_ptr = LeafPtr(new (&m_leaf_cache_storage) ArrayBoolNull(m_table.unchecked_ptr()->get_alloc()));
        m_cluster->init_leaf(this->m_condition_column_key, m_array_ptr.get());
        m_leaf_ptr = m_array_ptr.get();
    }

    void init(bool will_query_ranges) override
    {
        ParentNode::init(will_query_ranges);

        m_dD = 100.0;
    }

    size_t find_first_local(size_t start, size_t end) override
    {
        TConditionFunction condition;
        bool m_value_is_null = !m_value;
        for (size_t s = start; s < end; ++s) {
            util::Optional<bool> value = m_leaf_ptr->get(s);
            if (condition(value, m_value, !value, m_value_is_null))
                return s;
        }
        return not_found;
    }

    virtual std::string describe(util::serializer::SerialisationState& state) const override
    {
        return state.describe_column(ParentNode::m_table, m_condition_column_key) + " " +
               TConditionFunction::description() + " " + util::serializer::print_value(m_value);
    }

    std::unique_ptr<ParentNode> clone() const override
    {
        return std::unique_ptr<ParentNode>(new BoolNode(*this));
    }

private:
    util::Optional<bool> m_value;
    using LeafCacheStorage = typename std::aligned_storage<sizeof(ArrayBoolNull), alignof(ArrayBoolNull)>::type;
    using LeafPtr = std::unique_ptr<ArrayBoolNull, PlacementDelete>;
    LeafCacheStorage m_leaf_cache_storage;
    LeafPtr m_array_ptr;
    const ArrayBoolNull* m_leaf_ptr = nullptr;
};

class TimestampNodeBase : public ParentNode {
public:
    using TConditionValue = Timestamp;
    static const bool special_null_node = false;

    TimestampNodeBase(Timestamp v, ColKey column)
        : m_value(v)
    {
        m_condition_column_key = column;
    }

    TimestampNodeBase(null, ColKey column)
        : TimestampNodeBase(Timestamp{}, column)
    {
    }

    void cluster_changed() override
    {
        m_array_ptr = nullptr;
        m_array_ptr = LeafPtr(new (&m_leaf_cache_storage) ArrayTimestamp(m_table.unchecked_ptr()->get_alloc()));
        m_cluster->init_leaf(this->m_condition_column_key, m_array_ptr.get());
        m_leaf_ptr = m_array_ptr.get();
    }

    void init(bool will_query_ranges) override
    {
        ParentNode::init(will_query_ranges);

        m_dD = 100.0;
    }

protected:
    TimestampNodeBase(const TimestampNodeBase& from)
        : ParentNode(from)
        , m_value(from.m_value)
    {
    }

    Timestamp m_value;
    using LeafCacheStorage = typename std::aligned_storage<sizeof(ArrayTimestamp), alignof(ArrayTimestamp)>::type;
    using LeafPtr = std::unique_ptr<ArrayTimestamp, PlacementDelete>;
    LeafCacheStorage m_leaf_cache_storage;
    LeafPtr m_array_ptr;
    const ArrayTimestamp* m_leaf_ptr = nullptr;
>>>>>>> origin/develop12
};

template <class TConditionFunction>
class TimestampNode : public TimestampNodeBase {
public:
    using TimestampNodeBase::TimestampNodeBase;

<<<<<<< HEAD
    template <class Condition>
    size_t find_first_local_seconds(size_t start, size_t end)
    {
        while (start < end) {
            // Cache internal leaves
            if (start >= this->m_leaf_end_seconds || start < this->m_leaf_start_seconds) {
                this->get_leaf_seconds(*this->m_condition_column, start);
            }

            size_t end2;
            if (end > this->m_leaf_end_seconds)
                end2 = this->m_leaf_end_seconds - this->m_leaf_start_seconds;
            else
                end2 = end - this->m_leaf_start_seconds;

            size_t s = this->m_leaf_ptr_seconds->template find_first<Condition>(
                m_needle_seconds, start - this->m_leaf_start_seconds, end2);

            if (s == not_found) {
                start = this->m_leaf_end_seconds;
                continue;
            }
            return s + this->m_leaf_start_seconds;
        }
        return not_found;
    }

    // see query_engine.cpp for operator specialisations
    size_t find_first_local(size_t start, size_t end) override
    {
        REALM_ASSERT(this->m_table);

        size_t ret = m_condition_column->find<TConditionFunction>(m_value, start, end);
        return ret;
    }

    virtual std::string describe(util::serializer::SerialisationState& state) const override
    {
        REALM_ASSERT(m_condition_column != nullptr);
        return state.describe_column(ParentNode::m_table, m_condition_column->get_column_index()) + " " +
               TConditionFunction::description() + " " + util::serializer::print_value(TimestampNode::m_value);
    }

    std::unique_ptr<ParentNode> clone(QueryNodeHandoverPatches* patches) const override
    {
        return std::unique_ptr<ParentNode>(new TimestampNode(*this, patches));
    }
};

template <>
size_t TimestampNode<Greater>::find_first_local(size_t start, size_t end);
template <>
size_t TimestampNode<Less>::find_first_local(size_t start, size_t end);
template <>
size_t TimestampNode<GreaterEqual>::find_first_local(size_t start, size_t end);
template <>
size_t TimestampNode<LessEqual>::find_first_local(size_t start, size_t end);
template <>
size_t TimestampNode<Equal>::find_first_local(size_t start, size_t end);
template <>
size_t TimestampNode<NotEqual>::find_first_local(size_t start, size_t end);
template <>
size_t TimestampNode<NotNull>::find_first_local(size_t start, size_t end);

=======
    size_t find_first_local(size_t start, size_t end) override
    {
        return m_leaf_ptr->find_first<TConditionFunction>(m_value, start, end);
    }

    std::string describe(util::serializer::SerialisationState& state) const override
    {
        REALM_ASSERT(m_condition_column_key);
        return state.describe_column(ParentNode::m_table, m_condition_column_key) + " " +
               TConditionFunction::description() + " " + util::serializer::print_value(TimestampNode::m_value);
    }

    std::unique_ptr<ParentNode> clone() const override
    {
        return std::unique_ptr<ParentNode>(new TimestampNode(*this));
    }
};

>>>>>>> origin/develop12
class StringNodeBase : public ParentNode {
public:
    using TConditionValue = StringData;
    static const bool special_null_node = true;

<<<<<<< HEAD
    StringNodeBase(StringData v, size_t column)
        : m_value(v.is_null() ? util::none : util::make_optional(std::string(v)))
    {
        m_condition_column_idx = column;
=======
    StringNodeBase(StringData v, ColKey column)
        : m_value(v.is_null() ? util::none : util::make_optional(std::string(v)))
    {
        m_condition_column_key = column;
>>>>>>> origin/develop12
    }

    void table_changed() override
    {
<<<<<<< HEAD
        m_condition_column = &get_column_base(m_condition_column_idx);
        m_column_type = get_real_column_type(m_condition_column_idx);
    }

    void verify_column() const override
    {
        do_verify_column(m_condition_column);
    }

    bool has_search_index() const
    {
        return m_condition_column->has_search_index();
    }

    void init() override
    {
        ParentNode::init();
=======
        m_is_string_enum = m_table.unchecked_ptr()->is_enumerated(m_condition_column_key);
    }

    void cluster_changed() override
    {
        // Assigning nullptr will cause the Leaf destructor to be called. Must
        // be done before assigning a new one. Otherwise the destructor will be
        // called after the constructor is called and that is unfortunate if
        // the object has the same address. (As in this case)
        m_array_ptr = nullptr;
        // Create new Leaf
        m_array_ptr = LeafPtr(new (&m_leaf_cache_storage) ArrayString(m_table.unchecked_ptr()->get_alloc()));
        m_cluster->init_leaf(this->m_condition_column_key, m_array_ptr.get());
        m_leaf_ptr = m_array_ptr.get();
    }

    void init(bool will_query_ranges) override
    {
        ParentNode::init(will_query_ranges);
>>>>>>> origin/develop12

        m_dT = 10.0;
        m_probes = 0;
        m_matches = 0;
        m_end_s = 0;
        m_leaf_start = 0;
        m_leaf_end = 0;
    }

<<<<<<< HEAD
    void clear_leaf_state()
    {
        m_leaf.reset(nullptr);
    }

    StringNodeBase(const StringNodeBase& from, QueryNodeHandoverPatches* patches)
        : ParentNode(from, patches)
        , m_value(from.m_value)
        , m_condition_column(from.m_condition_column)
        , m_column_type(from.m_column_type)
        , m_leaf_type(from.m_leaf_type)
    {
        if (m_condition_column && patches)
            m_condition_column_idx = m_condition_column->get_column_index();
=======
    virtual void clear_leaf_state()
    {
        m_array_ptr = nullptr;
    }

    StringNodeBase(const StringNodeBase& from)
        : ParentNode(from)
        , m_value(from.m_value)
        , m_is_string_enum(from.m_is_string_enum)
    {
>>>>>>> origin/develop12
    }

    virtual std::string describe(util::serializer::SerialisationState& state) const override
    {
<<<<<<< HEAD
        REALM_ASSERT(m_condition_column != nullptr);
=======
        REALM_ASSERT(m_condition_column_key);
>>>>>>> origin/develop12
        StringData sd;
        if (bool(StringNodeBase::m_value)) {
            sd = StringData(StringNodeBase::m_value.value());
        }
<<<<<<< HEAD
        return state.describe_column(ParentNode::m_table, m_condition_column->get_column_index())
            + " " + describe_condition() + " " + util::serializer::print_value(sd);
=======
        return state.describe_column(ParentNode::m_table, m_condition_column_key) + " " + describe_condition() + " " +
               util::serializer::print_value(sd);
>>>>>>> origin/develop12
    }

protected:
    util::Optional<std::string> m_value;

<<<<<<< HEAD
    const ColumnBase* m_condition_column = nullptr;
    ColumnType m_column_type;

    // Used for linear scan through short/long-string
    std::unique_ptr<const ArrayParent> m_leaf;
    StringColumn::LeafType m_leaf_type;
    size_t m_end_s = 0;
    size_t m_leaf_start = 0;
    size_t m_leaf_end = 0;
    
    inline StringData get_string(size_t s)
    {
        StringData t;
        
        if (m_column_type == col_type_StringEnum) {
            // enum
            t = static_cast<const StringEnumColumn*>(m_condition_column)->get(s);
        }
        else {
            // short or long
            const StringColumn* asc = static_cast<const StringColumn*>(m_condition_column);
            REALM_ASSERT_3(s, <, asc->size());
            if (s >= m_end_s || s < m_leaf_start) {
                // we exceeded current leaf's range
                clear_leaf_state();
                size_t ndx_in_leaf;
                m_leaf = asc->get_leaf(s, ndx_in_leaf, m_leaf_type);
                m_leaf_start = s - ndx_in_leaf;
                
                if (m_leaf_type == StringColumn::leaf_type_Small)
                    m_end_s = m_leaf_start + static_cast<const ArrayString&>(*m_leaf).size();
                else if (m_leaf_type == StringColumn::leaf_type_Medium)
                    m_end_s = m_leaf_start + static_cast<const ArrayStringLong&>(*m_leaf).size();
                else
                    m_end_s = m_leaf_start + static_cast<const ArrayBigBlobs&>(*m_leaf).size();
            }
            
            if (m_leaf_type == StringColumn::leaf_type_Small)
                t = static_cast<const ArrayString&>(*m_leaf).get(s - m_leaf_start);
            else if (m_leaf_type == StringColumn::leaf_type_Medium)
                t = static_cast<const ArrayStringLong&>(*m_leaf).get(s - m_leaf_start);
            else
                t = static_cast<const ArrayBigBlobs&>(*m_leaf).get_string(s - m_leaf_start);
        }
        return t;
=======
    using LeafCacheStorage = typename std::aligned_storage<sizeof(ArrayString), alignof(ArrayString)>::type;
    using LeafPtr = std::unique_ptr<ArrayString, PlacementDelete>;
    LeafCacheStorage m_leaf_cache_storage;
    LeafPtr m_array_ptr;
    const ArrayString* m_leaf_ptr = nullptr;

    bool m_is_string_enum = false;

    size_t m_end_s = 0;
    size_t m_leaf_start = 0;
    size_t m_leaf_end = 0;

    inline StringData get_string(size_t s)
    {
        return m_leaf_ptr->get(s);
>>>>>>> origin/develop12
    }
};

// Conditions for strings. Note that Equal is specialized later in this file!
template <class TConditionFunction>
class StringNode : public StringNodeBase {
public:
<<<<<<< HEAD
    StringNode(StringData v, size_t column)
=======
    StringNode(StringData v, ColKey column)
>>>>>>> origin/develop12
        : StringNodeBase(v, column)
    {
        auto upper = case_map(v, true);
        auto lower = case_map(v, false);
        if (!upper || !lower) {
            error_code = "Malformed UTF-8: " + std::string(v);
        }
        else {
            m_ucase = std::move(*upper);
            m_lcase = std::move(*lower);
        }
    }

<<<<<<< HEAD
    void init() override
=======
    void init(bool will_query_ranges) override
>>>>>>> origin/develop12
    {
        clear_leaf_state();

        m_dD = 100.0;

<<<<<<< HEAD
        StringNodeBase::init();
=======
        StringNodeBase::init(will_query_ranges);
>>>>>>> origin/develop12
    }

    size_t find_first_local(size_t start, size_t end) override
    {
        TConditionFunction cond;

        for (size_t s = start; s < end; ++s) {
            StringData t = get_string(s);
<<<<<<< HEAD
            
=======

>>>>>>> origin/develop12
            if (cond(StringData(m_value), m_ucase.c_str(), m_lcase.c_str(), t))
                return s;
        }
        return not_found;
    }

    virtual std::string describe_condition() const override
    {
        return TConditionFunction::description();
    }

<<<<<<< HEAD
    std::unique_ptr<ParentNode> clone(QueryNodeHandoverPatches* patches) const override
    {
        return std::unique_ptr<ParentNode>(new StringNode<TConditionFunction>(*this, patches));
    }

    StringNode(const StringNode& from, QueryNodeHandoverPatches* patches)
        : StringNodeBase(from, patches)
=======
    std::unique_ptr<ParentNode> clone() const override
    {
        return std::unique_ptr<ParentNode>(new StringNode<TConditionFunction>(*this));
    }

    StringNode(const StringNode& from)
        : StringNodeBase(from)
>>>>>>> origin/develop12
        , m_ucase(from.m_ucase)
        , m_lcase(from.m_lcase)
    {
    }

protected:
    std::string m_ucase;
    std::string m_lcase;
};

// Specialization for Contains condition on Strings - we specialize because we can utilize Boyer-Moore
template <>
class StringNode<Contains> : public StringNodeBase {
public:
<<<<<<< HEAD
    StringNode(StringData v, size_t column)
    : StringNodeBase(v, column), m_charmap()
    {
        if (v.size() == 0)
            return;
        
        // Build a dictionary of char-to-last distances in the search string
        // (zero indicates that the char is not in needle)
        size_t last_char_pos = v.size()-1;
        for (size_t i = 0; i < last_char_pos; ++i) {
            // we never jump longer increments than 255 chars, even if needle is longer (to fit in one byte)
            uint8_t jump = last_char_pos-i < 255 ? static_cast<uint8_t>(last_char_pos-i) : 255;
            
=======
    StringNode(StringData v, ColKey column)
        : StringNodeBase(v, column)
        , m_charmap()
    {
        if (v.size() == 0)
            return;

        // Build a dictionary of char-to-last distances in the search string
        // (zero indicates that the char is not in needle)
        size_t last_char_pos = v.size() - 1;
        for (size_t i = 0; i < last_char_pos; ++i) {
            // we never jump longer increments than 255 chars, even if needle is longer (to fit in one byte)
            uint8_t jump = last_char_pos - i < 255 ? static_cast<uint8_t>(last_char_pos - i) : 255;

>>>>>>> origin/develop12
            unsigned char c = v[i];
            m_charmap[c] = jump;
        }
    }
<<<<<<< HEAD
    
    void init() override
    {
        clear_leaf_state();
        
        m_dD = 100.0;
        
        StringNodeBase::init();
    }
    
    
    size_t find_first_local(size_t start, size_t end) override
    {
        Contains cond;
        
        for (size_t s = start; s < end; ++s) {
            StringData t = get_string(s);
            
=======

    void init(bool will_query_ranges) override
    {
        clear_leaf_state();

        m_dD = 100.0;

        StringNodeBase::init(will_query_ranges);
    }


    size_t find_first_local(size_t start, size_t end) override
    {
        Contains cond;

        for (size_t s = start; s < end; ++s) {
            StringData t = get_string(s);

>>>>>>> origin/develop12
            if (cond(StringData(m_value), m_charmap, t))
                return s;
        }
        return not_found;
    }

    virtual std::string describe_condition() const override
    {
        return Contains::description();
    }


<<<<<<< HEAD
    std::unique_ptr<ParentNode> clone(QueryNodeHandoverPatches* patches) const override
    {
        return std::unique_ptr<ParentNode>(new StringNode<Contains>(*this, patches));
    }
    
    StringNode(const StringNode& from, QueryNodeHandoverPatches* patches)
    : StringNodeBase(from, patches)
    , m_charmap(from.m_charmap)
    {
    }
    
=======
    std::unique_ptr<ParentNode> clone() const override
    {
        return std::unique_ptr<ParentNode>(new StringNode<Contains>(*this));
    }

    StringNode(const StringNode& from)
        : StringNodeBase(from)
        , m_charmap(from.m_charmap)
    {
    }

>>>>>>> origin/develop12
protected:
    std::array<uint8_t, 256> m_charmap;
};

// Specialization for ContainsIns condition on Strings - we specialize because we can utilize Boyer-Moore
template <>
class StringNode<ContainsIns> : public StringNodeBase {
public:
<<<<<<< HEAD
    StringNode(StringData v, size_t column)
    : StringNodeBase(v, column), m_charmap()
=======
    StringNode(StringData v, ColKey column)
        : StringNodeBase(v, column)
        , m_charmap()
>>>>>>> origin/develop12
    {
        auto upper = case_map(v, true);
        auto lower = case_map(v, false);
        if (!upper || !lower) {
            error_code = "Malformed UTF-8: " + std::string(v);
        }
        else {
            m_ucase = std::move(*upper);
            m_lcase = std::move(*lower);
        }

        if (v.size() == 0)
            return;

        // Build a dictionary of char-to-last distances in the search string
        // (zero indicates that the char is not in needle)
<<<<<<< HEAD
        size_t last_char_pos = m_ucase.size()-1;
        for (size_t i = 0; i < last_char_pos; ++i) {
            // we never jump longer increments than 255 chars, even if needle is longer (to fit in one byte)
            uint8_t jump = last_char_pos-i < 255 ? static_cast<uint8_t>(last_char_pos-i) : 255;
=======
        size_t last_char_pos = m_ucase.size() - 1;
        for (size_t i = 0; i < last_char_pos; ++i) {
            // we never jump longer increments than 255 chars, even if needle is longer (to fit in one byte)
            uint8_t jump = last_char_pos - i < 255 ? static_cast<uint8_t>(last_char_pos - i) : 255;
>>>>>>> origin/develop12

            unsigned char uc = m_ucase[i];
            unsigned char lc = m_lcase[i];
            m_charmap[uc] = jump;
            m_charmap[lc] = jump;
        }
<<<<<<< HEAD

    }

    void init() override
=======
    }

    void init(bool will_query_ranges) override
>>>>>>> origin/develop12
    {
        clear_leaf_state();

        m_dD = 100.0;

<<<<<<< HEAD
        StringNodeBase::init();
=======
        StringNodeBase::init(will_query_ranges);
>>>>>>> origin/develop12
    }


    size_t find_first_local(size_t start, size_t end) override
    {
        ContainsIns cond;

        for (size_t s = start; s < end; ++s) {
            StringData t = get_string(s);
            // The current behaviour is to return all results when querying for a null string.
            // See comment above Query_NextGen_StringConditions on why every string including "" contains null.
            if (!bool(m_value)) {
                return s;
            }
            if (cond(StringData(m_value), m_ucase.c_str(), m_lcase.c_str(), m_charmap, t))
                return s;
        }
        return not_found;
    }

    virtual std::string describe_condition() const override
    {
        return ContainsIns::description();
    }

<<<<<<< HEAD
    std::unique_ptr<ParentNode> clone(QueryNodeHandoverPatches* patches) const override
    {
        return std::unique_ptr<ParentNode>(new StringNode<ContainsIns>(*this, patches));
    }

    StringNode(const StringNode& from, QueryNodeHandoverPatches* patches)
    : StringNodeBase(from, patches)
    , m_charmap(from.m_charmap)
    , m_ucase(from.m_ucase)
    , m_lcase(from.m_lcase)
=======
    std::unique_ptr<ParentNode> clone() const override
    {
        return std::unique_ptr<ParentNode>(new StringNode<ContainsIns>(*this));
    }

    StringNode(const StringNode& from)
        : StringNodeBase(from)
        , m_charmap(from.m_charmap)
        , m_ucase(from.m_ucase)
        , m_lcase(from.m_lcase)
>>>>>>> origin/develop12
    {
    }

protected:
    std::array<uint8_t, 256> m_charmap;
    std::string m_ucase;
    std::string m_lcase;
};

class StringNodeEqualBase : public StringNodeBase {
public:
<<<<<<< HEAD
    StringNodeEqualBase(StringData v, size_t column)
        : StringNodeBase(v, column)
    {
    }
    StringNodeEqualBase(const StringNodeEqualBase& from, QueryNodeHandoverPatches* patches)
        : StringNodeBase(from, patches)
    {
    }
    ~StringNodeEqualBase() noexcept override
    {
        deallocate();
    }

    void deallocate() noexcept;
    void init() override;
=======
    StringNodeEqualBase(StringData v, ColKey column)
        : StringNodeBase(v, column)
    {
    }
    StringNodeEqualBase(const StringNodeEqualBase& from)
        : StringNodeBase(from)
        , m_has_search_index(from.m_has_search_index)
    {
    }

    void init(bool) override;

    bool has_search_index() const override
    {
        return m_has_search_index;
    }

    void cluster_changed() override
    {
        // If we use searchindex, we do not need further access to clusters
        if (!m_has_search_index) {
            StringNodeBase::cluster_changed();
        }
    }


>>>>>>> origin/develop12
    size_t find_first_local(size_t start, size_t end) override;

    virtual std::string describe_condition() const override
    {
        return Equal::description();
    }

protected:
<<<<<<< HEAD
=======
    ObjKey m_actual_key;
    ObjKey m_last_start_key;
    size_t m_results_start;
    size_t m_results_ndx;
    size_t m_results_end;
    bool m_has_search_index = false;

>>>>>>> origin/develop12
    inline BinaryData str_to_bin(const StringData& s) noexcept
    {
        return BinaryData(s.data(), s.size());
    }

<<<<<<< HEAD
    virtual void _search_index_init() = 0;
    virtual size_t _find_first_local(size_t start, size_t end) = 0;

    size_t m_key_ndx = not_found;
    size_t m_last_indexed;

    // Used for linear scan through enum-string
    SequentialGetter<StringEnumColumn> m_cse;

    // Used for index lookup
    std::unique_ptr<IntegerColumn> m_index_matches;
    bool m_index_matches_destroy = false;
    std::unique_ptr<SequentialGetter<IntegerColumn>> m_index_getter;
    size_t m_results_start;
    size_t m_results_end;
    size_t m_last_start;
=======
    virtual ObjKey get_key(size_t ndx) = 0;
    virtual void _search_index_init() = 0;
    virtual size_t _find_first_local(size_t start, size_t end) = 0;
>>>>>>> origin/develop12
};

// Specialization for Equal condition on Strings - we specialize because we can utilize indexes (if they exist) for
// Equal. This specialisation also supports combining other StringNode<Equal> conditions into itself in order to
// optimise the non-indexed linear search that can be happen when many conditions are OR'd together in an "IN" query.
// Future optimization: make specialization for greater, notequal, etc
template <>
class StringNode<Equal> : public StringNodeEqualBase {
public:
    using StringNodeEqualBase::StringNodeEqualBase;

<<<<<<< HEAD
    void _search_index_init() override;

    void consume_condition(StringNode<Equal>* other);

    std::unique_ptr<ParentNode> clone(QueryNodeHandoverPatches* patches) const override
    {
        return std::unique_ptr<ParentNode>(new StringNode<Equal>(*this, patches));
=======
    void table_changed() override
    {
        StringNodeBase::table_changed();
        m_has_search_index = m_table.unchecked_ptr()->has_search_index(m_condition_column_key) ||
                             m_table.unchecked_ptr()->get_primary_key_column() == m_condition_column_key;
    }

    void _search_index_init() override;

    bool do_consume_condition(ParentNode& other) override;

    std::unique_ptr<ParentNode> clone() const override
    {
        return std::unique_ptr<ParentNode>(new StringNode<Equal>(*this));
>>>>>>> origin/develop12
    }

    std::string describe(util::serializer::SerialisationState& state) const override;

<<<<<<< HEAD
    StringNode<Equal>(const StringNode& from, QueryNodeHandoverPatches* patches)
    : StringNodeEqualBase(from, patches)
    {
        for (auto it = from.m_needles.begin(); it != from.m_needles.end(); ++it) {
            if (it->data() == nullptr && it->size() == 0) {
                m_needles.insert(StringData()); // nulls
            }
            else {
                m_needle_storage.emplace_back(StringBuffer());
                m_needle_storage.back().append(it->data(), it->size());
                m_needles.insert(StringData(m_needle_storage.back().data(), m_needle_storage.back().size()));
=======
    StringNode<Equal>(const StringNode& from)
        : StringNodeEqualBase(from)
    {
        for (auto& needle : from.m_needles) {
            if (needle.is_null()) {
                m_needles.emplace();
            }
            else {
                m_needle_storage.push_back(std::make_unique<char[]>(needle.size()));
                std::copy(needle.data(), needle.data() + needle.size(), m_needle_storage.back().get());
                m_needles.insert(StringData(m_needle_storage.back().get(), needle.size()));
            }
        }
    }
    void index_based_aggregate(size_t limit, Evaluator evaluator) override
    {
        if (limit == 0)
            return;
        if (m_index_matches == nullptr) {
            if (m_results_end) { // 1 result
                auto obj = m_table->get_object(m_actual_key);
                evaluator(obj);
            }
        }
        else { // multiple results
            for (size_t t = m_results_start; t < m_results_end && limit > 0; ++t) {
                auto obj = m_table->get_object(ObjKey(m_index_matches->get(t)));
                if (evaluator(obj)) {
                    --limit;
                }
>>>>>>> origin/develop12
            }
        }
    }

private:
<<<<<<< HEAD
    template <class ArrayType>
    size_t find_first_in(ArrayType& array, size_t begin, size_t end);

    size_t _find_first_local(size_t start, size_t end) override;
    std::unordered_set<StringData> m_needles;
    std::vector<StringBuffer> m_needle_storage;
=======
    std::unique_ptr<IntegerColumn> m_index_matches;

    ObjKey get_key(size_t ndx) override
    {
        if (IntegerColumn* vec = m_index_matches.get()) {
            return ObjKey(vec->get(ndx));
        }
        else if (m_results_end == 1) {
            return m_actual_key;
        }
        return ObjKey();
    }

    size_t _find_first_local(size_t start, size_t end) override;
    std::unordered_set<StringData> m_needles;
    std::vector<std::unique_ptr<char[]>> m_needle_storage;
>>>>>>> origin/develop12
};


// Specialization for EqualIns condition on Strings - we specialize because we can utilize indexes (if they exist) for
// EqualIns.
template <>
class StringNode<EqualIns> : public StringNodeEqualBase {
public:
<<<<<<< HEAD
    StringNode(StringData v, size_t column)
=======
    StringNode(StringData v, ColKey column)
>>>>>>> origin/develop12
        : StringNodeEqualBase(v, column)
    {
        auto upper = case_map(v, true);
        auto lower = case_map(v, false);
        if (!upper || !lower) {
            error_code = "Malformed UTF-8: " + std::string(v);
        }
        else {
            m_ucase = std::move(*upper);
            m_lcase = std::move(*lower);
        }
    }

<<<<<<< HEAD
=======
    void clear_leaf_state() override
    {
        StringNodeEqualBase::clear_leaf_state();
        m_index_matches.clear();
    }

    void table_changed() override
    {
        StringNodeBase::table_changed();
        m_has_search_index = m_table.unchecked_ptr()->has_search_index(m_condition_column_key);
    }
>>>>>>> origin/develop12
    void _search_index_init() override;

    virtual std::string describe_condition() const override
    {
        return EqualIns::description();
    }

<<<<<<< HEAD
    std::unique_ptr<ParentNode> clone(QueryNodeHandoverPatches* patches) const override
    {
        return std::unique_ptr<ParentNode>(new StringNode(*this, patches));
    }

    StringNode(const StringNode& from, QueryNodeHandoverPatches* patches)
        : StringNodeEqualBase(from, patches)
=======
    std::unique_ptr<ParentNode> clone() const override
    {
        return std::unique_ptr<ParentNode>(new StringNode(*this));
    }

    StringNode(const StringNode& from)
        : StringNodeEqualBase(from)
>>>>>>> origin/develop12
        , m_ucase(from.m_ucase)
        , m_lcase(from.m_lcase)
    {
    }

<<<<<<< HEAD
private:
    std::string m_ucase;
    std::string m_lcase;

=======
    void index_based_aggregate(size_t limit, Evaluator evaluator) override
    {
        for (size_t t = 0; t < m_index_matches.size() && limit > 0; ++t) {
            auto obj = m_table->get_object(m_index_matches[t]);
            if (evaluator(obj)) {
                --limit;
            }
        }
    }

private:
    // Used for index lookup
    std::vector<ObjKey> m_index_matches;
    std::string m_ucase;
    std::string m_lcase;

    ObjKey get_key(size_t ndx) override
    {
        return m_index_matches[ndx];
    }

>>>>>>> origin/develop12
    size_t _find_first_local(size_t start, size_t end) override;
};

// OR node contains at least two node pointers: Two or more conditions to OR
// together in m_conditions, and the next AND condition (if any) in m_child.
//
// For 'second.equal(23).begin_group().first.equal(111).Or().first.equal(222).end_group().third().equal(555)', this
// will first set m_conditions[0] = left-hand-side through constructor, and then later, when .first.equal(222) is
// invoked, invocation will set m_conditions[1] = right-hand-side through Query& Query::Or() (see query.cpp).
// In there, m_child is also set to next AND condition (if any exists) following the OR.
class OrNode : public ParentNode {
public:
    OrNode(std::unique_ptr<ParentNode> condition)
    {
        m_dT = 50.0;
        if (condition)
            m_conditions.emplace_back(std::move(condition));
    }

<<<<<<< HEAD
    OrNode(const OrNode& other, QueryNodeHandoverPatches* patches)
        : ParentNode(other, patches)
    {
        for (const auto& condition : other.m_conditions) {
            m_conditions.emplace_back(condition->clone(patches));
=======
    OrNode(const OrNode& other)
        : ParentNode(other)
    {
        for (const auto& condition : other.m_conditions) {
            m_conditions.emplace_back(condition->clone());
>>>>>>> origin/develop12
        }
    }

    void table_changed() override
    {
        for (auto& condition : m_conditions) {
<<<<<<< HEAD
            condition->set_table(*m_table);
        }
    }

    void verify_column() const override
    {
        for (auto& condition : m_conditions) {
            condition->verify_column();
        }
=======
            condition->set_table(m_table);
        }
    }

    void cluster_changed() override
    {
        for (auto& condition : m_conditions) {
            condition->set_cluster(m_cluster);
        }

        m_start.clear();
        m_start.resize(m_conditions.size(), 0);

        m_last.clear();
        m_last.resize(m_conditions.size(), 0);

        m_was_match.clear();
        m_was_match.resize(m_conditions.size(), false);
>>>>>>> origin/develop12
    }

    std::string describe(util::serializer::SerialisationState& state) const override
    {
        std::string s;
        for (size_t i = 0; i < m_conditions.size(); ++i) {
            if (m_conditions[i]) {
                s += m_conditions[i]->describe_expression(state);
                if (i != m_conditions.size() - 1) {
                    s += " or ";
                }
            }
        }
        if (m_conditions.size() > 1) {
            s = "(" + s + ")";
        }
        return s;
    }

<<<<<<< HEAD
    void init() override
    {
        ParentNode::init();

        m_dD = 10.0;

        std::sort(m_conditions.begin(), m_conditions.end(),
                  [](auto& a, auto& b) { return a->m_condition_column_idx < b->m_condition_column_idx; });

        combine_conditions<StringNode<Equal>>();
        combine_conditions<IntegerNode<IntegerColumn, Equal>>();
        combine_conditions<IntegerNode<IntNullColumn, Equal>>();
=======
    void collect_dependencies(std::vector<TableKey>& versions) const override
    {
        for (const auto& cond : m_conditions) {
            cond->collect_dependencies(versions);
        }
    }

    void init(bool will_query_ranges) override
    {
        ParentNode::init(will_query_ranges);

        m_dD = 10.0;

        combine_conditions(!will_query_ranges);
>>>>>>> origin/develop12

        m_start.clear();
        m_start.resize(m_conditions.size(), 0);

        m_last.clear();
        m_last.resize(m_conditions.size(), 0);

        m_was_match.clear();
        m_was_match.resize(m_conditions.size(), false);

        std::vector<ParentNode*> v;
        for (auto& condition : m_conditions) {
<<<<<<< HEAD
            condition->init();
=======
            condition->init(will_query_ranges);
>>>>>>> origin/develop12
            v.clear();
            condition->gather_children(v);
        }
    }

    size_t find_first_local(size_t start, size_t end) override
    {
        if (start >= end)
            return not_found;

        size_t index = not_found;

        for (size_t c = 0; c < m_conditions.size(); ++c) {
            // out of order search; have to discard cached results
            if (start < m_start[c]) {
                m_last[c] = 0;
                m_was_match[c] = false;
            }
            // already searched this range and didn't match
            else if (m_last[c] >= end)
                continue;
            // already search this range and *did* match
            else if (m_was_match[c] && m_last[c] >= start) {
                if (index > m_last[c])
                    index = m_last[c];
                continue;
            }

            m_start[c] = start;
            size_t fmax = std::max(m_last[c], start);
            size_t f = m_conditions[c]->find_first(fmax, end);
            m_was_match[c] = f != not_found;
            m_last[c] = f == not_found ? end : f;
            if (f != not_found && index > m_last[c])
                index = m_last[c];
        }

        return index;
    }

    std::string validate() override
    {
        if (error_code != "")
            return error_code;
        if (m_conditions.size() == 0)
            return "Missing left-hand side of OR";
        if (m_conditions.size() == 1)
            return "Missing right-hand side of OR";
        std::string s;
        if (m_child != 0)
            s = m_child->validate();
        if (s != "")
            return s;
        for (size_t i = 0; i < m_conditions.size(); ++i) {
            s = m_conditions[i]->validate();
            if (s != "")
                return s;
        }
        return "";
    }

<<<<<<< HEAD
    std::unique_ptr<ParentNode> clone(QueryNodeHandoverPatches* patches) const override
    {
        return std::unique_ptr<ParentNode>(new OrNode(*this, patches));
    }

    void apply_handover_patch(QueryNodeHandoverPatches& patches, Group& group) override
    {
        for (auto it = m_conditions.rbegin(); it != m_conditions.rend(); ++it)
            (*it)->apply_handover_patch(patches, group);

        ParentNode::apply_handover_patch(patches, group);
=======
    std::unique_ptr<ParentNode> clone() const override
    {
        return std::unique_ptr<ParentNode>(new OrNode(*this));
>>>>>>> origin/develop12
    }

    std::vector<std::unique_ptr<ParentNode>> m_conditions;

private:
<<<<<<< HEAD
    template<class QueryNodeType>
    void combine_conditions() {
        QueryNodeType* first_match = nullptr;
        QueryNodeType* advance = nullptr;
        auto it = m_conditions.begin();
        while (it != m_conditions.end()) {
            // Only try to optimize on QueryNodeType conditions without search index
            auto node = it->get();
            if ((first_match = dynamic_cast<QueryNodeType*>(node)) && first_match->m_child == nullptr &&
                !first_match->has_search_index()) {
                auto col_ndx = first_match->m_condition_column_idx;
                auto next = it + 1;
                while (next != m_conditions.end() && (*next)->m_condition_column_idx == col_ndx) {
                    auto next_node = next->get();
                    if ((advance = dynamic_cast<QueryNodeType*>(next_node)) && next_node->m_child == nullptr) {
                        first_match->consume_condition(advance);
                        next = m_conditions.erase(next);
                    }
                    else {
                        ++next;
                    }
                }
                it = next;
            }
            else {
                ++it;
            }
        }
=======
    void combine_conditions(bool ignore_indexes)
    {
        std::sort(m_conditions.begin(), m_conditions.end(),
                  [](auto& a, auto& b) { return a->m_condition_column_key < b->m_condition_column_key; });

        auto prev = m_conditions.begin()->get();
        auto cond = [&](auto& node) {
            if (prev->consume_condition(*node, ignore_indexes))
                return true;
            prev = &*node;
            return false;
        };
        m_conditions.erase(std::remove_if(m_conditions.begin() + 1, m_conditions.end(), cond), m_conditions.end());
>>>>>>> origin/develop12
    }

    // start index of the last find for each cond
    std::vector<size_t> m_start;
    // last looked at index of the lasft find for each cond
    // is a matching index if m_was_match is true
    std::vector<size_t> m_last;
    std::vector<bool> m_was_match;
};


class NotNode : public ParentNode {
public:
    NotNode(std::unique_ptr<ParentNode> condition)
        : m_condition(std::move(condition))
    {
        m_dT = 50.0;
    }

    void table_changed() override
    {
<<<<<<< HEAD
        m_condition->set_table(*m_table);
    }

    void verify_column() const override
    {
        m_condition->verify_column();
    }

    void init() override
    {
        ParentNode::init();
=======
        m_condition->set_table(m_table);
    }

    void cluster_changed() override
    {
        m_condition->set_cluster(m_cluster);
        // Heuristics bookkeeping:
        m_known_range_start = 0;
        m_known_range_end = 0;
        m_first_in_known_range = not_found;
    }

    void init(bool will_query_ranges) override
    {
        ParentNode::init(will_query_ranges);
>>>>>>> origin/develop12

        m_dD = 10.0;

        std::vector<ParentNode*> v;

<<<<<<< HEAD
        m_condition->init();
        v.clear();
        m_condition->gather_children(v);

        // Heuristics bookkeeping:
        m_known_range_start = 0;
        m_known_range_end = 0;
        m_first_in_known_range = not_found;
=======
        m_condition->init(false);
        v.clear();
        m_condition->gather_children(v);
>>>>>>> origin/develop12
    }

    size_t find_first_local(size_t start, size_t end) override;

    std::string validate() override
    {
        if (error_code != "")
            return error_code;
        if (m_condition == 0)
            return "Missing argument to Not";
        std::string s;
        if (m_child != 0)
            s = m_child->validate();
        if (s != "")
            return s;
        s = m_condition->validate();
        if (s != "")
            return s;
        return "";
    }

<<<<<<< HEAD
    virtual std::string describe(util::serializer::SerialisationState& state) const override
=======
    std::string describe(util::serializer::SerialisationState& state) const override
>>>>>>> origin/develop12
    {
        if (m_condition) {
            return "!(" + m_condition->describe_expression(state) + ")";
        }
        return "!()";
    }

<<<<<<< HEAD

    std::unique_ptr<ParentNode> clone(QueryNodeHandoverPatches* patches) const override
    {
        return std::unique_ptr<ParentNode>(new NotNode(*this, patches));
    }

    NotNode(const NotNode& from, QueryNodeHandoverPatches* patches)
        : ParentNode(from, patches)
        , m_condition(from.m_condition ? from.m_condition->clone(patches) : nullptr)
=======
    void collect_dependencies(std::vector<TableKey>& versions) const override
    {
        if (m_condition) {
            m_condition->collect_dependencies(versions);
        }
    }


    std::unique_ptr<ParentNode> clone() const override
    {
        return std::unique_ptr<ParentNode>(new NotNode(*this));
    }

    NotNode(const NotNode& from)
        : ParentNode(from)
        , m_condition(from.m_condition ? from.m_condition->clone() : nullptr)
>>>>>>> origin/develop12
        , m_known_range_start(from.m_known_range_start)
        , m_known_range_end(from.m_known_range_end)
        , m_first_in_known_range(from.m_first_in_known_range)
    {
    }

<<<<<<< HEAD
    void apply_handover_patch(QueryNodeHandoverPatches& patches, Group& group) override
    {
        m_condition->apply_handover_patch(patches, group);
        ParentNode::apply_handover_patch(patches, group);
    }

=======
>>>>>>> origin/develop12
    std::unique_ptr<ParentNode> m_condition;

private:
    // FIXME This heuristic might as well be reused for all condition nodes.
    size_t m_known_range_start;
    size_t m_known_range_end;
    size_t m_first_in_known_range;

    bool evaluate_at(size_t rowndx);
    void update_known(size_t start, size_t end, size_t first);
    size_t find_first_loop(size_t start, size_t end);
    size_t find_first_covers_known(size_t start, size_t end);
    size_t find_first_covered_by_known(size_t start, size_t end);
    size_t find_first_overlap_lower(size_t start, size_t end);
    size_t find_first_overlap_upper(size_t start, size_t end);
    size_t find_first_no_overlap(size_t start, size_t end);
};


// Compare two columns with eachother row-by-row
<<<<<<< HEAD
template <class ColType, class TConditionFunction>
class TwoColumnsNode : public ParentNode {
public:
    using TConditionValue = typename ColType::value_type;

    TwoColumnsNode(size_t column1, size_t column2)
    {
        m_dT = 100.0;
        m_condition_column_idx1 = column1;
        m_condition_column_idx2 = column2;
=======
template <class LeafType, class TConditionFunction>
class TwoColumnsNode : public ParentNode {
public:
    using TConditionValue = typename LeafType::value_type;

    TwoColumnsNode(ColKey column1, ColKey column2)
    {
        m_dT = 100.0;
        m_condition_column_key1 = column1;
        m_condition_column_key2 = column2;
>>>>>>> origin/develop12
    }

    ~TwoColumnsNode() noexcept override
    {
<<<<<<< HEAD
        delete[] m_value.data();
    }

    void table_changed() override
    {
        m_getter1.init(&get_column<ColType>(m_condition_column_idx1));
        m_getter2.init(&get_column<ColType>(m_condition_column_idx2));
    }

    void verify_column() const override
    {
        do_verify_column(m_getter1.m_column, m_condition_column_idx1);
        do_verify_column(m_getter2.m_column, m_condition_column_idx2);
=======
    }

    void cluster_changed() override
    {
        m_array_ptr1 = nullptr;
        m_array_ptr1 = LeafPtr(new (&m_leaf_cache_storage1) LeafType(m_table.unchecked_ptr()->get_alloc()));
        this->m_cluster->init_leaf(this->m_condition_column_key1, m_array_ptr1.get());
        m_leaf_ptr1 = m_array_ptr1.get();

        m_array_ptr2 = nullptr;
        m_array_ptr2 = LeafPtr(new (&m_leaf_cache_storage2) LeafType(m_table.unchecked_ptr()->get_alloc()));
        this->m_cluster->init_leaf(this->m_condition_column_key2, m_array_ptr2.get());
        m_leaf_ptr2 = m_array_ptr2.get();
>>>>>>> origin/develop12
    }

    virtual std::string describe(util::serializer::SerialisationState& state) const override
    {
<<<<<<< HEAD
        REALM_ASSERT(m_getter1.m_column != nullptr && m_getter2.m_column != nullptr);
        return state.describe_column(ParentNode::m_table, m_getter1.m_column->get_column_index())
            + " " + describe_condition() + " "
            + state.describe_column(ParentNode::m_table,m_getter2.m_column->get_column_index());
=======
        REALM_ASSERT(m_condition_column_key1 && m_condition_column_key2);
        return state.describe_column(ParentNode::m_table, m_condition_column_key1) + " " + describe_condition() +
               " " + state.describe_column(ParentNode::m_table, m_condition_column_key2);
>>>>>>> origin/develop12
    }

    virtual std::string describe_condition() const override
    {
        return TConditionFunction::description();
    }

<<<<<<< HEAD
    void init() override
    {
        ParentNode::init();
=======
    void init(bool will_query_ranges) override
    {
        ParentNode::init(will_query_ranges);
>>>>>>> origin/develop12
        m_dD = 100.0;
    }

    size_t find_first_local(size_t start, size_t end) override
    {
        size_t s = start;

        while (s < end) {
            if (std::is_same<TConditionValue, int64_t>::value) {
                // For int64_t we've created an array intrinsics named compare_leafs which template expands bitwidths
                // of boths arrays to make Get faster.
<<<<<<< HEAD
                m_getter1.cache_next(s);
                m_getter2.cache_next(s);

                QueryState<int64_t> qs;
                bool resume = m_getter1.m_leaf_ptr->template compare_leafs<TConditionFunction, act_ReturnFirst>(
                    m_getter2.m_leaf_ptr, s - m_getter1.m_leaf_start, m_getter1.local_end(end), 0, &qs,
                    CallbackDummy());

                if (resume)
                    s = m_getter1.m_leaf_end;
                else
                    return to_size_t(qs.m_state) + m_getter1.m_leaf_start;
=======
                QueryState<int64_t> qs(act_ReturnFirst);
                bool resume = m_leaf_ptr1->template compare_leafs<TConditionFunction, act_ReturnFirst>(
                    m_leaf_ptr2, start, end, 0, &qs, CallbackDummy());

                if (resume)
                    s = end;
                else
                    return to_size_t(qs.m_state);
>>>>>>> origin/develop12
            }
            else {
// This is for float and double.

#if 0 && defined(REALM_COMPILER_AVX)
// AVX has been disabled because of array alignment (see https://app.asana.com/0/search/8836174089724/5763107052506)
//
// For AVX you can call things like if (sseavx<1>()) to test for AVX, and then utilize _mm256_movemask_ps (VC)
// or movemask_cmp_ps (gcc/clang)
//
// See https://github.com/rrrlasse/realm/tree/AVX for an example of utilizing AVX for a two-column search which has
// been benchmarked to: floats: 288 ms vs 552 by using AVX compared to 2-level-unrolled FPU loop. doubles: 415 ms vs
// 475 (more bandwidth bound). Tests against SSE have not been performed; AVX may not pay off. Please benchmark
#endif

<<<<<<< HEAD
                TConditionValue v1 = m_getter1.get_next(s);
                TConditionValue v2 = m_getter2.get_next(s);
=======
                TConditionValue v1 = m_leaf_ptr1->get(s);
                TConditionValue v2 = m_leaf_ptr2->get(s);
>>>>>>> origin/develop12
                TConditionFunction C;

                if (C(v1, v2))
                    return s;
                else
                    s++;
            }
        }
        return not_found;
    }

<<<<<<< HEAD
    std::unique_ptr<ParentNode> clone(QueryNodeHandoverPatches* patches) const override
    {
        return std::unique_ptr<ParentNode>(new TwoColumnsNode<ColType, TConditionFunction>(*this, patches));
    }

    TwoColumnsNode(const TwoColumnsNode& from, QueryNodeHandoverPatches* patches)
        : ParentNode(from, patches)
        , m_value(from.m_value)
        , m_condition_column(from.m_condition_column)
        , m_column_type(from.m_column_type)
        , m_condition_column_idx1(from.m_condition_column_idx1)
        , m_condition_column_idx2(from.m_condition_column_idx2)
    {
        if (m_condition_column)
            m_condition_column_idx = m_condition_column->get_column_index();
        copy_getter(m_getter1, m_condition_column_idx1, from.m_getter1, patches);
        copy_getter(m_getter2, m_condition_column_idx2, from.m_getter2, patches);
    }

private:
    BinaryData m_value;
    const BinaryColumn* m_condition_column = nullptr;
    ColumnType m_column_type;

    size_t m_condition_column_idx1 = not_found;
    size_t m_condition_column_idx2 = not_found;

    SequentialGetter<ColType> m_getter1;
    SequentialGetter<ColType> m_getter2;
=======
    std::unique_ptr<ParentNode> clone() const override
    {
        return std::unique_ptr<ParentNode>(new TwoColumnsNode<LeafType, TConditionFunction>(*this));
    }

    TwoColumnsNode(const TwoColumnsNode& from)
        : ParentNode(from)
        , m_condition_column_key1(from.m_condition_column_key1)
        , m_condition_column_key2(from.m_condition_column_key2)
    {
    }

private:
    mutable ColKey m_condition_column_key1;
    mutable ColKey m_condition_column_key2;

    using LeafCacheStorage = typename std::aligned_storage<sizeof(LeafType), alignof(LeafType)>::type;
    using LeafPtr = std::unique_ptr<LeafType, PlacementDelete>;

    LeafCacheStorage m_leaf_cache_storage1;
    LeafPtr m_array_ptr1;
    const LeafType* m_leaf_ptr1 = nullptr;
    LeafCacheStorage m_leaf_cache_storage2;
    LeafPtr m_array_ptr2;
    const LeafType* m_leaf_ptr2 = nullptr;
>>>>>>> origin/develop12
};


// For Next-Generation expressions like col1 / col2 + 123 > col4 * 100.
class ExpressionNode : public ParentNode {
public:
    ExpressionNode(std::unique_ptr<Expression>);

<<<<<<< HEAD
    void init() override;
    size_t find_first_local(size_t start, size_t end) override;

    void table_changed() override;
    void verify_column() const override;

    virtual std::string describe(util::serializer::SerialisationState& state) const override;

    std::unique_ptr<ParentNode> clone(QueryNodeHandoverPatches* patches) const override;
    void apply_handover_patch(QueryNodeHandoverPatches& patches, Group& group) override;

private:
    ExpressionNode(const ExpressionNode& from, QueryNodeHandoverPatches* patches);
=======
    void init(bool) override;
    size_t find_first_local(size_t start, size_t end) override;

    void table_changed() override;
    void cluster_changed() override;
    void collect_dependencies(std::vector<TableKey>&) const override;

    virtual std::string describe(util::serializer::SerialisationState& state) const override;

    std::unique_ptr<ParentNode> clone() const override;

private:
    ExpressionNode(const ExpressionNode& from);
>>>>>>> origin/develop12

    std::unique_ptr<Expression> m_expression;
};


<<<<<<< HEAD
struct LinksToNodeHandoverPatch : public QueryNodeHandoverPatch {
    std::vector<std::unique_ptr<RowBaseHandoverPatch>> m_target_rows;
    size_t m_origin_column;
};

class LinksToNode : public ParentNode {
public:
    LinksToNode(size_t origin_column_index, const ConstRow& target_row)
        : m_origin_column(origin_column_index)
        , m_target_rows(1, target_row)
    {
        m_dD = 10.0;
        m_dT = 50.0;
    }

    LinksToNode(size_t origin_column_index, const std::vector<ConstRow>& target_rows)
        : m_origin_column(origin_column_index)
        , m_target_rows(target_rows)
    {
        m_dD = 10.0;
        m_dT = 50.0;
=======
class LinksToNode : public ParentNode {
public:
    LinksToNode(ColKey origin_column_key, ObjKey target_key)
        : m_target_keys(1, target_key)
    {
        m_dD = 10.0;
        m_dT = 50.0;
        m_condition_column_key = origin_column_key;
    }

    LinksToNode(ColKey origin_column_key, const std::vector<ObjKey>& target_keys)
        : m_target_keys(target_keys)
    {
        m_dD = 10.0;
        m_dT = 50.0;
        m_condition_column_key = origin_column_key;
>>>>>>> origin/develop12
    }

    void table_changed() override
    {
<<<<<<< HEAD
        m_column_type = m_table->get_column_type(m_origin_column);
        m_column = &const_cast<Table*>(m_table.get())->get_column_link_base(m_origin_column);
        REALM_ASSERT(m_column_type == type_Link || m_column_type == type_LinkList);
    }

    void verify_column() const override
    {
        do_verify_column(m_column, m_origin_column);
    }

    virtual std::string describe(util::serializer::SerialisationState&) const override
    {
        throw SerialisationError("Serialising a query which links to an object is currently unsupported.");
        // We can do something like the following when core gets stable keys
        //return describe_column() + " " + describe_condition() + " " + util::serializer::print_value(m_target_row.get_index());
    }
    virtual std::string describe_condition() const override
    {
        return "links to";
    }


    size_t find_first_local(size_t start, size_t end) override
    {
        REALM_ASSERT(m_column);
        if (m_column_type == type_Link) {
            LinkColumn& cl = static_cast<LinkColumn&>(*m_column);
            for (auto& row : m_target_rows) {
                if (row.is_attached()) {
                    // LinkColumn stores link to row N as the integer N + 1
                    auto pos = cl.find_first(row.get_index() + 1, start, end);
=======
        m_column_type = m_table.unchecked_ptr()->get_column_type(m_condition_column_key);
        REALM_ASSERT(m_column_type == type_Link || m_column_type == type_LinkList);
    }

    void cluster_changed() override
    {
        m_array_ptr = nullptr;
        if (m_column_type == type_Link) {
            m_array_ptr = LeafPtr(new (&m_storage.m_list) ArrayKey(m_table.unchecked_ptr()->get_alloc()));
        }
        else if (m_column_type == type_LinkList) {
            m_array_ptr = LeafPtr(new (&m_storage.m_linklist) ArrayList(m_table.unchecked_ptr()->get_alloc()));
        }
        m_cluster->init_leaf(this->m_condition_column_key, m_array_ptr.get());
        m_leaf_ptr = m_array_ptr.get();
    }

    virtual std::string describe(util::serializer::SerialisationState& state) const override
    {
        REALM_ASSERT(m_condition_column_key);
        if (m_target_keys.size() > 1)
            throw SerialisationError("Serialising a query which links to multiple objects is currently unsupported.");
        return state.describe_column(ParentNode::m_table, m_condition_column_key) + " " + describe_condition() + " " +
               util::serializer::print_value(m_target_keys[0]);
    }

    virtual std::string describe_condition() const override
    {
        return "==";
    }

    size_t find_first_local(size_t start, size_t end) override
    {
        if (m_column_type == type_Link) {
            for (auto& key : m_target_keys) {
                if (key) {
                    // LinkColumn stores link to row N as the integer N + 1
                    auto pos = static_cast<const ArrayKey*>(m_leaf_ptr)->find_first(key, start, end);
>>>>>>> origin/develop12
                    if (pos != realm::npos) {
                        return pos;
                    }
                }
            }
        }
        else if (m_column_type == type_LinkList) {
<<<<<<< HEAD
            LinkListColumn& cll = static_cast<LinkListColumn&>(*m_column);

            for (size_t i = start; i < end; i++) {
                LinkViewRef lv = cll.get(i);
                for (auto& row : m_target_rows) {
                    if (row.is_attached()) {
                        if (lv->find(row.get_index()) != not_found)
                            return i;
=======
            ArrayKeyNonNullable arr(m_table.unchecked_ptr()->get_alloc());
            for (size_t i = start; i < end; i++) {
                if (ref_type ref = static_cast<const ArrayList*>(m_leaf_ptr)->get(i)) {
                    arr.init_from_ref(ref);
                    for (auto& key : m_target_keys) {
                        if (key) {
                            if (arr.find_first(key, 0, arr.size()) != not_found)
                                return i;
                        }
>>>>>>> origin/develop12
                    }
                }
            }
        }

        return not_found;
    }

<<<<<<< HEAD
    std::unique_ptr<ParentNode> clone(QueryNodeHandoverPatches* patches) const override
    {
        return std::unique_ptr<ParentNode>(patches ? new LinksToNode(*this, patches) : new LinksToNode(*this));
    }

    void apply_handover_patch(QueryNodeHandoverPatches& patches, Group& group) override
    {
        REALM_ASSERT(patches.size());
        std::unique_ptr<QueryNodeHandoverPatch> abstract_patch = std::move(patches.back());
        patches.pop_back();

        auto patch = dynamic_cast<LinksToNodeHandoverPatch*>(abstract_patch.get());
        REALM_ASSERT(patch);

        m_origin_column = patch->m_origin_column;
        auto sz = patch->m_target_rows.size();
        m_target_rows.resize(sz);
        for (size_t i = 0; i < sz; i++) {
            m_target_rows[i].apply_and_consume_patch(patch->m_target_rows[i], group);
        }

        ParentNode::apply_handover_patch(patches, group);
    }

private:
    size_t m_origin_column = npos;
    std::vector<ConstRow> m_target_rows;
    LinkColumnBase* m_column = nullptr;
    DataType m_column_type;

    LinksToNode(const LinksToNode& source, QueryNodeHandoverPatches* patches)
        : ParentNode(source, patches)
    {
        auto patch = std::make_unique<LinksToNodeHandoverPatch>();
        patch->m_origin_column = source.m_column->get_column_index();
        auto sz = source.m_target_rows.size();
        patch->m_target_rows.resize(sz);
        for (size_t i = 0; i < sz; i++) {
            ConstRow::generate_patch(source.m_target_rows[i], patch->m_target_rows[i]);
        }
        patches->push_back(std::move(patch));
=======
    std::unique_ptr<ParentNode> clone() const override
    {
        return std::unique_ptr<ParentNode>(new LinksToNode(*this));
    }

private:
    std::vector<ObjKey> m_target_keys;
    DataType m_column_type = type_Link;
    using LeafPtr = std::unique_ptr<ArrayPayload, PlacementDelete>;
    union Storage {
        typename std::aligned_storage<sizeof(ArrayKey), alignof(ArrayKey)>::type m_list;
        typename std::aligned_storage<sizeof(ArrayList), alignof(ArrayList)>::type m_linklist;
    };
    Storage m_storage;
    LeafPtr m_array_ptr;
    const ArrayPayload* m_leaf_ptr = nullptr;


    LinksToNode(const LinksToNode& source)
        : ParentNode(source)
        , m_target_keys(source.m_target_keys)
        , m_column_type(source.m_column_type)
    {
>>>>>>> origin/develop12
    }
};

} // namespace realm

#endif // REALM_QUERY_ENGINE_HPP
