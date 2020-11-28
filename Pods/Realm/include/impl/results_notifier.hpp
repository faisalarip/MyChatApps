////////////////////////////////////////////////////////////////////////////
//
// Copyright 2015 Realm Inc.
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

#ifndef REALM_RESULTS_NOTIFIER_HPP
#define REALM_RESULTS_NOTIFIER_HPP

#include "collection_notifier.hpp"
#include "results.hpp"

<<<<<<< HEAD
#include <realm/group_shared.hpp>

namespace realm {
namespace _impl {
class ResultsNotifier : public CollectionNotifier {
public:
    ResultsNotifier(Results& target);

    void target_results_moved(Results& old_target, Results& new_target);

private:
    // Target Results to update
    // Can only be used with lock_target() held
    Results* m_target_results;

    // The source Query, in handover form iff m_sg is null
    std::unique_ptr<SharedGroup::Handover<Query>> m_query_handover;
    std::unique_ptr<Query> m_query;

    DescriptorOrdering::HandoverPatch m_ordering_handover;
=======
#include <realm/db.hpp>

namespace realm {
namespace _impl {
class ResultsNotifierBase : public CollectionNotifier {
public:
    using ListIndices = util::Optional<std::vector<size_t>>;
    using CollectionNotifier::CollectionNotifier;

    virtual bool get_tableview(TableView&) { return false; }
    virtual bool get_list_indices(ListIndices&) { return false; }
};

class ResultsNotifier : public ResultsNotifierBase {
public:
    ResultsNotifier(Results& target);
    bool get_tableview(TableView& out) override;

private:
    std::unique_ptr<Query> m_query;
>>>>>>> origin/develop12
    DescriptorOrdering m_descriptor_ordering;
    bool m_target_is_in_table_order;

    // The TableView resulting from running the query. Will be detached unless
    // the query was (re)run since the last time the handover object was created
<<<<<<< HEAD
    TableView m_tv;
    std::unique_ptr<SharedGroup::Handover<TableView>> m_tv_handover;
    std::unique_ptr<SharedGroup::Handover<TableView>> m_tv_to_deliver;

    // The table version from the last time the query was run. Used to avoid
    // rerunning the query when there's no chance of it changing.
    uint_fast64_t m_last_seen_version = -1;

    // The rows from the previous run of the query, for calculating diffs
    std::vector<size_t> m_previous_rows;

    // The changeset calculated during run() and delivered in do_prepare_handover()
    CollectionChangeBuilder m_changes;
    TransactionChangeInfo* m_info = nullptr;

    bool need_to_run();
    void calculate_changes();
    void deliver(SharedGroup&) override;

    void run() override;
    void do_prepare_handover(SharedGroup&) override;
=======
    TableView m_run_tv;

    TransactionRef m_handover_transaction;
    std::unique_ptr<TableView> m_handover_tv;
    TransactionRef m_delivered_transaction;
    std::unique_ptr<TableView> m_delivered_tv;

    // The table version from the last time the query was run. Used to avoid
    // rerunning the query when there's no chance of it changing.
    TableVersions m_last_seen_version;

    // The rows from the previous run of the query, for calculating diffs
    std::vector<int64_t> m_previous_rows;

    TransactionChangeInfo* m_info = nullptr;
    bool m_results_were_used = true;

    bool need_to_run();
    void calculate_changes();

    void run() override;
    void do_prepare_handover(Transaction&) override;
    bool do_add_required_change_info(TransactionChangeInfo& info) override;
    bool prepare_to_deliver() override;

    void release_data() noexcept override;
    void do_attach_to(Transaction& sg) override;
};

class ListResultsNotifier : public ResultsNotifierBase {
public:
    ListResultsNotifier(Results& target);
    bool get_list_indices(ListIndices& out) override;

private:
    std::shared_ptr<LstBase> m_list;
    util::Optional<bool> m_sort_order;
    bool m_distinct = false;

    ListIndices m_run_indices;

    VersionID m_handover_transaction_version;
    ListIndices m_handover_indices;
    VersionID m_delivered_transaction_version;
    ListIndices m_delivered_indices;

    // The rows from the previous run of the query, for calculating diffs
    std::vector<size_t> m_previous_indices;

    TransactionChangeInfo* m_info = nullptr;
    bool m_results_were_used = true;

    bool need_to_run();
    void calculate_changes();

    void run() override;
    void do_prepare_handover(Transaction&) override;
>>>>>>> origin/develop12
    bool do_add_required_change_info(TransactionChangeInfo& info) override;
    bool prepare_to_deliver() override;

    void release_data() noexcept override;
<<<<<<< HEAD
    void do_attach_to(SharedGroup& sg) override;
    void do_detach_from(SharedGroup& sg) override;
=======
    void do_attach_to(Transaction& sg) override;
>>>>>>> origin/develop12
};

} // namespace _impl
} // namespace realm

#endif /* REALM_RESULTS_NOTIFIER_HPP */
