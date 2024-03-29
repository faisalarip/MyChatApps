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

#ifndef REALM_GROUP_HPP
#define REALM_GROUP_HPP

#include <functional>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>

#include <realm/util/features.h>
#include <realm/exceptions.hpp>
#include <realm/impl/input_stream.hpp>
#include <realm/impl/output_stream.hpp>
#include <realm/impl/cont_transact_hist.hpp>
#include <realm/metrics/metrics.hpp>
#include <realm/table.hpp>
#include <realm/alloc_slab.hpp>

namespace realm {

<<<<<<< HEAD
class SharedGroup;
=======
class DB;
class TableKeys;

>>>>>>> origin/develop12
namespace _impl {
class GroupFriend;
class TransactLogConvenientEncoder;
class TransactLogParser;
}


/// A group is a collection of named tables.
///
<<<<<<< HEAD
/// Tables occur in the group in an unspecified order, but an order that
/// generally remains fixed. The order is guaranteed to remain fixed between two
/// points in time if no tables are added to, or removed from the group during
/// that time. When tables are added to, or removed from the group, the order
/// may change arbitrarily.
///
/// If `table` is a table accessor attached to a group-level table, and `group`
/// is a group accessor attached to the group, then the following is guaranteed,
/// even after a change in the table order:
///
/// \code{.cpp}
///
///     table == group.get_table(table.get_index_in_group())
///
/// \endcode
///
class Group : private Table::Parent {
=======
class Group : public ArrayParent {
>>>>>>> origin/develop12
public:
    /// Construct a free-standing group. This group instance will be
    /// in the attached state, but neither associated with a file, nor
    /// with an external memory buffer.
    Group();

    enum OpenMode {
        /// Open in read-only mode. Fail if the file does not already exist.
        mode_ReadOnly,
        /// Open in read/write mode. Create the file if it doesn't exist.
        mode_ReadWrite,
        /// Open in read/write mode. Fail if the file does not already exist.
        mode_ReadWriteNoCreate
    };

    /// Equivalent to calling open(const std::string&, const char*, OpenMode)
    /// on an unattached group accessor.
    explicit Group(const std::string& file, const char* encryption_key = nullptr, OpenMode = mode_ReadOnly);

    /// Equivalent to calling open(BinaryData, bool) on an unattached
    /// group accessor. Note that if this constructor throws, the
    /// ownership of the memory buffer will remain with the caller,
    /// regardless of whether \a take_ownership is set to `true` or
    /// `false`.
    explicit Group(BinaryData, bool take_ownership = true);

    struct unattached_tag {
    };

    /// Create a Group instance in its unattached state. It may then
    /// be attached to a database file later by calling one of the
    /// open() methods. You may test whether this instance is
    /// currently in its attached state by calling
    /// is_attached(). Calling any other method (except the
    /// destructor) while in the unattached state has undefined
    /// behavior.
    Group(unattached_tag) noexcept;

    // FIXME: Implement a proper copy constructor (fairly trivial).
    Group(const Group&) = delete;
    Group& operator=(const Group&) = delete;

    ~Group() noexcept override;

    /// Attach this Group instance to the specified database file.
    ///
    /// By default, the specified file is opened in read-only mode
    /// (mode_ReadOnly). This allows opening a file even when the
    /// caller lacks permission to write to that file. The opened
    /// group may still be modified freely, but the changes cannot be
    /// written back to the same file using the commit() function. An
    /// attempt to do that, will cause an exception to be thrown. When
    /// opening in read-only mode, it is an error if the specified
    /// file does not already exist in the file system.
    ///
    /// Alternatively, the file can be opened in read/write mode
    /// (mode_ReadWrite). This allows use of the commit() function,
    /// but, of course, it also requires that the caller has
    /// permission to write to the specified file. When opening in
    /// read-write mode, an attempt to create the specified file will
    /// be made, if it does not already exist in the file system.
    ///
    /// In any case, if the file already exists, it must contain a
    /// valid Realm database. In many cases invalidity will be
    /// detected and cause the InvalidDatabase exception to be thrown,
    /// but you should not rely on it.
    ///
    /// Note that changes made to the database via a Group instance
    /// are not automatically committed to the specified file. You
    /// may, however, at any time, explicitly commit your changes by
    /// calling the commit() method, provided that the specified
    /// open-mode is not mode_ReadOnly. Alternatively, you may call
    /// write() to write the entire database to a new file. Writing
    /// the database to a new file does not end, or in any other way
    /// change the association between the Group instance and the file
    /// that was specified in the call to open().
    ///
    /// A Realm file that contains a history (see Replication::HistoryType) may
    /// be opened via Group::open(), as long as the application can ensure that
    /// there is no concurrent access to the file (see below for more on
    /// concurrency), but if the file is modified via Group::commit() the
    /// history will be discarded. To retain the history, the application must
    /// instead access the file in shared mode, i.e., via SharedGroup, and
    /// supply the right kind of replication plugin (see
    /// Replication::get_history_type()).
    ///
    /// A file that is passed to Group::open(), may not be modified by
    /// a third party until after the Group object is
    /// destroyed. Behavior is undefined if a file is modified by a
    /// third party while any Group object is associated with it.
    ///
    /// Calling open() on a Group instance that is already in the
    /// attached state has undefined behavior.
    ///
    /// Accessing a Realm database file through manual construction
    /// of a Group object does not offer any level of thread safety or
    /// transaction safety. When any of those kinds of safety are a
    /// concern, consider using a SharedGroup instead. When accessing
    /// a database file in read/write mode through a manually
    /// constructed Group object, it is entirely the responsibility of
    /// the application that the file is not accessed in any way by a
    /// third party during the life-time of that group object. It is,
    /// on the other hand, safe to concurrently access a database file
    /// by multiple manually created Group objects, as long as all of
    /// them are opened in read-only mode, and there is no other party
    /// that modifies the file concurrently.
    ///
    /// Do not call this function on a group instance that is managed
    /// by a shared group. Doing so will result in undefined behavior.
    ///
    /// Even if this function throws, it may have the side-effect of
    /// creating the specified file, and the file may get left behind
    /// in an invalid state. Of course, this can only happen if
    /// read/write mode (mode_ReadWrite) was requested, and the file
    /// did not already exist.
    ///
    /// \param file File system path to a Realm database file.
    ///
    /// \param encryption_key 32-byte key used to encrypt and decrypt
    /// the database file, or nullptr to disable encryption.
    ///
    /// \param mode Specifying a mode that is not mode_ReadOnly
    /// requires that the specified file can be opened in read/write
    /// mode. In general there is no reason to open a group in
    /// read/write mode unless you want to be able to call
    /// Group::commit().
    ///
    /// \throw util::File::AccessError If the file could not be
    /// opened. If the reason corresponds to one of the exception
    /// types that are derived from util::File::AccessError, the
    /// derived exception type is thrown. Note that InvalidDatabase is
    /// among these derived exception types.
    void open(const std::string& file, const char* encryption_key = nullptr, OpenMode mode = mode_ReadOnly);

    /// Attach this Group instance to the specified memory buffer.
    ///
    /// This is similar to constructing a group from a file except
    /// that in this case the database is assumed to be stored in the
    /// specified memory buffer.
    ///
    /// If \a take_ownership is `true`, you pass the ownership of the
    /// specified buffer to the group. In this case the buffer will
    /// eventually be freed using std::free(), so the buffer you pass,
    /// must have been allocated using std::malloc().
    ///
    /// On the other hand, if \a take_ownership is set to `false`, it
    /// is your responsibility to keep the memory buffer alive during
    /// the lifetime of the group, and in case the buffer needs to be
    /// deallocated afterwards, that is your responsibility too.
    ///
    /// If this function throws, the ownership of the memory buffer
    /// will remain with the caller, regardless of whether \a
    /// take_ownership is set to `true` or `false`.
    ///
    /// Calling open() on a Group instance that is already in the
    /// attached state has undefined behavior.
    ///
    /// Do not call this function on a group instance that is managed
    /// by a shared group. Doing so will result in undefined behavior.
    ///
    /// \throw InvalidDatabase If the specified buffer does not appear
    /// to contain a valid database.
    void open(BinaryData, bool take_ownership = true);

    /// A group may be created in the unattached state, and then later
    /// attached to a file with a call to open(). Calling any method
    /// other than open(), and is_attached() on an unattached instance
    /// results in undefined behavior.
    bool is_attached() const noexcept;
<<<<<<< HEAD

=======
    /// A group is frozen only if it is actually a frozen transaction.
    virtual bool is_frozen() const noexcept { return false; }
>>>>>>> origin/develop12
    /// Returns true if, and only if the number of tables in this
    /// group is zero.
    bool is_empty() const noexcept;

<<<<<<< HEAD
    /// Returns the number of tables in this group.
    size_t size() const noexcept;

=======
    size_t size() const noexcept;

    int get_history_schema_version() noexcept;

    Replication* get_replication() const
    {
        return *get_repl();
    }

    /// The sync file id is set when a client synchronizes with the server for the
    /// first time. It is used when generating GlobalKeys for tables without a primary
    /// key, where it is used as the "hi" part. This ensures global uniqueness of
    /// GlobalKeys.
    uint64_t get_sync_file_id() const noexcept;
    void set_sync_file_id(uint64_t id);

    /// Returns the keys for all tables in this group.
    TableKeys get_table_keys() const;

>>>>>>> origin/develop12
    /// \defgroup group_table_access Table Accessors
    ///
    /// has_table() returns true if, and only if this group contains a table
    /// with the specified name.
    ///
<<<<<<< HEAD
    /// find_table() returns the index of the first table in this group with the
    /// specified name, or `realm::not_found` if this group does not contain a
    /// table with the specified name.
    ///
    /// get_table_name() returns the name of table at the specified index.
    ///
    /// The versions of get_table(), that accepts a \a name argument, return the
    /// first table with the specified name, or null if no such table exists.
=======
    /// find_table() returns the key of the first table in this group with the
    /// specified name, or `realm::not_found` if this group does not contain a
    /// table with the specified name.
    ///
    /// get_table_name() returns the name of table with the specified key.
    ///
    /// The versions of get_table(), that accepts a \a name argument, return a
    /// table with the specified name, or null if no such table exists.
>>>>>>> origin/develop12
    ///
    /// add_table() adds a table with the specified name to this group. It
    /// throws TableNameInUse if \a require_unique_name is true and \a name
    /// clashes with the name of an existing table. If \a require_unique_name is
    /// false, it is possible to add more than one table with the same
<<<<<<< HEAD
    /// name. Whenever a table is added, the order of the preexisting tables may
    /// change arbitrarily, and the new table may not end up as the last one
    /// either. But know that you can always call Table::get_index_in_group() on
    /// the returned table accessor to find out at which index it ends up.
=======
    /// name. Whenever a table is added the key assigned to it is returned.
>>>>>>> origin/develop12
    ///
    /// get_or_add_table() checks if a table exists in this group with the specified
    /// name. If it doesn't exist, a table is created.
    ///
<<<<<<< HEAD
    /// get_or_insert_table() works slightly differently from get_or_add_table(),
    /// in that it considers the position of the requested table as part of that
    /// table's identifying "key", in addition to the name.
    ///
    /// remove_table() removes the specified table from this group. A table can
    /// be removed only when it is not the target of a link column of a
    /// different table. Whenever a table is removed, the order of the remaining
    /// tables may change arbitrarily.
=======
    /// remove_table() removes the specified table from this group. A table can
    /// be removed only when it is not the target of a link column of a
    /// different table.
>>>>>>> origin/develop12
    ///
    /// rename_table() changes the name of a preexisting table. If \a
    /// require_unique_name is false, it becomes possible to have more than one
    /// table with a given name in a single group.
    ///
    /// The template functions work exactly like their non-template namesakes
    /// except as follows: The template versions of get_table() and
    /// get_or_add_table() throw DescriptorMismatch if the dynamic type of the
    /// specified table does not match the statically specified custom table
    /// type. The template versions of add_table() and get_or_add_table() set
    /// the dynamic type (descriptor) to match the statically specified custom
    /// table type.
    ///
<<<<<<< HEAD
    /// \param index Index of table in this group.
=======
    /// \param key Key of table in this group.
>>>>>>> origin/develop12
    ///
    /// \param name Name of table. All strings are valid table names as long as
    /// they are valid UTF-8 encodings and the number of bytes does not exceed
    /// `max_table_name_length`. A call to add_table() or get_or_add_table()
    /// with a name that is longer than `max_table_name_length` will cause an
    /// exception to be thrown.
    ///
    /// \param new_name New name for preexisting table.
    ///
    /// \param require_unique_name When set to true (the default), it becomes
    /// impossible to add a table with a name that is already in use, or to
    /// rename a table to a name that is already in use.
    ///
    /// \param was_added When specified, the boolean variable is set to true if
    /// the table was added, and to false otherwise. If the function throws, the
    /// boolean variable retains its original value.
    ///
    /// \return get_table(), add_table(), and get_or_add_table() return a table
    /// accessor attached to the requested (or added) table. get_table() may
    /// return null.
    ///
    /// \throw DescriptorMismatch Thrown by get_table() and get_or_add_table()
    /// tf the dynamic table type does not match the statically specified custom
    /// table type (\a T).
    ///
    /// \throw NoSuchTable Thrown by remove_table() and rename_table() if there
    /// is no table with the specified \a name.
    ///
    /// \throw TableNameInUse Thrown by add_table() if \a require_unique_name is
    /// true and \a name clashes with the name of a preexisting table. Thrown by
    /// rename_table() if \a require_unique_name is true and \a new_name clashes
    /// with the name of a preexisting table.
    ///
    /// \throw CrossTableLinkTarget Thrown by remove_table() if the specified
    /// table is the target of a link column of a different table.
    ///
    //@{

    static const size_t max_table_name_length = 63;

    bool has_table(StringData name) const noexcept;
<<<<<<< HEAD
    size_t find_table(StringData name) const noexcept;
    StringData get_table_name(size_t table_ndx) const;

    TableRef get_table(size_t index);
    ConstTableRef get_table(size_t index) const;
=======
    TableKey find_table(StringData name) const noexcept;
    StringData get_table_name(TableKey key) const;

    TableRef get_table(TableKey key);
    ConstTableRef get_table(TableKey key) const;

    // Catch some implicit conversions
    TableRef get_table(int) = delete;
    ConstTableRef get_table(int) const = delete;
>>>>>>> origin/develop12

    TableRef get_table(StringData name);
    ConstTableRef get_table(StringData name) const;

<<<<<<< HEAD
    TableRef add_table(StringData name, bool require_unique_name = true);
    TableRef insert_table(size_t index, StringData name, bool require_unique_name = true);
    TableRef get_or_add_table(StringData name, bool* was_added = nullptr);
    TableRef get_or_insert_table(size_t index, StringData name, bool* was_added = nullptr);

    void remove_table(size_t index);
    void remove_table(StringData name);

    void rename_table(size_t index, StringData new_name, bool require_unique_name = true);
=======
    TableRef add_table(StringData name);
    TableRef add_table_with_primary_key(StringData name, DataType pk_type, StringData pk_name, bool nullable = false);
    TableRef get_or_add_table(StringData name, bool* was_added = nullptr);

    void remove_table(TableKey key);
    void remove_table(StringData name);

    void rename_table(TableKey key, StringData new_name, bool require_unique_name = true);
>>>>>>> origin/develop12
    void rename_table(StringData name, StringData new_name, bool require_unique_name = true);

    //@}

    // Serialization

    /// Write this database to the specified output stream.
    ///
    /// \param out The destination output stream to write to.
    ///
    /// \param pad If true, the file is padded to ensure the footer is aligned
    /// to the end of a page
    void write(std::ostream& out, bool pad = false) const;

    /// Write this database to a new file. It is an error to specify a
    /// file that already exists. This is to protect against
    /// overwriting a database file that is currently open, which
    /// would cause undefined behaviour.
    ///
    /// \param file A filesystem path.
    ///
    /// \param encryption_key 32-byte key used to encrypt the database file,
    /// or nullptr to disable encryption.
    ///
    /// \param version If different from 0, the new file will be a full fledged
    /// realm file with free list and history info. The version of the commit
    /// will be set to the value given here.
    ///
    /// \throw util::File::AccessError If the file could not be
    /// opened. If the reason corresponds to one of the exception
    /// types that are derived from util::File::AccessError, the
    /// derived exception type is thrown. In particular,
    /// util::File::Exists will be thrown if the file exists already.
    void write(const std::string& file, const char* encryption_key = nullptr, uint64_t version = 0,
               bool write_history = true) const;

    /// Write this database to a memory buffer.
    ///
    /// Ownership of the returned buffer is transferred to the
    /// caller. The memory will have been allocated using
    /// std::malloc().
    BinaryData write_to_mem() const;

    /// Commit changes to the attached file. This requires that the
    /// attached file is opened in read/write mode.
    ///
    /// Calling this function on an unattached group, a free-standing
    /// group, a group whose attached file is opened in read-only
    /// mode, a group that is attached to a memory buffer, or a group
    /// that is managed by a shared group, is an error and will result
    /// in undefined behavior.
    ///
    /// Table accesors will remain valid across the commit. Note that
    /// this is not the case when working with proper transactions.
    void commit();

    //@{
    /// Some operations on Tables in a Group can cause indirect changes to other
    /// fields, including in other Tables in the same Group. Specifically,
    /// removing a row will set any links to that row to null, and if it had the
    /// last strong links to other rows, will remove those rows. When this
    /// happens, The cascade notification handler will be called with a
    /// CascadeNotification containing information about what indirect changes
    /// will occur, before any changes are made.
    ///
    /// has_cascade_notification_handler() returns true if and only if there is
    /// currently a non-null notification handler registered.
    ///
    /// set_cascade_notification_handler() replaces the current handler (if any)
    /// with the passed in handler. Pass in nullptr to remove the current handler
    /// without registering a new one.
    ///
    /// CascadeNotification contains a vector of rows which will be removed and
    /// a vector of links which will be set to null (or removed, for entries in
    /// LinkLists).
    struct CascadeNotification {
        struct row {
<<<<<<< HEAD
            /// Non-zero iff the removal of this row is ordered
            /// (Table::remove()), as opposed to ordered
            /// (Table::move_last_over()). Implicit removals are always
            /// unordered.
            ///
            /// This flag does not take part in comparisons (operator==() and
            /// operator<()).
            size_t is_ordered_removal : 1;

            /// Index within group of a group-level table.
            size_t table_ndx : std::numeric_limits<size_t>::digits - 1;

            /// Row index which will be removed.
            size_t row_ndx;

            row()
                : is_ordered_removal(0)
            {
            }

            bool operator==(const row&) const noexcept;
            bool operator!=(const row&) const noexcept;

            /// Trivial lexicographic order
            bool operator<(const row&) const noexcept;
        };

        struct link {
            const Table* origin_table; ///< A group-level table.
            size_t origin_col_ndx;     ///< Link column being nullified.
            size_t origin_row_ndx;     ///< Row in column being nullified.
            /// The target row index which is being removed. Mostly relevant for
            /// LinkList (to know which entries are being removed), but also
            /// valid for Link.
            size_t old_target_row_ndx;
=======
            /// Key identifying a group-level table.
            TableKey table_key;

            /// Key identifying object to be removed.
            ObjKey key;

            row() = default;

            row(TableKey tk, ObjKey k)
                : table_key(tk)
                , key(k)
            {
            }
            bool operator==(const row& r) const noexcept
            {
                return table_key == r.table_key && key == r.key;
            }
            bool operator!=(const row& r) const noexcept
            {
                return !(*this == r);
            }
            /// Trivial lexicographic order
            bool operator<(const row& r) const noexcept
            {
                return table_key < r.table_key || (table_key == r.table_key && key < r.key);
            }
        };

        struct link {
            link() = default;
            link(TableKey tk, ColKey ck, ObjKey k, ObjKey otk)
                : origin_table(tk)
                , origin_col_key(ck)
                , origin_key(k)
                , old_target_key(otk)
            {
            }
            TableKey origin_table;     ///< A group-level table.
            ColKey origin_col_key;     ///< Link column being nullified.
            ObjKey origin_key;         ///< Row in column being nullified.
            /// The target row index which is being removed. Mostly relevant for
            /// LinkList (to know which entries are being removed), but also
            /// valid for Link.
            ObjKey old_target_key;
>>>>>>> origin/develop12
        };

        /// A sorted list of rows which will be removed by the current operation.
        std::vector<row> rows;

        /// An unordered list of links which will be nullified by the current
        /// operation.
        std::vector<link> links;
    };

    bool has_cascade_notification_handler() const noexcept;
    void set_cascade_notification_handler(std::function<void(const CascadeNotification&)> new_handler) noexcept;

    //@}

    //@{
    /// During sync operation, schema changes may happen at runtime as connected
    /// clients update their schema as part of an app update. Since this is a
    /// relatively rare event, no attempt is made at limiting the amount of work
    /// the handler is required to do to update its information about table and
    /// column indices (i.e., all table and column indices must be recalculated).
    ///
    /// At the time of writing, only additive schema changes may occur in that
    /// scenario.
    ///
    /// has_schema_change_notification_handler() returns true iff there is currently
    /// a non-null notification handler registered.
    ///
    /// set_schema_change_notification_handler() replaces the current handler (if any)
    /// with the passed in handler. Pass in nullptr to remove the current handler
    /// without registering a new one.

    bool has_schema_change_notification_handler() const noexcept;
    void set_schema_change_notification_handler(std::function<void()> new_handler) noexcept;

    //@}

    // Conversion
    template <class S>
    void to_json(S& out, size_t link_depth = 0, std::map<std::string, std::string>* renames = nullptr) const;
<<<<<<< HEAD
    void to_string(std::ostream& out) const;
=======
>>>>>>> origin/develop12

    /// Compare two groups for equality. Two groups are equal if, and
    /// only if, they contain the same tables in the same order, that
    /// is, for each table T at index I in one of the groups, there is
    /// a table at index I in the other group that is equal to T.
    /// Tables are equal if they have the same content and the same table name.
    bool operator==(const Group&) const;

    /// Compare two groups for inequality. See operator==().
    bool operator!=(const Group& g) const
    {
        return !(*this == g);
    }

    /// Control of what to include when computing memory usage
    enum SizeAggregateControl {
<<<<<<< HEAD
        size_of_state = 1, ///< size of tables, indexes, toplevel array
        size_of_history = 2, ///< size of the in-file history compartment
=======
        size_of_state = 1,     ///< size of tables, indexes, toplevel array
        size_of_history = 2,   ///< size of the in-file history compartment
>>>>>>> origin/develop12
        size_of_freelists = 4, ///< size of the freelists
        size_of_all = 7
    };
    /// Compute the sum of the sizes in number of bytes of all the array nodes
    /// that currently make up this group. When this group represents a snapshot
    /// in a Realm file (such as during a read transaction via a SharedGroup
    /// instance), this function computes the footprint of that snapshot within
    /// the Realm file.
    ///
    /// If this group accessor is the detached state, this function returns
    /// zero.
    size_t compute_aggregated_byte_size(SizeAggregateControl ctrl = SizeAggregateControl::size_of_all) const noexcept;
    /// Return the size taken up by the current snapshot. This is in contrast to
    /// the number returned by SharedGroup::get_stats() which will return the
    /// size of the last snapshot done in that SharedGroup. If the snapshots are
    /// identical, the numbers will of course be equal.
    size_t get_used_space() const noexcept;

    void verify() const;
<<<<<<< HEAD
=======
    void validate_primary_columns();
>>>>>>> origin/develop12
#ifdef REALM_DEBUG
    void print() const;
    void print_free() const;
    MemStats get_stats();
    void enable_mem_diagnostics(bool enable = true)
    {
        m_alloc.enable_debug(enable);
    }
<<<<<<< HEAD
    void to_dot(std::ostream&) const;
    void to_dot() const; // To std::cerr (for GDB)
    void to_dot(const char* file_path) const;
#endif

private:
    SlabAlloc m_alloc;
=======
#endif

protected:
    virtual Replication* const* get_repl() const
    {
        return &Table::g_dummy_replication;
    }

private:
    static constexpr char g_class_name_prefix[] = "class_";
    static constexpr size_t g_class_name_prefix_len = 6;

    // nullptr, if we're sharing an allocator provided during initialization
    std::unique_ptr<SlabAlloc> m_local_alloc;
    // in-use allocator. If local, then equal to m_local_alloc.
    SlabAlloc& m_alloc;
>>>>>>> origin/develop12

    int m_file_format_version;
    /// `m_top` is the root node (or top array) of the Realm, and has the
    /// following layout:
    ///
    /// <pre>
    ///
    ///                                                     Introduced in file
    ///   Slot  Value                                       format version
    ///   ---------------------------------------------------------------------
    ///    1st   m_table_names
    ///    2nd   m_tables
    ///    3rd   Logical file size
    ///    4th   GroupWriter::m_free_positions (optional)
    ///    5th   GroupWriter::m_free_lengths   (optional)
    ///    6th   GroupWriter::m_free_versions  (optional)
    ///    7th   Transaction number / version  (optional)
    ///    8th   History type         (optional)             4
    ///    9th   History ref          (optional)             4
    ///   10th   History version      (optional)             7
<<<<<<< HEAD
=======
    ///   11th   Sync File Id         (optional)            10
>>>>>>> origin/develop12
    ///
    /// </pre>
    ///
    /// The 'History type' slot stores a value of type
    /// Replication::HistoryType. The 'History version' slot stores a history
    /// schema version as returned by Replication::get_history_schema_version().
    ///
    /// The first three entries are mandatory. In files created by
    /// Group::write(), none of the optional entries are present and the size of
    /// `m_top` is 3. In files updated by Group::commit(), the 4th and 5th entry
    /// are present, and the size of `m_top` is 5. In files updated by way of a
    /// transaction (SharedGroup::commit()), the 4th, 5th, 6th, and 7th entry
    /// are present, and the size of `m_top` is 7. In files that contain a
<<<<<<< HEAD
    /// changeset history, the 8th, 9th, and 10th entry are present, except that
    /// if the file was opened in nonshared mode (via Group::open()), and the
    /// file format remains at 6 (not previously upgraded to 7 or later), then
    /// the 10th entry will be absent.
=======
    /// changeset history, the 8th, 9th, and 10th entry are present. The 11th entry
    /// will be present if the file is syncked and the client has received a client
    /// file id from the server.
>>>>>>> origin/develop12
    ///
    /// When a group accessor is attached to a newly created file or an empty
    /// memory buffer where there is no top array yet, `m_top`, `m_tables`, and
    /// `m_table_names` will be left in the detached state until the initiation
    /// of the first write transaction. In particular, they will remain in the
    /// detached state during read transactions that precede the first write
    /// transaction.
    Array m_top;
<<<<<<< HEAD
    ArrayInteger m_tables;
    ArrayString m_table_names;

    typedef std::vector<Table*> table_accessors;
    mutable table_accessors m_table_accessors;

    bool m_attached = false;
=======
    Array m_tables;
    ArrayStringShort m_table_names;
    uint64_t m_last_seen_mapping_version = 0;

    typedef std::vector<Table*> TableAccessors;
    mutable TableAccessors m_table_accessors;
    mutable std::mutex m_accessor_mutex;
    mutable int m_num_tables = 0;
    bool m_attached = false;
    bool m_is_writable = true;
>>>>>>> origin/develop12
    const bool m_is_shared;

    std::function<void(const CascadeNotification&)> m_notify_handler;
    std::function<void()> m_schema_change_handler;
    std::shared_ptr<metrics::Metrics> m_metrics;
    size_t m_total_rows;

<<<<<<< HEAD
=======
    class TableRecycler : public std::vector<Table*> {
    public:
        ~TableRecycler()
        {
            for (auto t : *this) {
                delete t;
            }
        }
    };

    static constexpr size_t s_table_name_ndx = 0;
    static constexpr size_t s_table_refs_ndx = 1;
    static constexpr size_t s_file_size_ndx = 2;
    static constexpr size_t s_free_pos_ndx = 3;
    static constexpr size_t s_free_size_ndx = 4;
    static constexpr size_t s_free_version_ndx = 5;
    static constexpr size_t s_version_ndx = 6;
    static constexpr size_t s_hist_type_ndx = 7;
    static constexpr size_t s_hist_ref_ndx = 8;
    static constexpr size_t s_hist_version_ndx = 9;
    static constexpr size_t s_sync_file_id_ndx = 10;

    static constexpr size_t s_group_max_size = 11;

    // We use the classic approach to construct a FIFO from two LIFO's,
    // insertion is done into recycler_1, removal is done from recycler_2,
    // and when recycler_2 is empty, recycler_1 is reversed into recycler_2.
    // this i O(1) for each entry.
    static TableRecycler g_table_recycler_1;
    static TableRecycler g_table_recycler_2;
    // number of tables held back before being recycled. We hold back recycling
    // the latest to increase the probability of detecting race conditions
    // without crashing.
    const static int g_table_recycling_delay = 100;
    static std::mutex g_table_recycler_mutex;

>>>>>>> origin/develop12
    struct shared_tag {
    };
    Group(shared_tag) noexcept;

<<<<<<< HEAD
=======
    Group(SlabAlloc* alloc) noexcept;
>>>>>>> origin/develop12
    void init_array_parents() noexcept;

    void open(ref_type top_ref, const std::string& file_path);

<<<<<<< HEAD
    /// If `top_ref` is not zero, attach this group accessor to the specified
    /// underlying node structure. If `top_ref` is zero and \a
    /// create_group_when_missing is true, create a new node structure that
    /// represents an empty group, and attach this group accessor to it. It is
    /// an error to call this function on an already attached group accessor.
    void attach(ref_type top_ref, bool create_group_when_missing);
=======
    // If the underlying memory mappings have been extended, this method is used
    // to update all the tables' allocator wrappers. The allocator wrappers are
    // configure to either allow or deny changes.
    void update_allocator_wrappers(bool writable);

    /// If `top_ref` is not zero, attach this group accessor to the specified
    /// underlying node structure. If `top_ref` is zero and \a
    /// create_group_when_missing is true, create a new node structure that
    /// represents an empty group, and attach this group accessor to it.
    void attach(ref_type top_ref, bool writable, bool create_group_when_missing);
>>>>>>> origin/develop12

    /// Detach this group accessor from the underlying node structure. If this
    /// group accessors is already in the detached state, this function does
    /// nothing (idempotency).
    void detach() noexcept;

    /// \param writable Must be set to true when, and only when attaching for a
    /// write transaction.
    void attach_shared(ref_type new_top_ref, size_t new_file_size, bool writable);

    void create_empty_group();
<<<<<<< HEAD
=======
    void remove_table(size_t table_ndx, TableKey key);
>>>>>>> origin/develop12

    void reset_free_space_tracking();

    void remap(size_t new_file_size);
<<<<<<< HEAD
    void remap_and_update_refs(ref_type new_top_ref, size_t new_file_size);
=======
    void remap_and_update_refs(ref_type new_top_ref, size_t new_file_size, bool writable);
>>>>>>> origin/develop12

    /// Recursively update refs stored in all cached array
    /// accessors. This includes cached array accessors in any
    /// currently attached table accessors. This ensures that the
    /// group instance itself, as well as any attached table accessor
    /// that exists across Group::commit() will remain valid. This
    /// function is not appropriate for use in conjunction with
    /// commits via shared group.
    void update_refs(ref_type top_ref, size_t old_baseline) noexcept;

    // Overriding method in ArrayParent
    void update_child_ref(size_t, ref_type) override;

    // Overriding method in ArrayParent
    ref_type get_child_ref(size_t) const noexcept override;

<<<<<<< HEAD
    // Overriding method in Table::Parent
    StringData get_child_name(size_t) const noexcept override;

    // Overriding method in Table::Parent
    void child_accessor_destroyed(Table*) noexcept override;

    // Overriding method in Table::Parent
    std::recursive_mutex* get_accessor_management_lock() noexcept override
    { return nullptr; } // we don't need locking for group!

    // Overriding method in Table::Parent
    Group* get_parent_group() noexcept override;

=======
>>>>>>> origin/develop12
    class TableWriter;
    class DefaultTableWriter;

    static void write(std::ostream&, int file_format_version, TableWriter&, bool no_top_array,
                      bool pad_for_encryption, uint_fast64_t version_number);

<<<<<<< HEAD
    typedef void (*DescSetter)(Table&);
    typedef bool (*DescMatcher)(const Spec&);

    Table* do_get_table(size_t table_ndx, DescMatcher desc_matcher);
    const Table* do_get_table(size_t table_ndx, DescMatcher desc_matcher) const;
    Table* do_get_table(StringData name, DescMatcher desc_matcher);
    const Table* do_get_table(StringData name, DescMatcher desc_matcher) const;
    Table* do_insert_table(size_t, StringData name, DescSetter desc_setter, bool require_unique_name);
    Table* do_insert_table(size_t, StringData name, DescSetter desc_setter);
    Table* do_get_or_add_table(StringData name, DescMatcher desc_matcher, DescSetter setter, bool* was_added);
    Table* do_get_or_insert_table(size_t, StringData name, DescMatcher desc_matcher, DescSetter desc_setter,
                                  bool* was_added);

    void create_and_insert_table(size_t new_table_ndx, StringData name);
    Table* create_table_accessor(size_t table_ndx);
=======
    Table* do_get_table(size_t ndx);
    const Table* do_get_table(size_t ndx) const;
    Table* do_get_table(StringData name);
    const Table* do_get_table(StringData name) const;
    Table* do_add_table(StringData name);

    Table* do_get_or_add_table(StringData name, bool* was_added);

    void create_and_insert_table(TableKey key, StringData name);
    Table* create_table_accessor(size_t table_ndx);
    void recycle_table_accessor(Table*);
>>>>>>> origin/develop12

    void detach_table_accessors() noexcept; // Idempotent

    void mark_all_table_accessors() noexcept;

    void write(util::File& file, const char* encryption_key, uint_fast64_t version_number, bool write_history) const;
    void write(std::ostream&, bool pad, uint_fast64_t version_numer, bool write_history) const;

<<<<<<< HEAD
    Replication* get_replication() const noexcept;
    void set_replication(Replication*) noexcept;
=======
>>>>>>> origin/develop12
    std::shared_ptr<metrics::Metrics> get_metrics() const noexcept;
    void set_metrics(std::shared_ptr<metrics::Metrics> other) noexcept;
    void update_num_objects();
    class TransactAdvancer;
<<<<<<< HEAD
    void advance_transact(ref_type new_top_ref, size_t new_file_size, _impl::NoCopyInputStream&);
    void refresh_dirty_accessors();
    template <class F>
    void update_table_indices(F&& map_function);
=======
    void advance_transact(ref_type new_top_ref, size_t new_file_size, _impl::NoCopyInputStream&, bool writable);
    void refresh_dirty_accessors();
    void flush_accessors_for_commit();
>>>>>>> origin/develop12

    /// \brief The version of the format of the node structure (in file or in
    /// memory) in use by Realm objects associated with this group.
    ///
    /// Every group contains a file format version field, which is returned
    /// by this function. The file format version field is set to the file format
    /// version specified by the attached file (or attached memory buffer) at the
    /// time of attachment and the value is used to determine if a file format
    /// upgrade is required.
    ///
    /// A value of zero means that the file format is not yet decided. This is
    /// only possible for empty Realms where top-ref is zero. (When group is created
    /// with the unattached_tag). The version number will then be determined in the
    /// subsequent call to Group::open.
    ///
    /// In shared mode (when a Realm file is opened via a SharedGroup instance)
    /// it can happen that the file format is upgraded asyncronously (via
    /// another SharedGroup instance), and in that case the file format version
    /// field can get out of date, but only for a short while. It is always
    /// guaranteed to be, and remain up to date after the opening process completes
    /// (when SharedGroup::do_open() returns).
    ///
    /// An empty Realm file (one whose top-ref is zero) may specify a file
    /// format version of zero to indicate that the format is not yet
    /// decided. In that case the file format version must be changed to a proper
    /// before the opening process completes (Group::open() or SharedGroup::open()).
    ///
    /// File format versions:
    ///
    ///   1 Initial file format version
    ///
    ///   2 Various changes.
    ///
    ///   3 Supporting null on string columns broke the file format in following
    ///     way: Index appends an 'X' character to all strings except the null
    ///     string, to be able to distinguish between null and empty
    ///     string. Bumped to 3 because of null support of String columns and
    ///     because of new format of index.
    ///
    ///   4 Introduction of optional in-Realm history of changes (additional
    ///     entries in Group::m_top). Since this change is not forward
    ///     compatible, the file format version had to be bumped. This change is
    ///     implemented in a way that achieves backwards compatibility with
    ///     version 3 (and in turn with version 2).
    ///
    ///   5 Introduced the new Timestamp column type that replaces DateTime.
    ///     When opening an older database file, all DateTime columns will be
    ///     automatically upgraded Timestamp columns.
    ///
    ///   6 Introduced a new structure for the StringIndex. Moved the commit
    ///     logs into the Realm file. Changes to the transaction log format
    ///     including reshuffling instructions. This is the format used in
    ///     milestone 2.0.0.
    ///
    ///   7 Introduced "history schema version" as 10th entry in top array.
    ///
    ///   8 Subtables can now have search index.
    ///
    ///   9 Replication instruction values shuffled, instr_MoveRow added.
    ///
<<<<<<< HEAD
=======
    ///  10 Cluster based table layout. Memory mapping changes which require
    ///     special treatment of large files of preceeding versions.
    ///
    ///  11 Same as 10, but version 10 files will have search index added on
    ///     string primary key columns.
    ///
>>>>>>> origin/develop12
    /// IMPORTANT: When introducing a new file format version, be sure to review
    /// the file validity checks in Group::open() and SharedGroup::do_open, the file
    /// format selection logic in
    /// Group::get_target_file_format_version_for_session(), and the file format
    /// upgrade logic in Group::upgrade_file_format().

    int get_file_format_version() const noexcept;
    void set_file_format_version(int) noexcept;
    int get_committed_file_format_version() const noexcept;

    /// The specified history type must be a value of Replication::HistoryType.
    static int get_target_file_format_version_for_session(int current_file_format_version, int history_type) noexcept;

<<<<<<< HEAD
    /// Must be called from within a write transaction
    void upgrade_file_format(int target_file_format_version);

=======
>>>>>>> origin/develop12
    std::pair<ref_type, size_t> get_to_dot_parent(size_t ndx_in_parent) const override;

    void send_cascade_notification(const CascadeNotification& notification) const;
    void send_schema_change_notification() const;

    static void get_version_and_history_info(const Array& top, _impl::History::version_type& version,
                                             int& history_type, int& history_schema_version) noexcept;
    static ref_type get_history_ref(const Array& top) noexcept;
<<<<<<< HEAD
    static int get_history_schema_version(const Array& top) noexcept;
    void set_history_schema_version(int version);
    void set_history_parent(Array& history_root) noexcept;
    void prepare_history_parent(Array& history_root, int history_type, int history_schema_version);
    static void validate_top_array(const Array& arr, const SlabAlloc& alloc);

    friend class Table;
    friend class GroupWriter;
    friend class SharedGroup;
    friend class _impl::GroupFriend;
    friend class _impl::TransactLogConvenientEncoder;
    friend class _impl::TransactLogParser;
    friend class Replication;
    friend class TrivialReplication;
    friend class metrics::QueryInfo;
    friend class metrics::Metrics;
};


// Implementation

inline Group::Group(const std::string& file, const char* key, OpenMode mode)
    : m_alloc() // Throws
    , m_top(m_alloc)
    , m_tables(m_alloc)
    , m_table_names(m_alloc)
    , m_is_shared(false)
    , m_total_rows(0)
{
    init_array_parents();

    open(file, key, mode); // Throws
}

inline Group::Group(BinaryData buffer, bool take_ownership)
    : m_alloc() // Throws
    , m_top(m_alloc)
    , m_tables(m_alloc)
    , m_table_names(m_alloc)
    , m_is_shared(false)
    , m_total_rows(0)
{
    init_array_parents();
    open(buffer, take_ownership); // Throws
}

inline Group::Group(unattached_tag) noexcept
    : m_alloc()
    , // Throws
    m_top(m_alloc)
    , m_tables(m_alloc)
    , m_table_names(m_alloc)
    , m_is_shared(false)
    , m_total_rows(0)
{
    init_array_parents();
}

inline Group* Group::get_parent_group() noexcept
{
    return this;
}

inline Group::Group(shared_tag) noexcept
    : m_alloc()
    , // Throws
    m_top(m_alloc)
    , m_tables(m_alloc)
    , m_table_names(m_alloc)
    , m_is_shared(true)
    , m_total_rows(0)
{
    init_array_parents();
=======
    void set_history_schema_version(int version);
    template <class Accessor>
    void set_history_parent(Accessor& history_root) noexcept;
    void prepare_top_for_history(int history_type, int history_schema_version, uint64_t file_ident);
    template <class Accessor>
    void prepare_history_parent(Accessor& history_root, int history_type, int history_schema_version,
                                uint64_t file_ident);
    static void validate_top_array(const Array& arr, const SlabAlloc& alloc);

    size_t find_table_index(StringData name) const noexcept;
    TableKey ndx2key(size_t ndx) const;
    size_t key2ndx(TableKey key) const;
    size_t key2ndx_checked(TableKey key) const;
    void set_size() const noexcept;
    std::map<TableRef, ColKey> get_primary_key_columns_from_pk_table(TableRef pk_table);
    void check_table_name_uniqueness(StringData name)
    {
        if (m_table_names.find_first(name) != not_found)
            throw TableNameInUse();
    }

    friend class Table;
    friend class GroupWriter;
    friend class DB;
    friend class _impl::GroupFriend;
    friend class _impl::TransactLogConvenientEncoder;
    friend class _impl::TransactLogParser;
    friend class TrivialReplication;
    friend class metrics::QueryInfo;
    friend class metrics::Metrics;
    friend class Transaction;
    friend class TableKeyIterator;
    friend class CascadeState;
};

class TableKeyIterator {
public:
    bool operator!=(const TableKeyIterator& other)
    {
        return m_pos != other.m_pos;
    }
    TableKeyIterator& operator++();
    TableKey operator*();

private:
    friend class TableKeys;
    const Group* m_group;
    size_t m_pos;
    size_t m_index_in_group = 0;
    TableKey m_table_key;

    TableKeyIterator(const Group* g, size_t p)
        : m_group(g)
        , m_pos(p)
    {
    }
    void load_key();
};

class TableKeys {
public:
    TableKeys(const Group* g)
        : m_iter(g, 0)
    {
    }
    size_t size() const
    {
        return m_iter.m_group->size();
    }
    bool empty() const
    {
        return size() == 0;
    }
    TableKey operator[](size_t p) const;
    TableKeyIterator begin() const
    {
        return TableKeyIterator(m_iter.m_group, 0);
    }
    TableKeyIterator end() const
    {
        return TableKeyIterator(m_iter.m_group, size());
    }

private:
    mutable TableKeyIterator m_iter;
};

// Implementation

inline TableKeys Group::get_table_keys() const
{
    return TableKeys(this);
>>>>>>> origin/develop12
}

inline bool Group::is_attached() const noexcept
{
    return m_attached;
}

inline bool Group::is_empty() const noexcept
{
    if (!is_attached())
        return false;
<<<<<<< HEAD
    if (m_table_names.is_attached())
        return m_table_names.is_empty();
    return true;
}

inline size_t Group::size() const noexcept
{
    if (!is_attached())
        return 0;
    if (m_table_names.is_attached())
        return m_table_names.size();
    return 0;
}

inline StringData Group::get_table_name(size_t table_ndx) const
{
    if (table_ndx >= size())
        throw LogicError(LogicError::table_index_out_of_range);
=======
    return size() == 0;
}

inline size_t Group::key2ndx(TableKey key) const
{
    size_t idx = key.value & 0xFFFF;
    return idx;
}

inline StringData Group::get_table_name(TableKey key) const
{
    size_t table_ndx = key2ndx_checked(key);
>>>>>>> origin/develop12
    return m_table_names.get(table_ndx);
}

inline bool Group::has_table(StringData name) const noexcept
{
<<<<<<< HEAD
    size_t ndx = find_table(name);
    return ndx != not_found;
}

inline size_t Group::find_table(StringData name) const noexcept
{
    if (!is_attached())
        return 0;
=======
    size_t ndx = find_table_index(name);
    return ndx != not_found;
}

inline size_t Group::find_table_index(StringData name) const noexcept
{
>>>>>>> origin/develop12
    if (m_table_names.is_attached())
        return m_table_names.find_first(name);
    return not_found;
}

<<<<<<< HEAD
inline TableRef Group::get_table(size_t table_ndx)
{
    if (!is_attached())
        throw LogicError(LogicError::detached_accessor);
    DescMatcher desc_matcher = nullptr;                   // Do not check descriptor
    Table* table = do_get_table(table_ndx, desc_matcher); // Throws
    return TableRef(table);
}

inline ConstTableRef Group::get_table(size_t table_ndx) const
{
    if (!is_attached())
        throw LogicError(LogicError::detached_accessor);
    DescMatcher desc_matcher = nullptr;                         // Do not check descriptor
    const Table* table = do_get_table(table_ndx, desc_matcher); // Throws
    return ConstTableRef(table);
}

inline TableRef Group::get_table(StringData name)
{
    if (!is_attached())
        throw LogicError(LogicError::detached_accessor);
    DescMatcher desc_matcher = nullptr;              // Do not check descriptor
    Table* table = do_get_table(name, desc_matcher); // Throws
    return TableRef(table);
}

inline ConstTableRef Group::get_table(StringData name) const
{
    if (!is_attached())
        throw LogicError(LogicError::detached_accessor);
    DescMatcher desc_matcher = nullptr;                    // Do not check descriptor
    const Table* table = do_get_table(name, desc_matcher); // Throws
    return ConstTableRef(table);
}

inline TableRef Group::insert_table(size_t table_ndx, StringData name, bool require_unique_name)
{
    if (!is_attached())
        throw LogicError(LogicError::detached_accessor);
    DescSetter desc_setter = nullptr;                                                  // Do not add any columns
    Table* table = do_insert_table(table_ndx, name, desc_setter, require_unique_name); // Throws
    return TableRef(table);
}

inline TableRef Group::add_table(StringData name, bool require_unique_name)
{
    return insert_table(size(), name, require_unique_name);
}

inline TableRef Group::get_or_insert_table(size_t table_ndx, StringData name, bool* was_added)
{
    if (!is_attached())
        throw LogicError(LogicError::detached_accessor);
    DescMatcher desc_matcher = nullptr; // Do not check descriptor
    DescSetter desc_setter = nullptr;   // Do not add any columns
    Table* table = do_get_or_insert_table(table_ndx, name, desc_matcher, desc_setter, was_added); // Throws
    return TableRef(table);
=======
inline TableKey Group::find_table(StringData name) const noexcept
{
    if (!is_attached())
        return TableKey();
    size_t ndx = find_table_index(name);
    return (ndx != npos) ? ndx2key(ndx) : TableKey{};
}

inline TableRef Group::get_table(TableKey key)
{
    if (!is_attached())
        throw LogicError(LogicError::detached_accessor);
    auto ndx = key2ndx_checked(key);
    Table* table = do_get_table(ndx); // Throws
    return TableRef(table, table ? table->m_alloc.get_instance_version() : 0);
}

inline ConstTableRef Group::get_table(TableKey key) const
{
    if (!is_attached())
        throw LogicError(LogicError::detached_accessor);
    auto ndx = key2ndx_checked(key);
    const Table* table = do_get_table(ndx); // Throws
    return ConstTableRef(table, table ? table->m_alloc.get_instance_version() : 0);
}

inline TableRef Group::get_table(StringData name)
{
    if (!is_attached())
        throw LogicError(LogicError::detached_accessor);
    Table* table = do_get_table(name); // Throws
    return TableRef(table, table ? table->m_alloc.get_instance_version() : 0);
}

inline ConstTableRef Group::get_table(StringData name) const
{
    if (!is_attached())
        throw LogicError(LogicError::detached_accessor);
    const Table* table = do_get_table(name); // Throws
    return ConstTableRef(table, table ? table->m_alloc.get_instance_version() : 0);
}

inline TableRef Group::add_table(StringData name)
{
    if (!is_attached())
        throw LogicError(LogicError::detached_accessor);
    check_table_name_uniqueness(name);
    Table* table = do_add_table(name); // Throws
    return TableRef(table, table ? table->m_alloc.get_instance_version() : 0);
>>>>>>> origin/develop12
}

inline TableRef Group::get_or_add_table(StringData name, bool* was_added)
{
    if (!is_attached())
        throw LogicError(LogicError::detached_accessor);
<<<<<<< HEAD
    DescMatcher desc_matcher = nullptr;                                             // Do not check descriptor
    DescSetter desc_setter = nullptr;                                               // Do not add any columns
    Table* table = do_get_or_add_table(name, desc_matcher, desc_setter, was_added); // Throws
    return TableRef(table);
=======
    Table* table = do_get_or_add_table(name, was_added); // Throws
    return TableRef(table, table ? table->m_alloc.get_instance_version() : 0);
>>>>>>> origin/develop12
}

template <class S>
void Group::to_json(S& out, size_t link_depth, std::map<std::string, std::string>* renames) const
{
    if (!is_attached())
        throw LogicError(LogicError::detached_accessor);

    std::map<std::string, std::string> renames2;
    renames = renames ? renames : &renames2;

    out << "{" << std::endl;

<<<<<<< HEAD
    for (size_t i = 0; i < m_tables.size(); ++i) {
        StringData name = m_table_names.get(i);
=======
    auto keys = get_table_keys();
    for (size_t i = 0; i < keys.size(); ++i) {
        auto key = keys[i];
        StringData name = get_table_name(key);
>>>>>>> origin/develop12
        std::map<std::string, std::string>& m = *renames;
        if (m[name] != "")
            name = m[name];

<<<<<<< HEAD
        ConstTableRef table = get_table(i);
=======
        ConstTableRef table = get_table(key);
>>>>>>> origin/develop12

        if (i)
            out << ",";
        out << "\"" << name << "\"";
        out << ":";
        table->to_json(out, link_depth, renames);
        out << std::endl;
    }

    out << "}" << std::endl;
}

inline void Group::init_array_parents() noexcept
{
    m_table_names.set_parent(&m_top, 0);
    m_tables.set_parent(&m_top, 1);
}

inline void Group::update_child_ref(size_t child_ndx, ref_type new_ref)
{
    m_tables.set(child_ndx, new_ref);
}

inline ref_type Group::get_child_ref(size_t child_ndx) const noexcept
{
    return m_tables.get_as_ref(child_ndx);
}

<<<<<<< HEAD
inline StringData Group::get_child_name(size_t child_ndx) const noexcept
{
    return m_table_names.get(child_ndx);
}

inline void Group::child_accessor_destroyed(Table*) noexcept
{
    // Ignore
}

=======
>>>>>>> origin/develop12
inline bool Group::has_cascade_notification_handler() const noexcept
{
    return !!m_notify_handler;
}

inline void
Group::set_cascade_notification_handler(std::function<void(const CascadeNotification&)> new_handler) noexcept
{
    m_notify_handler = std::move(new_handler);
}

inline void Group::send_cascade_notification(const CascadeNotification& notification) const
{
<<<<<<< HEAD
    if (m_notify_handler)
        m_notify_handler(notification);
=======
    REALM_ASSERT_DEBUG(m_notify_handler);
    m_notify_handler(notification);
>>>>>>> origin/develop12
}

inline bool Group::has_schema_change_notification_handler() const noexcept
{
    return !!m_schema_change_handler;
}

inline void Group::set_schema_change_notification_handler(std::function<void()> new_handler) noexcept
{
    m_schema_change_handler = std::move(new_handler);
}

inline void Group::send_schema_change_notification() const
{
    if (m_schema_change_handler)
        m_schema_change_handler();
}

<<<<<<< HEAD
inline void Group::get_version_and_history_info(const Array& top, _impl::History::version_type& version,
                                                int& history_type, int& history_schema_version) noexcept
{
    using version_type = _impl::History::version_type;
    version_type version_2 = 0;
    int history_type_2 = 0;
    int history_schema_version_2 = 0;
    if (top.is_attached()) {
        if (top.size() >= 6) {
            REALM_ASSERT(top.size() >= 7);
            version_2 = version_type(top.get_as_ref_or_tagged(6).get_as_int());
        }
        if (top.size() >= 8) {
            REALM_ASSERT(top.size() >= 9);
            history_type_2           = int(top.get_as_ref_or_tagged(7).get_as_int());
        }
        if (top.size() >= 10) {
            history_schema_version_2 = int(top.get_as_ref_or_tagged(9).get_as_int());
        }
    }
    // Version 0 is not a legal initial version, so it has to be set to 1
    // instead.
    if (version_2 == 0)
        version_2 = 1;
    version = version_2;
    history_type = history_type_2;
    history_schema_version = history_schema_version_2;
}

inline ref_type Group::get_history_ref(const Array& top) noexcept
{
    bool has_history = (top.is_attached() && top.size() >= 8);
    if (has_history) {
        // This function is only used is shared mode (from SharedGroup)
        REALM_ASSERT(top.size() >= 10);
        return top.get_as_ref(8);
=======
inline ref_type Group::get_history_ref(const Array& top) noexcept
{
    bool has_history = (top.is_attached() && top.size() > s_hist_type_ndx);
    if (has_history) {
        // This function is only used is shared mode (from SharedGroup)
        REALM_ASSERT(top.size() > s_hist_version_ndx);
        return top.get_as_ref(s_hist_ref_ndx);
>>>>>>> origin/develop12
    }
    return 0;
}

<<<<<<< HEAD
inline int Group::get_history_schema_version(const Array& top) noexcept
{
    bool has_history = (top.is_attached() && top.size() >= 8);
    if (has_history) {
        // This function is only used is shared mode (from SharedGroup)
        REALM_ASSERT(top.size() >= 10);
        return int(top.get_as_ref_or_tagged(9).get_as_int());
    }
    return 0;
=======
inline void Group::set_sync_file_id(uint64_t id)
{
    while (m_top.size() < s_sync_file_id_ndx + 1)
        m_top.add(0);
    m_top.set(s_sync_file_id_ndx, RefOrTagged::make_tagged(id));
>>>>>>> origin/develop12
}

inline void Group::set_history_schema_version(int version)
{
<<<<<<< HEAD
    // This function is only used is shared mode (from SharedGroup)
    REALM_ASSERT(m_top.size() >= 10);
    m_top.set(9, RefOrTagged::make_tagged(unsigned(version))); // Throws
}

inline void Group::set_history_parent(Array& history_root) noexcept
=======
    while (m_top.size() < s_hist_version_ndx + 1)
        m_top.add(0);
    m_top.set(s_hist_version_ndx, RefOrTagged::make_tagged(unsigned(version))); // Throws
}

template <class Accessor>
inline void Group::set_history_parent(Accessor& history_root) noexcept
>>>>>>> origin/develop12
{
    history_root.set_parent(&m_top, 8);
}

<<<<<<< HEAD
=======
template <class Accessor>
void Group::prepare_history_parent(Accessor& history_root, int history_type, int history_schema_version,
                                   uint64_t file_ident)
{
    prepare_top_for_history(history_type, history_schema_version, file_ident);
    set_history_parent(history_root);
}

>>>>>>> origin/develop12
class Group::TableWriter {
public:
    struct HistoryInfo {
        ref_type ref = 0;
        int type = 0;
        int version = 0;
<<<<<<< HEAD
=======
        uint64_t sync_file_id = 0;
>>>>>>> origin/develop12
    };

    virtual ref_type write_names(_impl::OutputStream&) = 0;
    virtual ref_type write_tables(_impl::OutputStream&) = 0;
    virtual HistoryInfo write_history(_impl::OutputStream&) = 0;
    virtual ~TableWriter() noexcept
    {
    }
};

<<<<<<< HEAD
inline const Table* Group::do_get_table(size_t table_ndx, DescMatcher desc_matcher) const
{
    return const_cast<Group*>(this)->do_get_table(table_ndx, desc_matcher); // Throws
}

inline const Table* Group::do_get_table(StringData name, DescMatcher desc_matcher) const
{
    return const_cast<Group*>(this)->do_get_table(name, desc_matcher); // Throws
=======
inline const Table* Group::do_get_table(size_t ndx) const
{
    return const_cast<Group*>(this)->do_get_table(ndx); // Throws
}

inline const Table* Group::do_get_table(StringData name) const
{
    return const_cast<Group*>(this)->do_get_table(name); // Throws
>>>>>>> origin/develop12
}

inline void Group::reset_free_space_tracking()
{
<<<<<<< HEAD
    m_alloc.reset_free_space_tracking(); // Throws
}

inline Replication* Group::get_replication() const noexcept
{
    return m_alloc.get_replication();
}

inline void Group::set_replication(Replication* repl) noexcept
{
    m_alloc.set_replication(repl);
}

=======
    // if used whith a shared allocator, free space should never be reset through
    // Group, but rather through the proper owner of the allocator, which is the DB object.
    REALM_ASSERT(m_local_alloc);
    m_alloc.reset_free_space_tracking(); // Throws
}

>>>>>>> origin/develop12
inline std::shared_ptr<metrics::Metrics> Group::get_metrics() const noexcept
{
    return m_metrics;
}

inline void Group::set_metrics(std::shared_ptr<metrics::Metrics> shared) noexcept
{
    m_metrics = shared;
}

// The purpose of this class is to give internal access to some, but
// not all of the non-public parts of the Group class.
class _impl::GroupFriend {
public:
<<<<<<< HEAD
    static Allocator& get_alloc(Group& group) noexcept
    {
        return group.m_alloc;
    }

    static const Allocator& get_alloc(const Group& group) noexcept
=======
    static Allocator& get_alloc(const Group& group) noexcept
>>>>>>> origin/develop12
    {
        return group.m_alloc;
    }

    static ref_type get_top_ref(const Group& group) noexcept
    {
        return group.m_top.get_ref();
    }

<<<<<<< HEAD
    static Table& get_table(Group& group, size_t ndx_in_group)
    {
        Group::DescMatcher desc_matcher = 0;                           // Do not check descriptor
        Table* table = group.do_get_table(ndx_in_group, desc_matcher); // Throws
        return *table;
    }

    static const Table& get_table(const Group& group, size_t ndx_in_group)
    {
        Group::DescMatcher desc_matcher = 0;                                 // Do not check descriptor
        const Table* table = group.do_get_table(ndx_in_group, desc_matcher); // Throws
        return *table;
    }

    static Table* get_table(Group& group, StringData name)
    {
        Group::DescMatcher desc_matcher = 0;                   // Do not check descriptor
        Table* table = group.do_get_table(name, desc_matcher); // Throws
        return table;
    }

    static const Table* get_table(const Group& group, StringData name)
    {
        Group::DescMatcher desc_matcher = 0;                         // Do not check descriptor
        const Table* table = group.do_get_table(name, desc_matcher); // Throws
        return table;
    }

    static Table& insert_table(Group& group, size_t table_ndx, StringData name, bool require_unique_name)
    {
        Group::DescSetter desc_setter = nullptr; // Do not add any columns
        return *group.do_insert_table(table_ndx, name, desc_setter, require_unique_name);
    }

    static Table& add_table(Group& group, StringData name, bool require_unique_name)
    {
        return insert_table(group, group.size(), name, require_unique_name);
    }

    static Table& get_or_insert_table(Group& group, size_t table_ndx, StringData name, bool* was_inserted)
    {
        Group::DescMatcher desc_matcher = nullptr; // Do not check descriptor
        Group::DescSetter desc_setter = nullptr;   // Do not add any columns
        return *group.do_get_or_insert_table(table_ndx, name, desc_matcher, desc_setter, was_inserted);
    }

    static Table& get_or_add_table(Group& group, StringData name, bool* was_inserted)
    {
        Group::DescMatcher desc_matcher = nullptr; // Do not check descriptor
        Group::DescSetter desc_setter = nullptr;   // Do not add any columns
        return *group.do_get_or_add_table(name, desc_matcher, desc_setter, was_inserted);
    }

    static void send_cascade_notification(const Group& group, const Group::CascadeNotification& notification)
    {
        group.send_cascade_notification(notification);
    }

    static Replication* get_replication(const Group& group) noexcept
    {
        return group.get_replication();
    }

    static void set_replication(Group& group, Replication* repl) noexcept
    {
        group.set_replication(repl);
    }

    static void detach(Group& group) noexcept
    {
        group.detach();
    }

    static void attach_shared(Group& group, ref_type new_top_ref, size_t new_file_size, bool writable)
    {
        group.attach_shared(new_top_ref, new_file_size, writable); // Throws
    }

    static void reset_free_space_tracking(Group& group)
    {
        group.reset_free_space_tracking(); // Throws
    }

    static void remap(Group& group, size_t new_file_size)
    {
        group.remap(new_file_size); // Throws
    }

    static void remap_and_update_refs(Group& group, ref_type new_top_ref, size_t new_file_size)
    {
        group.remap_and_update_refs(new_top_ref, new_file_size); // Throws
    }

    static void advance_transact(Group& group, ref_type new_top_ref, size_t new_file_size,
                                 _impl::NoCopyInputStream& in)
    {
        group.advance_transact(new_top_ref, new_file_size, in); // Throws
    }

    static void create_empty_group_when_missing(Group& group)
    {
        if (!group.m_top.is_attached())
            group.create_empty_group(); // Throws
=======
    static ref_type get_history_ref(Allocator& alloc, ref_type top_ref) noexcept
    {
        Array top(alloc);
        if (top_ref != 0)
            top.init_from_ref(top_ref);
        return Group::get_history_ref(top);
    }

    static ref_type get_history_ref(const Group& group) noexcept
    {
        return Group::get_history_ref(group.m_top);
    }

    static int get_file_format_version(const Group& group) noexcept
    {
        return group.get_file_format_version();
>>>>>>> origin/develop12
    }

    static void get_version_and_history_info(const Allocator& alloc, ref_type top_ref,
                                             _impl::History::version_type& version,
                                             int& history_type,
                                             int& history_schema_version) noexcept
    {
        Array top{const_cast<Allocator&>(alloc)};
        if (top_ref != 0)
            top.init_from_ref(top_ref);
        Group::get_version_and_history_info(top, version, history_type, history_schema_version);
    }

<<<<<<< HEAD
    static ref_type get_history_ref(const Group& group) noexcept
    {
        return Group::get_history_ref(group.m_top);
    }

    static ref_type get_history_ref(Allocator& alloc, ref_type top_ref) noexcept
    {
        Array top(alloc);
        if (top_ref != 0)
            top.init_from_ref(top_ref);
        return Group::get_history_ref(top);
    }

    static int get_history_schema_version(const Group& group) noexcept
    {
        return Group::get_history_schema_version(group.m_top);
    }

    static int get_history_schema_version(Allocator& alloc, ref_type top_ref) noexcept
    {
        Array top{alloc};
        if (top_ref != 0)
            top.init_from_ref(top_ref);
        return Group::get_history_schema_version(top);
    }

    static void set_history_schema_version(Group& group, int version)
    {
        group.set_history_schema_version(version); // Throws
    }

    static void set_history_parent(Group& group, Array& history_root) noexcept
    {
        group.set_history_parent(history_root);
    }

    static void prepare_history_parent(Group& group, Array& history_root, int history_type,
                                       int history_schema_version)
    {
        group.prepare_history_parent(history_root, history_type, history_schema_version); // Throws
    }

    static int get_file_format_version(const Group& group) noexcept
    {
        return group.get_file_format_version();
    }

    static void set_file_format_version(Group& group, int file_format_version) noexcept
    {
        group.set_file_format_version(file_format_version);
    }

    static int get_committed_file_format_version(const Group& group) noexcept
    {
        return group.get_committed_file_format_version();
    }

    static int get_target_file_format_version_for_session(int current_file_format_version, int history_type) noexcept
    {
        return Group::get_target_file_format_version_for_session(current_file_format_version, history_type);
    }

    static void upgrade_file_format(Group& group, int target_file_format_version)
    {
        group.upgrade_file_format(target_file_format_version); // Throws
    }
};


struct CascadeState : Group::CascadeNotification {
    /// If non-null, then no recursion will be performed for rows of that
    /// table. The effect is then exactly as if all the rows of that table were
    /// added to \a state.rows initially, and then removed again after the
    /// explicit invocations of Table::cascade_break_backlinks_to() (one for
    /// each initiating row). This is used by Table::clear() to avoid
    /// reentrance.
    ///
    /// Must never be set concurrently with stop_on_link_list_column.
    Table* stop_on_table = nullptr;

    /// If non-null, then Table::cascade_break_backlinks_to() will skip the
    /// removal of reciprocal backlinks for the link list at
    /// stop_on_link_list_row_ndx in this column, and no recursion will happen
    /// on its behalf. This is used by LinkView::clear() to avoid reentrance.
    ///
    /// Must never be set concurrently with stop_on_table.
    LinkListColumn* stop_on_link_list_column = nullptr;

    /// Is ignored if stop_on_link_list_column is null.
    size_t stop_on_link_list_row_ndx = 0;

    /// If false, the links field is not needed, so any work done just for that
    /// can be skipped.
    bool track_link_nullifications = false;

    /// If false, weak links are followed too
    bool only_strong_links = true;
};

inline bool Group::CascadeNotification::row::operator==(const row& r) const noexcept
{
    return table_ndx == r.table_ndx && row_ndx == r.row_ndx;
}

inline bool Group::CascadeNotification::row::operator!=(const row& r) const noexcept
{
    return !(*this == r);
}

inline bool Group::CascadeNotification::row::operator<(const row& r) const noexcept
{
    return table_ndx < r.table_ndx || (table_ndx == r.table_ndx && row_ndx < r.row_ndx);
}

=======
    static void set_history_schema_version(Group& group, int version)
    {
        group.set_history_schema_version(version); // Throws
    }

    template <class Accessor>
    static void set_history_parent(Group& group, Accessor& history_root) noexcept
    {
        group.set_history_parent(history_root);
    }

    template <class Accessor>
    static void prepare_history_parent(Group& group, Accessor& history_root, int history_type,
                                       int history_schema_version, uint64_t file_ident = 0)
    {
        group.prepare_history_parent(history_root, history_type, history_schema_version, file_ident); // Throws
    }

    // This is used by upgrade functions in Sync
    static Table* get_table_by_ndx(Group& group, size_t ndx)
    {
        return group.do_get_table(ndx);
    }

    static int get_target_file_format_version_for_session(int current_file_format_version, int history_type) noexcept
    {
        return Group::get_target_file_format_version_for_session(current_file_format_version, history_type);
    }
};


class CascadeState {
public:
    enum class Mode {
        /// If we remove the last link to an object, delete that object, even if
        /// the link we removed was not a strong link
        All,
        /// If we remove the last link to an object, delete that object only if
        /// the link we removed was a strong link
        Strong,
        /// Never delete objects due to removing links
        None
    };

    struct Link {
        TableKey origin_table;     ///< A group-level table.
        ColKey origin_col_key;     ///< Link column being nullified.
        ObjKey origin_key;         ///< Row in column being nullified.
        /// The target row index which is being removed. Mostly relevant for
        /// LinkList (to know which entries are being removed), but also
        /// valid for Link.
        ObjKey old_target_key;
    };

    CascadeState(Mode mode = Mode::Strong, Group* g = nullptr) noexcept
        : m_mode(mode)
        , m_group(g)
    {
    }

    /// Indicate which links to take action on. Either all, strong or none.
    Mode m_mode;

    std::vector<std::pair<TableKey, ObjKey>> m_to_be_deleted;
    std::vector<Link> m_to_be_nullified;
    Group* m_group = nullptr;

    bool notification_handler() const noexcept
    {
        return m_group && m_group->has_cascade_notification_handler();
    }

    void send_notifications(Group::CascadeNotification& notifications) const
    {
        REALM_ASSERT_DEBUG(notification_handler());
        m_group->send_cascade_notification(notifications);
    }

    bool enqueue_for_cascade(const Obj& target_obj, bool link_is_strong, bool last_removed)
    {
        // Check if the object should be cascade deleted
        if (m_mode == Mode::None && last_removed) {
            return false;
        }
        if (m_mode == Mode::All || link_is_strong) {
            bool has_backlinks = target_obj.has_backlinks(m_mode == Mode::Strong);
            if (!has_backlinks) {
                // Object has no more backlinks - add to list for deletion
                m_to_be_deleted.emplace_back(target_obj.get_table()->get_key(), target_obj.get_key());
                return true;
            }
        }
        return false;
    }

    void enqueue_for_nullification(Table& src_table, ColKey src_col_key, ObjKey origin_key, ObjKey target_key)
    {
        // Nullify immediately if we don't need to send cascade notifications
        if (!notification_handler()) {
            Obj obj = src_table.get_object(origin_key);
            obj.nullify_link(src_col_key, target_key);
            return;
        }

        // Otherwise enqueue it
        m_to_be_nullified.push_back({src_table.get_key(), src_col_key, origin_key, target_key});
    }

    void send_notifications()
    {
        if (!notification_handler()) {
            return;
        }
        Group::CascadeNotification notification;
        for (auto& o : m_to_be_deleted)
            notification.rows.emplace_back(o.first, o.second);
        for (auto& l : m_to_be_nullified)
            notification.links.emplace_back(l.origin_table, l.origin_col_key, l.origin_key, l.old_target_key);
        send_notifications(notification);
    }
};

>>>>>>> origin/develop12
} // namespace realm

#endif // REALM_GROUP_HPP
