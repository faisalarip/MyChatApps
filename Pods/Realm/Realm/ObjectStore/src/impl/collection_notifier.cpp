////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016 Realm Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////

#include "impl/collection_notifier.hpp"

#include "impl/realm_coordinator.hpp"
#include "shared_realm.hpp"

<<<<<<< HEAD
#include <realm/group_shared.hpp>
#include <realm/link_view.hpp>
=======
#include <realm/db.hpp>
>>>>>>> origin/develop12

using namespace realm;
using namespace realm::_impl;

<<<<<<< HEAD
std::function<bool (size_t)>
CollectionNotifier::get_modification_checker(TransactionChangeInfo const& info,
                                             Table const& root_table)
=======
bool CollectionNotifier::all_related_tables_covered(const TableVersions& versions)
{
    if (m_related_tables.size() > versions.size()) {
        return false;
    }
    auto first = versions.begin();
    auto last = versions.end();
    for (auto& it : m_related_tables) {
        TableKey tk{it.table_key};
        auto match = std::find_if(first, last, [tk](auto& elem) {
            return elem.first == tk;
        });
        if (match == last) {
            // tk not found in versions
            return false;
        }
    }
    return true;
}

std::function<bool (ObjectChangeSet::ObjectKeyType)>
CollectionNotifier::get_modification_checker(TransactionChangeInfo const& info,
                                             ConstTableRef root_table)
>>>>>>> origin/develop12
{
    if (info.schema_changed)
        set_table(root_table);

    // First check if any of the tables accessible from the root table were
    // actually modified. This can be false if there were only insertions, or
    // deletions which were not linked to by any row in the linking table
    auto table_modified = [&](auto& tbl) {
<<<<<<< HEAD
        return tbl.table_ndx < info.tables.size()
            && !info.tables[tbl.table_ndx].modifications.empty();
    };
    if (!any_of(begin(m_related_tables), end(m_related_tables), table_modified)) {
        return [](size_t) { return false; };
    }
    if (m_related_tables.size() == 1) {
        auto& modifications = info.tables[m_related_tables[0].table_ndx].modifications;
        return [&](size_t row) { return modifications.contains(row); };
    }

    return DeepChangeChecker(info, root_table, m_related_tables);
=======
        auto it = info.tables.find(tbl.table_key.value);
        return it != info.tables.end() && !it->second.modifications_empty();
    };
    if (!any_of(begin(m_related_tables), end(m_related_tables), table_modified)) {
        return [](ObjectChangeSet::ObjectKeyType) { return false; };
    }
    if (m_related_tables.size() == 1) {
        auto& object_set = info.tables.find(m_related_tables[0].table_key.value)->second;
        return [&](ObjectChangeSet::ObjectKeyType object_key) { return object_set.modifications_contains(object_key); };
    }

    return DeepChangeChecker(info, *root_table, m_related_tables);
>>>>>>> origin/develop12
}

void DeepChangeChecker::find_related_tables(std::vector<RelatedTable>& out, Table const& table)
{
<<<<<<< HEAD
    auto table_ndx = table.get_index_in_group();
    if (table_ndx == npos)
        return;
    if (any_of(begin(out), end(out), [=](auto& tbl) { return tbl.table_ndx == table_ndx; }))
=======
    auto table_key = table.get_key();
    if (any_of(begin(out), end(out), [=](auto& tbl) { return tbl.table_key == table_key; }))
>>>>>>> origin/develop12
        return;

    // We need to add this table to `out` before recurring so that the check
    // above works, but we can't store a pointer to the thing being populated
    // because the recursive calls may resize `out`, so instead look it up by
    // index every time
    size_t out_index = out.size();
<<<<<<< HEAD
    out.push_back({table_ndx, {}});

    for (size_t i = 0, count = table.get_column_count(); i != count; ++i) {
        auto type = table.get_column_type(i);
        if (type == type_Link || type == type_LinkList) {
            out[out_index].links.push_back({i, type == type_LinkList});
            find_related_tables(out, *table.get_link_target(i));
=======
    out.push_back({table_key, {}});

    for (auto col_key : table.get_column_keys()) {
        auto type = table.get_column_type(col_key);
        if (type == type_Link || type == type_LinkList) {
            out[out_index].links.push_back({col_key.value, type == type_LinkList});
            find_related_tables(out, *table.get_link_target(col_key));
>>>>>>> origin/develop12
        }
    }
}

DeepChangeChecker::DeepChangeChecker(TransactionChangeInfo const& info,
                                     Table const& root_table,
                                     std::vector<RelatedTable> const& related_tables)
: m_info(info)
, m_root_table(root_table)
<<<<<<< HEAD
, m_root_table_ndx(root_table.get_index_in_group())
, m_root_modifications(m_root_table_ndx < info.tables.size() ? &info.tables[m_root_table_ndx].modifications : nullptr)
=======
, m_root_table_key(root_table.get_key().value)
, m_root_object_changes([&] {
    auto it = info.tables.find(m_root_table_key.value);
    return it != info.tables.end() ? &it->second : nullptr;
}())
>>>>>>> origin/develop12
, m_related_tables(related_tables)
{
}

<<<<<<< HEAD
bool DeepChangeChecker::check_outgoing_links(size_t table_ndx,
                                             Table const& table,
                                             size_t row_ndx, size_t depth)
{
    auto it = find_if(begin(m_related_tables), end(m_related_tables),
                      [&](auto&& tbl) { return tbl.table_ndx == table_ndx; });
    if (it == m_related_tables.end())
        return false;

    // Check if we're already checking if the destination of the link is
    // modified, and if not add it to the stack
    auto already_checking = [&](size_t col) {
        auto end = m_current_path.begin() + depth;
        auto match = std::find_if(m_current_path.begin(), end, [&](auto& p) {
            return p.table == table_ndx && p.row == row_ndx && p.col == col;
=======
bool DeepChangeChecker::check_outgoing_links(TableKey table_key, Table const& table,
                                             int64_t obj_key, size_t depth)
{
    auto it = find_if(begin(m_related_tables), end(m_related_tables),
                      [&](auto&& tbl) { return tbl.table_key == table_key; });
    if (it == m_related_tables.end())
        return false;
    if (it->links.empty())
        return false;

    // Check if we're already checking if the destination of the link is
    // modified, and if not add it to the stack
    auto already_checking = [&](int64_t col) {
        auto end = m_current_path.begin() + depth;
        auto match = std::find_if(m_current_path.begin(), end, [&](auto& p) {
            return p.obj_key == obj_key && p.col_key == col;
>>>>>>> origin/develop12
        });
        if (match != end) {
            for (; match < end; ++match) match->depth_exceeded = true;
            return true;
        }
<<<<<<< HEAD
        m_current_path[depth] = {table_ndx, row_ndx, col, false};
        return false;
    };

    auto linked_object_changed = [&](OutgoingLink const& link) {
        if (already_checking(link.col_ndx))
            return false;
        if (!link.is_list) {
            if (table.is_null_link(link.col_ndx, row_ndx))
                return false;
            auto dst = table.get_link(link.col_ndx, row_ndx);
            return check_row(*table.get_link_target(link.col_ndx), dst, depth + 1);
        }

        auto& target = *table.get_link_target(link.col_ndx);
        auto lvr = table.get_linklist(link.col_ndx, row_ndx);
        for (size_t j = 0, size = lvr->size(); j < size; ++j) {
            size_t dst = lvr->get(j).get_index();
            if (check_row(target, dst, depth + 1))
                return true;
        }
        return false;
=======
        m_current_path[depth] = {obj_key, col, false};
        return false;
    };

    ConstObj obj = table.get_object(ObjKey(obj_key));
    auto linked_object_changed = [&](OutgoingLink const& link) {
        if (already_checking(link.col_key))
            return false;
        if (!link.is_list) {
            if (obj.is_null(ColKey(link.col_key)))
                return false;
            auto dst = obj.get<ObjKey>(ColKey(link.col_key)).value;
            return check_row(*table.get_link_target(ColKey(link.col_key)), dst, depth + 1);
        }

        auto& target = *table.get_link_target(ColKey(link.col_key));
        auto lvr = obj.get_linklist(ColKey(link.col_key));
        return std::any_of(lvr.begin(), lvr.end(),
                           [&, this](auto key) { return this->check_row(target, key.value, depth + 1); });
>>>>>>> origin/develop12
    };

    return std::any_of(begin(it->links), end(it->links), linked_object_changed);
}

<<<<<<< HEAD
bool DeepChangeChecker::check_row(Table const& table, size_t idx, size_t depth)
=======
bool DeepChangeChecker::check_row(Table const& table, ObjKeyType key, size_t depth)
>>>>>>> origin/develop12
{
    // Arbitrary upper limit on the maximum depth to search
    if (depth >= m_current_path.size()) {
        // Don't mark any of the intermediate rows checked along the path as
        // not modified, as a search starting from them might hit a modification
        for (size_t i = 0; i < m_current_path.size(); ++i)
            m_current_path[i].depth_exceeded = true;
        return false;
    }

<<<<<<< HEAD
    size_t table_ndx = table.get_index_in_group();
    if (depth > 0 && table_ndx < m_info.tables.size() && m_info.tables[table_ndx].modifications.contains(idx))
        return true;

    if (m_not_modified.size() <= table_ndx)
        m_not_modified.resize(table_ndx + 1);
    if (m_not_modified[table_ndx].contains(idx))
        return false;

    bool ret = check_outgoing_links(table_ndx, table, idx, depth);
    if (!ret && (depth == 0 || !m_current_path[depth - 1].depth_exceeded))
        m_not_modified[table_ndx].add(idx);
    return ret;
}

bool DeepChangeChecker::operator()(size_t ndx)
{
    if (m_root_modifications && m_root_modifications->contains(ndx))
        return true;
    return check_row(m_root_table, ndx, 0);
=======
    TableKey table_key = table.get_key();
    if (depth > 0) {
        auto it = m_info.tables.find(table_key.value);
        if (it != m_info.tables.end() && it->second.modifications_contains(key))
            return true;
    }
    auto& not_modified = m_not_modified[table_key.value];
    auto it = not_modified.find(key);
    if (it != not_modified.end())
        return false;

    bool ret = check_outgoing_links(table_key, table, key, depth);
    if (!ret && (depth == 0 || !m_current_path[depth - 1].depth_exceeded))
        not_modified.insert(key);
    return ret;
}

bool DeepChangeChecker::operator()(ObjKeyType key)
{
    if (m_root_object_changes && m_root_object_changes->modifications_contains(key))
        return true;
    return check_row(m_root_table, key, 0);
>>>>>>> origin/develop12
}

CollectionNotifier::CollectionNotifier(std::shared_ptr<Realm> realm)
: m_realm(std::move(realm))
<<<<<<< HEAD
, m_sg_version(Realm::Internal::get_shared_group(*m_realm)->get_version_of_current_transaction())
=======
, m_sg_version(Realm::Internal::get_transaction(*m_realm).get_version_of_current_transaction())
>>>>>>> origin/develop12
{
}

CollectionNotifier::~CollectionNotifier()
{
    // Need to do this explicitly to ensure m_realm is destroyed with the mutex
    // held to avoid potential double-deletion
    unregister();
}

<<<<<<< HEAD
=======
void CollectionNotifier::release_data() noexcept
{
    m_sg = nullptr;
}

>>>>>>> origin/develop12
uint64_t CollectionNotifier::add_callback(CollectionChangeCallback callback)
{
    m_realm->verify_thread();

<<<<<<< HEAD
    std::lock_guard<std::mutex> lock(m_callback_mutex);
=======
    util::CheckedLockGuard lock(m_callback_mutex);
>>>>>>> origin/develop12
    auto token = m_next_token++;
    m_callbacks.push_back({std::move(callback), {}, {}, token, false, false});
    if (m_callback_index == npos) { // Don't need to wake up if we're already sending notifications
        Realm::Internal::get_coordinator(*m_realm).wake_up_notifier_worker();
        m_have_callbacks = true;
    }
    return token;
}

void CollectionNotifier::remove_callback(uint64_t token)
{
    // the callback needs to be destroyed after releasing the lock as destroying
    // it could cause user code to be called
    Callback old;
    {
<<<<<<< HEAD
        std::lock_guard<std::mutex> lock(m_callback_mutex);
=======
        util::CheckedLockGuard lock(m_callback_mutex);
>>>>>>> origin/develop12
        auto it = find_callback(token);
        if (it == end(m_callbacks)) {
            return;
        }

        size_t idx = distance(begin(m_callbacks), it);
        if (m_callback_index != npos) {
            if (m_callback_index >= idx)
                --m_callback_index;
        }
        --m_callback_count;

        old = std::move(*it);
        m_callbacks.erase(it);

        m_have_callbacks = !m_callbacks.empty();
    }
}

void CollectionNotifier::suppress_next_notification(uint64_t token)
{
    {
        std::lock_guard<std::mutex> lock(m_realm_mutex);
        REALM_ASSERT(m_realm);
        m_realm->verify_thread();
        m_realm->verify_in_write();
    }

<<<<<<< HEAD
    std::lock_guard<std::mutex> lock(m_callback_mutex);
=======
    util::CheckedLockGuard lock(m_callback_mutex);
>>>>>>> origin/develop12
    auto it = find_callback(token);
    if (it != end(m_callbacks)) {
        it->skip_next = true;
    }
}

std::vector<CollectionNotifier::Callback>::iterator CollectionNotifier::find_callback(uint64_t token)
{
    REALM_ASSERT(m_error || m_callbacks.size() > 0);

    auto it = find_if(begin(m_callbacks), end(m_callbacks),
                      [=](const auto& c) { return c.token == token; });
    // We should only fail to find the callback if it was removed due to an error
    REALM_ASSERT(m_error || it != end(m_callbacks));
    return it;
}

void CollectionNotifier::unregister() noexcept
{
    std::lock_guard<std::mutex> lock(m_realm_mutex);
    m_realm = nullptr;
}

bool CollectionNotifier::is_alive() const noexcept
{
    std::lock_guard<std::mutex> lock(m_realm_mutex);
    return m_realm != nullptr;
}

std::unique_lock<std::mutex> CollectionNotifier::lock_target()
{
    return std::unique_lock<std::mutex>{m_realm_mutex};
}

<<<<<<< HEAD
void CollectionNotifier::set_table(Table const& table)
{
    m_related_tables.clear();
    DeepChangeChecker::find_related_tables(m_related_tables, table);
=======
void CollectionNotifier::set_table(ConstTableRef table)
{
    m_related_tables.clear();
    DeepChangeChecker::find_related_tables(m_related_tables, *table);
>>>>>>> origin/develop12
}

void CollectionNotifier::add_required_change_info(TransactionChangeInfo& info)
{
    if (!do_add_required_change_info(info) || m_related_tables.empty()) {
        return;
    }

<<<<<<< HEAD
    auto max = max_element(begin(m_related_tables), end(m_related_tables),
                           [](auto&& a, auto&& b) { return a.table_ndx < b.table_ndx; });

    if (max->table_ndx >= info.table_modifications_needed.size())
        info.table_modifications_needed.resize(max->table_ndx + 1, false);
    for (auto& tbl : m_related_tables) {
        info.table_modifications_needed[tbl.table_ndx] = true;
    }
=======
    info.tables.reserve(m_related_tables.size());
    for (auto& tbl : m_related_tables)
        info.tables[tbl.table_key.value];
>>>>>>> origin/develop12
}

void CollectionNotifier::prepare_handover()
{
    REALM_ASSERT(m_sg);
    m_sg_version = m_sg->get_version_of_current_transaction();
    do_prepare_handover(*m_sg);
<<<<<<< HEAD
    m_has_run = true;

#ifdef REALM_DEBUG
    std::lock_guard<std::mutex> lock(m_callback_mutex);
=======
    add_changes(std::move(m_change));
    REALM_ASSERT(m_change.empty());
    m_has_run = true;

#ifdef REALM_DEBUG
    util::CheckedLockGuard lock(m_callback_mutex);
>>>>>>> origin/develop12
    for (auto& callback : m_callbacks)
        REALM_ASSERT(!callback.skip_next);
#endif
}

void CollectionNotifier::before_advance()
{
    for_each_callback([&](auto& lock, auto& callback) {
        if (callback.changes_to_deliver.empty()) {
            return;
        }

        auto changes = callback.changes_to_deliver;
        // acquire a local reference to the callback so that removing the
        // callback from within it can't result in a dangling pointer
        auto cb = callback.fn;
<<<<<<< HEAD
        lock.unlock();
=======
        lock.unlock_unchecked();
>>>>>>> origin/develop12
        cb.before(changes);
    });
}

void CollectionNotifier::after_advance()
{
    for_each_callback([&](auto& lock, auto& callback) {
        if (callback.initial_delivered && callback.changes_to_deliver.empty()) {
            return;
        }
        callback.initial_delivered = true;

        auto changes = std::move(callback.changes_to_deliver);
        // acquire a local reference to the callback so that removing the
        // callback from within it can't result in a dangling pointer
        auto cb = callback.fn;
<<<<<<< HEAD
        lock.unlock();
=======
        lock.unlock_unchecked();
>>>>>>> origin/develop12
        cb.after(changes);
    });
}

void CollectionNotifier::deliver_error(std::exception_ptr error)
{
    // Don't complain about double-unregistering callbacks
    m_error = true;

    m_callback_count = m_callbacks.size();
    for_each_callback([this, &error](auto& lock, auto& callback) {
        // acquire a local reference to the callback so that removing the
        // callback from within it can't result in a dangling pointer
        auto cb = std::move(callback.fn);
        auto token = callback.token;
<<<<<<< HEAD
        lock.unlock();
=======
        lock.unlock_unchecked();
>>>>>>> origin/develop12
        cb.error(error);

        // We never want to call the callback again after this, so just remove it
        this->remove_callback(token);
    });
}

bool CollectionNotifier::is_for_realm(Realm& realm) const noexcept
{
    std::lock_guard<std::mutex> lock(m_realm_mutex);
    return m_realm.get() == &realm;
}

bool CollectionNotifier::package_for_delivery()
{
    if (!prepare_to_deliver())
        return false;
<<<<<<< HEAD
    std::lock_guard<std::mutex> l(m_callback_mutex);
=======
    util::CheckedLockGuard lock(m_callback_mutex);
>>>>>>> origin/develop12
    for (auto& callback : m_callbacks)
        callback.changes_to_deliver = std::move(callback.accumulated_changes).finalize();
    m_callback_count = m_callbacks.size();
    return true;
}

template<typename Fn>
void CollectionNotifier::for_each_callback(Fn&& fn)
{
<<<<<<< HEAD
    std::unique_lock<std::mutex> callback_lock(m_callback_mutex);
=======
    util::CheckedUniqueLock callback_lock(m_callback_mutex);
>>>>>>> origin/develop12
    REALM_ASSERT_DEBUG(m_callback_count <= m_callbacks.size());
    for (++m_callback_index; m_callback_index < m_callback_count; ++m_callback_index) {
        fn(callback_lock, m_callbacks[m_callback_index]);
        if (!callback_lock.owns_lock())
<<<<<<< HEAD
            callback_lock.lock();
=======
            callback_lock.lock_unchecked();
>>>>>>> origin/develop12
    }

    m_callback_index = npos;
}

<<<<<<< HEAD
void CollectionNotifier::attach_to(SharedGroup& sg)
{
    REALM_ASSERT(!m_sg);

    m_sg = &sg;
    do_attach_to(sg);
}

void CollectionNotifier::detach()
{
    REALM_ASSERT(m_sg);
    do_detach_from(*m_sg);
    m_sg = nullptr;
}

SharedGroup& CollectionNotifier::source_shared_group()
{
    return *Realm::Internal::get_shared_group(*m_realm);
=======
void CollectionNotifier::attach_to(std::shared_ptr<Transaction> sg)
{
    do_attach_to(*sg);
    m_sg = std::move(sg);
}

Transaction& CollectionNotifier::source_shared_group()
{
    return Realm::Internal::get_transaction(*m_realm);
>>>>>>> origin/develop12
}

void CollectionNotifier::add_changes(CollectionChangeBuilder change)
{
<<<<<<< HEAD
    std::lock_guard<std::mutex> lock(m_callback_mutex);
=======
    util::CheckedLockGuard lock(m_callback_mutex);
>>>>>>> origin/develop12
    for (auto& callback : m_callbacks) {
        if (callback.skip_next) {
            REALM_ASSERT_DEBUG(callback.accumulated_changes.empty());
            callback.skip_next = false;
        }
        else {
            if (&callback == &m_callbacks.back())
                callback.accumulated_changes.merge(std::move(change));
            else
                callback.accumulated_changes.merge(CollectionChangeBuilder(change));
        }
    }
}

NotifierPackage::NotifierPackage(std::exception_ptr error,
                                 std::vector<std::shared_ptr<CollectionNotifier>> notifiers,
                                 RealmCoordinator* coordinator)
: m_notifiers(std::move(notifiers))
, m_coordinator(coordinator)
, m_error(std::move(error))
{
}

<<<<<<< HEAD
void NotifierPackage::package_and_wait(util::Optional<VersionID::version_type> target_version)
=======
// Clang TSE seems to not like returning a unique_lock from a function
void NotifierPackage::package_and_wait(util::Optional<VersionID::version_type> target_version) NO_THREAD_SAFETY_ANALYSIS
>>>>>>> origin/develop12
{
    if (!m_coordinator || m_error || !*this)
        return;

    auto lock = m_coordinator->wait_for_notifiers([&] {
        if (!target_version)
            return true;
        return std::all_of(begin(m_notifiers), end(m_notifiers), [&](auto const& n) {
            return !n->have_callbacks() || (n->has_run() && n->version().version >= *target_version);
        });
    });

    // Package the notifiers for delivery and remove any which don't have anything to deliver
    auto package = [&](auto& notifier) {
        if (notifier->has_run() && notifier->package_for_delivery()) {
            m_version = notifier->version();
            return false;
        }
        return true;
    };
    m_notifiers.erase(std::remove_if(begin(m_notifiers), end(m_notifiers), package), end(m_notifiers));
    if (m_version && target_version && m_version->version < *target_version) {
        m_notifiers.clear();
        m_version = util::none;
    }
    REALM_ASSERT(m_version || m_notifiers.empty());

    m_coordinator = nullptr;
}

void NotifierPackage::before_advance()
{
    if (m_error)
        return;
    for (auto& notifier : m_notifiers)
        notifier->before_advance();
}

<<<<<<< HEAD
void NotifierPackage::deliver(SharedGroup& sg)
=======
void NotifierPackage::after_advance()
>>>>>>> origin/develop12
{
    if (m_error) {
        for (auto& notifier : m_notifiers)
            notifier->deliver_error(m_error);
        return;
    }
<<<<<<< HEAD
    // Can't deliver while in a write transaction
    if (sg.get_transact_stage() != SharedGroup::transact_Reading)
        return;
    for (auto& notifier : m_notifiers)
        notifier->deliver(sg);
}

void NotifierPackage::after_advance()
{
    if (m_error)
        return;
=======
>>>>>>> origin/develop12
    for (auto& notifier : m_notifiers)
        notifier->after_advance();
}

void NotifierPackage::add_notifier(std::shared_ptr<CollectionNotifier> notifier)
{
    m_notifiers.push_back(notifier);
    m_coordinator->register_notifier(notifier);
}
