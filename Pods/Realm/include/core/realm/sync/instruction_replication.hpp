/*************************************************************************
 *
 * Copyright 2017 Realm Inc.
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

#ifndef REALM_SYNC_IMPL_INSTRUCTION_REPLICATION_HPP
#define REALM_SYNC_IMPL_INSTRUCTION_REPLICATION_HPP

#include <realm/replication.hpp>
#include <realm/sync/instructions.hpp>
#include <realm/sync/object.hpp>
#include <realm/sync/changeset_encoder.hpp>

namespace realm {
namespace sync {


<<<<<<< HEAD
class InstructionReplication: public TrivialReplication, public ObjectIDProvider {
public:
    enum class TableBehavior {
        Class,
        Array,
        Ignore
    };

    explicit InstructionReplication(const std::string& realm_path);
=======
class SyncReplication: public TrivialReplication {
public:
    explicit SyncReplication(const std::string& realm_path);
    enum class TableBehavior {
        Class,
        Ignore
    };
>>>>>>> origin/develop12
    void set_short_circuit(bool) noexcept;
    bool is_short_circuited() const noexcept;

    // reset() resets the encoder, the selected tables and the cache. It is
    // called by do_initiate_transact(), but can be called at the other times
    // as well.
    virtual void reset();

    ChangesetEncoder& get_instruction_encoder() noexcept;
    const ChangesetEncoder& get_instruction_encoder() const noexcept;

    //@{
    /// Generate instructions for Object Store tables. These must be called
    /// prior to calling the equivalent functions in Core's API. When creating a
    /// class-like table, `add_class()` must be called prior to
    /// `Group::insert_group_level_table()`. Similarly, `create_object()` or
    /// `create_object_with_primary_key()` must be called prior to
    /// `Table::insert_empty_row()` and/or `Table::set_int_unique()` or
    /// `Table::set_string_unique()` or `Table::set_null_unique()`.
    ///
    /// If a class-like table is added, or an object-like row is inserted,
    /// without calling these methods first, an exception will be thrown.
    ///
    /// A "class-like table" is defined as a table whose name begins with
    /// "class_" (this is the convention used by Object Store). Non-class-like
    /// tables can be created and modified using Core's API without calling
    /// these functions, because they do not result in instructions being
    /// emitted.
<<<<<<< HEAD
    void add_class(StringData table_name);
    void add_class_with_primary_key(StringData table_name, DataType pk_type, StringData pk_field, bool nullable);
    void create_object(const Table*, ObjectID);
    void create_object_with_primary_key(const Table*, ObjectID, StringData);
    void create_object_with_primary_key(const Table*, ObjectID, int_fast64_t);
    void create_object_with_primary_key(const Table*, ObjectID, realm::util::None);
=======
    void add_class(StringData table_name) override;
    void add_class_with_primary_key(StringData table_name, DataType pk_type, StringData pk_field, bool nullable) override;
    void create_object(const Table*, GlobalKey) override;
    void create_object_with_primary_key(const Table*, GlobalKey, Mixed) override;
>>>>>>> origin/develop12
    void prepare_erase_table(StringData table_name);
    //@}

    // TrivialReplication interface:
<<<<<<< HEAD
    void initialize(SharedGroup&) override;

    // TransactLogConvenientEncoder interface:
    void insert_group_level_table(size_t table_ndx, size_t num_tables, StringData name) override;
    void erase_group_level_table(size_t table_ndx, size_t num_tables) override;
    void rename_group_level_table(size_t table_ndx, StringData new_name) override;
    void insert_column(const Descriptor&, size_t col_ndx, DataType type, StringData name, LinkTargetInfo& link,
                               bool nullable = false) override;
    void erase_column(const Descriptor&, size_t col_ndx) override;
    void rename_column(const Descriptor&, size_t col_ndx, StringData name) override;

    void set_int(const Table*, size_t col_ndx, size_t ndx, int_fast64_t value, _impl::Instruction variant) override;
    void add_int(const Table*, size_t col_ndx, size_t ndx, int_fast64_t value) override;
    void set_bool(const Table*, size_t col_ndx, size_t ndx, bool value, _impl::Instruction variant) override;
    void set_float(const Table*, size_t col_ndx, size_t ndx, float value, _impl::Instruction variant) override;
    void set_double(const Table*, size_t col_ndx, size_t ndx, double value, _impl::Instruction variant) override;
    void set_string(const Table*, size_t col_ndx, size_t ndx, StringData value, _impl::Instruction variant) override;
    void set_binary(const Table*, size_t col_ndx, size_t ndx, BinaryData value, _impl::Instruction variant) override;
    void set_olddatetime(const Table*, size_t col_ndx, size_t ndx, OldDateTime value,
                                 _impl::Instruction variant) override;
    void set_timestamp(const Table*, size_t col_ndx, size_t ndx, Timestamp value, _impl::Instruction variant) override;
    void set_table(const Table*, size_t col_ndx, size_t ndx, _impl::Instruction variant) override;
    void set_mixed(const Table*, size_t col_ndx, size_t ndx, const Mixed& value, _impl::Instruction variant) override;
    void set_link(const Table*, size_t col_ndx, size_t ndx, size_t value, _impl::Instruction variant) override;
    void set_null(const Table*, size_t col_ndx, size_t ndx, _impl::Instruction variant) override;
    void set_link_list(const LinkView&, const IntegerColumn& values) override;
    void insert_substring(const Table*, size_t col_ndx, size_t row_ndx, size_t pos, StringData) override;
    void erase_substring(const Table*, size_t col_ndx, size_t row_ndx, size_t pos, size_t size) override;
    void insert_empty_rows(const Table*, size_t row_ndx, size_t num_rows_to_insert, size_t prior_num_rows) override;
    void add_row_with_key(const Table*, size_t row_ndx, size_t prior_num_rows, size_t key_col_ndx, int64_t key) override;
    void erase_rows(const Table*, size_t row_ndx, size_t num_rows_to_erase, size_t prior_num_rowsp,
                            bool is_move_last_over) override;
    void swap_rows(const Table*, size_t row_ndx_1, size_t row_ndx_2) override;
    void move_row(const Table*, size_t row_ndx_1, size_t row_ndx_2) override;
    void merge_rows(const Table*, size_t row_ndx, size_t new_row_ndx) override;
    void add_search_index(const Descriptor&, size_t col_ndx) override;
    void remove_search_index(const Descriptor&, size_t col_ndx) override;
    void set_link_type(const Table*, size_t col_ndx, LinkType) override;
    void clear_table(const Table*, size_t prior_num_rows) override;
    void optimize_table(const Table*) override;
    void link_list_set(const LinkView&, size_t ndx, size_t value) override;
    void link_list_insert(const LinkView&, size_t ndx, size_t value) override;
    void link_list_move(const LinkView&, size_t from_ndx, size_t to_ndx) override;
    void link_list_swap(const LinkView&, size_t ndx_1, size_t ndx_2) override;
    void link_list_erase(const LinkView&, size_t ndx) override;
    void link_list_clear(const LinkView&) override;
    void nullify_link(const Table*, size_t col_ndx, size_t ndx) override;
    void link_list_nullify(const LinkView&, size_t ndx) override;
=======
    void initialize(DB&) override;

    // TransactLogConvenientEncoder interface:
    void insert_group_level_table(TableKey table_key, size_t num_tables, StringData name) override;
    void erase_group_level_table(TableKey table_key, size_t num_tables) override;
    void rename_group_level_table(TableKey table_key, StringData new_name) override;
    void insert_column(const Table*, ColKey col_key, DataType type, StringData name, LinkTargetInfo& link,
                               bool nullable, bool Lsttype, LinkType) override;
    void erase_column(const Table*, ColKey col_key) override;
    void rename_column(const Table*, ColKey col_key, StringData name) override;

    void set_int(const Table*, ColKey col_key, ObjKey key, int_fast64_t value,
                 _impl::Instruction variant) override;
    void add_int(const Table*, ColKey col_key, ObjKey key, int_fast64_t value) override;
    void set_bool(const Table*, ColKey col_key, ObjKey key, bool value, _impl::Instruction variant) override;
    void set_float(const Table*, ColKey col_key, ObjKey key, float value, _impl::Instruction variant) override;
    void set_double(const Table*, ColKey col_key, ObjKey key, double value, _impl::Instruction variant) override;
    void set_string(const Table*, ColKey col_key, ObjKey key, StringData value,
                    _impl::Instruction variant) override;
    void set_binary(const Table*, ColKey col_key, ObjKey key, BinaryData value,
                    _impl::Instruction variant) override;
    void set_timestamp(const Table*, ColKey col_key, ObjKey key, Timestamp value,
                       _impl::Instruction variant) override;
    void set_link(const Table*, ColKey col_key, ObjKey key, ObjKey value, _impl::Instruction variant) override;
    void set_null(const Table*, ColKey col_key, ObjKey key, _impl::Instruction variant) override;
    void insert_substring(const Table*, ColKey col_key, ObjKey key, size_t pos, StringData) override;
    void erase_substring(const Table*, ColKey col_key, ObjKey key, size_t pos, size_t size) override;

    void list_set_null(const ConstLstBase& list, size_t ndx) override;
    void list_set_int(const ConstLstBase& Lst, size_t list_ndx, int64_t value) override;
    void list_set_bool(const ConstLstBase& Lst, size_t list_ndx, bool value) override;
    void list_set_float(const ConstLstBase& Lst, size_t list_ndx, float value) override;
    void list_set_double(const ConstLstBase& Lst, size_t list_ndx, double value) override;
    void list_set_string(const Lst<String>& Lst, size_t list_ndx, StringData value) override;
    void list_set_binary(const Lst<Binary>& Lst, size_t list_ndx, BinaryData value) override;
    void list_set_timestamp(const Lst<Timestamp>& Lst, size_t list_ndx, Timestamp value) override;

    void list_insert_int(const ConstLstBase& Lst, size_t list_ndx, int64_t value) override;
    void list_insert_bool(const ConstLstBase& Lst, size_t list_ndx, bool value) override;
    void list_insert_float(const ConstLstBase& Lst, size_t list_ndx, float value) override;
    void list_insert_double(const ConstLstBase& Lst, size_t list_ndx, double value) override;
    void list_insert_string(const Lst<String>& Lst, size_t list_ndx, StringData value) override;
    void list_insert_binary(const Lst<Binary>& Lst, size_t list_ndx, BinaryData value) override;
    void list_insert_timestamp(const Lst<Timestamp>& Lst, size_t list_ndx, Timestamp value) override;

    void create_object(const Table*, ObjKey) override;
    void remove_object(const Table*, ObjKey) override;
    void set_link_type(const Table*, ColKey col_key, LinkType) override;
    void clear_table(const Table*, size_t prior_num_rows) override;

    void list_insert_null(const ConstLstBase&, size_t ndx) override;
    void list_set_link(const Lst<ObjKey>&, size_t link_ndx, ObjKey value) override;
    void list_insert_link(const Lst<ObjKey>&, size_t link_ndx, ObjKey value) override;
    void list_move(const ConstLstBase&, size_t from_link_ndx, size_t to_link_ndx) override;
    void list_swap(const ConstLstBase&, size_t link_ndx_1, size_t link_ndx_2) override;
    void list_erase(const ConstLstBase&, size_t link_ndx) override;
    void list_clear(const ConstLstBase&) override;

    //@{

    /// Implicit nullifications due to removal of target row. This is redundant
    /// information from the point of view of replication, as the removal of the
    /// target row will reproduce the implicit nullifications in the target
    /// Realm anyway. The purpose of this instruction is to allow observers
    /// (reactor pattern) to be explicitly notified about the implicit
    /// nullifications.

    void nullify_link(const Table*, ColKey col_key, ObjKey key) override;
    void link_list_nullify(const Lst<ObjKey>&, size_t link_ndx) override;
    //@}
>>>>>>> origin/develop12

    template <class T>
    void emit(T instruction);

    TableBehavior select_table(const Table*);
    const Table* selected_table() const noexcept;

protected:
    // Replication interface:
<<<<<<< HEAD
    void do_initiate_transact(TransactionType, version_type current_version) override;
=======
    void do_initiate_transact(Group& group, version_type current_version, bool history_updated) override;
>>>>>>> origin/develop12
private:
    bool m_short_circuit = false;

    ChangesetEncoder m_encoder;
<<<<<<< HEAD
    SharedGroup* m_sg = nullptr;
    std::unique_ptr<TableInfoCache> m_cache;


    // FIXME: The base class already caches this.
    ConstTableRef m_selected_table;
    TableBehavior m_selected_table_behavior; // cache
    ConstLinkViewRef m_selected_link_list = nullptr;
=======
    DB* m_sg = nullptr;
    std::unique_ptr<TableInfoCache> m_cache;

    // FIXME: The base class already caches this.
    const Table* m_selected_table = nullptr;
    TableBehavior m_selected_table_behavior; // cache
    std::pair<ColKey, ObjKey> m_selected_list;
>>>>>>> origin/develop12

    // Consistency checks:
    std::string m_table_being_created;
    std::string m_table_being_created_primary_key;
    std::string m_table_being_erased;
<<<<<<< HEAD
    util::Optional<ObjectID> m_object_being_created;

    REALM_NORETURN void unsupported_instruction(); // Throws TransformError
    TableBehavior select_table(const Descriptor&);
    TableBehavior select_table_inner(const Table* table);
    bool select_link_list(const LinkView&); // returns true if table behavior != ignored
=======
    util::Optional<GlobalKey> m_object_being_created;

    REALM_NORETURN void unsupported_instruction(); // Throws TransformError
    TableBehavior select_table_inner(const Table* table);
    bool select_list(const ConstLstBase&); // returns true if table behavior != ignored
>>>>>>> origin/develop12

    TableBehavior get_table_behavior(const Table*) const;

    template <class T>
<<<<<<< HEAD
    void set(const Table*, size_t row_ndx, size_t col_ndx, T payload,
             _impl::Instruction variant);
    template <class T>
    void set_pk(const Table*, size_t row_ndx, size_t col_ndx, T payload,
=======
    void set(const Table*, ColKey col_key, ObjKey row_ndx, T payload,
             _impl::Instruction variant);
    template <class T>
    void list_set(const ConstLstBase& Lst, size_t ndx, T payload);
    template <class T>
    void list_insert(const ConstLstBase& Lst, size_t ndx, T payload);
    template <class T>
    void set_pk(const Table*, ColKey col_key, ObjKey row_ndx, T payload,
>>>>>>> origin/develop12
                _impl::Instruction variant);
    template <class T>
    auto as_payload(T value);
};

<<<<<<< HEAD
inline void InstructionReplication::set_short_circuit(bool b) noexcept
=======
inline void SyncReplication::set_short_circuit(bool b) noexcept
>>>>>>> origin/develop12
{
    m_short_circuit = b;
}

<<<<<<< HEAD
inline bool InstructionReplication::is_short_circuited() const noexcept
=======
inline bool SyncReplication::is_short_circuited() const noexcept
>>>>>>> origin/develop12
{
    return m_short_circuit;
}

<<<<<<< HEAD
inline ChangesetEncoder& InstructionReplication::get_instruction_encoder() noexcept
=======
inline ChangesetEncoder& SyncReplication::get_instruction_encoder() noexcept
>>>>>>> origin/develop12
{
    return m_encoder;
}

<<<<<<< HEAD
inline const ChangesetEncoder& InstructionReplication::get_instruction_encoder() const noexcept
=======
inline const ChangesetEncoder& SyncReplication::get_instruction_encoder() const noexcept
>>>>>>> origin/develop12
{
    return m_encoder;
}

template <class T>
<<<<<<< HEAD
inline void InstructionReplication::emit(T instruction)
=======
inline void SyncReplication::emit(T instruction)
>>>>>>> origin/develop12
{
    REALM_ASSERT(!m_short_circuit);
    m_encoder(instruction);
}

<<<<<<< HEAD
inline auto InstructionReplication::select_table(const Table* table) -> TableBehavior
=======
inline auto SyncReplication::select_table(const Table* table) -> TableBehavior
>>>>>>> origin/develop12
{
    if (m_selected_table == table) {
        return m_selected_table_behavior;
    }
    return select_table_inner(table);
}

<<<<<<< HEAD
inline const Table* InstructionReplication::selected_table() const noexcept
{
    return m_selected_table.get();
=======
inline const Table* SyncReplication::selected_table() const noexcept
{
    return m_selected_table;
>>>>>>> origin/develop12
}

// Temporarily short-circuit replication
class TempShortCircuitReplication {
public:
<<<<<<< HEAD
    TempShortCircuitReplication(InstructionReplication& bridge): m_bridge(bridge)
=======
    TempShortCircuitReplication(SyncReplication& bridge): m_bridge(bridge)
>>>>>>> origin/develop12
    {
        m_was_short_circuited = bridge.is_short_circuited();
        bridge.set_short_circuit(true);
    }

    ~TempShortCircuitReplication()
    {
        m_bridge.set_short_circuit(m_was_short_circuited);
    }

    bool was_short_circuited() const noexcept
    {
        return m_was_short_circuited;
    }
private:
<<<<<<< HEAD
    InstructionReplication& m_bridge;
=======
    SyncReplication& m_bridge;
>>>>>>> origin/develop12
    bool m_was_short_circuited;
};

} // namespace sync
} // namespace realm

#endif // REALM_SYNC_IMPL_INSTRUCTION_REPLICATION_HPP
