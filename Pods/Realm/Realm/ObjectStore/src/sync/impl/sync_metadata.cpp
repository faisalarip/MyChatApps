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

#include "sync/impl/sync_metadata.hpp"
<<<<<<< HEAD
=======
#include "impl/realm_coordinator.hpp"
>>>>>>> origin/develop12

#include "object_schema.hpp"
#include "object_store.hpp"
#include "property.hpp"
#include "results.hpp"
#include "schema.hpp"
#include "util/uuid.hpp"
#if REALM_PLATFORM_APPLE
#include "impl/apple/keychain_helper.hpp"
#endif

<<<<<<< HEAD
#include <realm/descriptor.hpp>
=======
#include <realm/db.hpp>
>>>>>>> origin/develop12
#include <realm/table.hpp>

namespace {
static const char * const c_sync_userMetadata = "UserMetadata";
static const char * const c_sync_marked_for_removal = "marked_for_removal";
static const char * const c_sync_identity = "identity";
static const char * const c_sync_local_uuid = "local_uuid";
static const char * const c_sync_auth_server_url = "auth_server_url";
static const char * const c_sync_user_token = "user_token";
static const char * const c_sync_user_is_admin = "user_is_admin";

static const char * const c_sync_fileActionMetadata = "FileActionMetadata";
static const char * const c_sync_original_name = "original_name";
static const char * const c_sync_new_name = "new_name";
static const char * const c_sync_action = "action";
static const char * const c_sync_url = "url";

static const char * const c_sync_clientMetadata = "ClientMetadata";
static const char * const c_sync_uuid = "uuid";

realm::Schema make_schema()
{
    using namespace realm;
    return Schema{
        {c_sync_userMetadata, {
            {c_sync_identity, PropertyType::String},
            {c_sync_local_uuid, PropertyType::String},
            {c_sync_marked_for_removal, PropertyType::Bool},
            {c_sync_user_token, PropertyType::String|PropertyType::Nullable},
            {c_sync_auth_server_url, PropertyType::String},
            {c_sync_user_is_admin, PropertyType::Bool},
        }},
        {c_sync_fileActionMetadata, {
            {c_sync_original_name, PropertyType::String, Property::IsPrimary{true}},
            {c_sync_new_name, PropertyType::String|PropertyType::Nullable},
            {c_sync_action, PropertyType::Int},
            {c_sync_url, PropertyType::String},
            {c_sync_identity, PropertyType::String},
        }},
        {c_sync_clientMetadata, {
            {c_sync_uuid, PropertyType::String},
        }}
    };
}

} // anonymous namespace

namespace realm {

// MARK: - Sync metadata manager

SyncMetadataManager::SyncMetadataManager(std::string path,
                                         bool should_encrypt,
                                         util::Optional<std::vector<char>> encryption_key)
{
    constexpr uint64_t SCHEMA_VERSION = 2;

    Realm::Config config;
    config.automatic_change_notifications = false;
    config.path = path;
    config.schema = make_schema();
    config.schema_version = SCHEMA_VERSION;
    config.schema_mode = SchemaMode::Automatic;
#if REALM_PLATFORM_APPLE
    if (should_encrypt && !encryption_key) {
        encryption_key = keychain::metadata_realm_encryption_key(File::exists(path));
    }
#endif
    if (should_encrypt) {
        if (!encryption_key) {
            throw std::invalid_argument("Metadata Realm encryption was specified, but no encryption key was provided.");
        }
        config.encryption_key = std::move(*encryption_key);
    }

    config.migration_function = [](SharedRealm old_realm, SharedRealm realm, Schema&) {
        if (old_realm->schema_version() < 2) {
            TableRef old_table = ObjectStore::table_for_object_type(old_realm->read_group(), c_sync_userMetadata);
            TableRef table = ObjectStore::table_for_object_type(realm->read_group(), c_sync_userMetadata);

<<<<<<< HEAD
            // Get all the SyncUserMetadata objects.
            Results results(old_realm, *old_table);

            // Column indices.
            size_t old_idx_identity = old_table->get_column_index(c_sync_identity);
            size_t old_idx_url = old_table->get_column_index(c_sync_auth_server_url);
            size_t idx_local_uuid = table->get_column_index(c_sync_local_uuid);
            size_t idx_url = table->get_column_index(c_sync_auth_server_url);

            for (size_t i = 0; i < results.size(); i++) {
                RowExpr entry = results.get(i);
                // Set the UUID equal to the user identity for existing users.
                auto identity = entry.get_string(old_idx_identity);
                table->set_string(idx_local_uuid, entry.get_index(), identity);
                // Migrate the auth server URLs to a non-nullable property.
                auto url = entry.get_string(old_idx_url);
                table->set_string(idx_url, entry.get_index(), url.is_null() ? "" : url);
=======
            // Column indices.
            ColKey old_idx_identity = old_table->get_column_key(c_sync_identity);
            ColKey old_idx_url = old_table->get_column_key(c_sync_auth_server_url);
            ColKey idx_local_uuid = table->get_column_key(c_sync_local_uuid);
            ColKey idx_url = table->get_column_key(c_sync_auth_server_url);

            auto to = table->begin();
            for (auto& from : *old_table) {
                REALM_ASSERT(to != table->end());
                // Set the UUID equal to the user identity for existing users.
                auto identity = from.get<String>(old_idx_identity);
                to->set(idx_local_uuid, identity);
                // Migrate the auth server URLs to a non-nullable property.
                auto url = from.get<String>(old_idx_url);
                to->set<String>(idx_url, url.is_null() ? "" : url);
                ++to;
>>>>>>> origin/develop12
            }
        }
    };

    SharedRealm realm = Realm::get_shared_realm(config);

    // Get data about the (hardcoded) schemas
    auto object_schema = realm->schema().find(c_sync_userMetadata);
    m_user_schema = {
<<<<<<< HEAD
        object_schema->persisted_properties[0].table_column,
        object_schema->persisted_properties[1].table_column,
        object_schema->persisted_properties[2].table_column,
        object_schema->persisted_properties[3].table_column,
        object_schema->persisted_properties[4].table_column,
        object_schema->persisted_properties[5].table_column,
=======
        object_schema->persisted_properties[0].column_key,
        object_schema->persisted_properties[1].column_key,
        object_schema->persisted_properties[2].column_key,
        object_schema->persisted_properties[3].column_key,
        object_schema->persisted_properties[4].column_key,
        object_schema->persisted_properties[5].column_key,
>>>>>>> origin/develop12
    };

    object_schema = realm->schema().find(c_sync_fileActionMetadata);
    m_file_action_schema = {
<<<<<<< HEAD
        object_schema->persisted_properties[0].table_column,
        object_schema->persisted_properties[1].table_column,
        object_schema->persisted_properties[2].table_column,
        object_schema->persisted_properties[3].table_column,
        object_schema->persisted_properties[4].table_column,
=======
        object_schema->persisted_properties[0].column_key,
        object_schema->persisted_properties[1].column_key,
        object_schema->persisted_properties[2].column_key,
        object_schema->persisted_properties[3].column_key,
        object_schema->persisted_properties[4].column_key,
>>>>>>> origin/develop12
    };

    object_schema = realm->schema().find(c_sync_clientMetadata);
    m_client_schema = {
<<<<<<< HEAD
        object_schema->persisted_properties[0].table_column,
=======
        object_schema->persisted_properties[0].column_key,
>>>>>>> origin/develop12
    };

    m_metadata_config = std::move(config);

    m_client_uuid = [&]() -> std::string {
        TableRef table = ObjectStore::table_for_object_type(realm->read_group(), c_sync_clientMetadata);
        if (table->is_empty()) {
            realm->begin_transaction();
            if (table->is_empty()) {
<<<<<<< HEAD
                size_t idx = table->add_empty_row();
                REALM_ASSERT_DEBUG(idx == 0);
                auto uuid = uuid_string();
                table->set_string(m_client_schema.idx_uuid, idx, uuid);
=======
                auto uuid = uuid_string();
                table->create_object().set(m_client_schema.idx_uuid, uuid);
>>>>>>> origin/develop12
                realm->commit_transaction();
                return uuid;
            }
            realm->cancel_transaction();
        }
<<<<<<< HEAD
        return table->get_string(m_client_schema.idx_uuid, 0);
=======
        return table->begin()->get<String>(m_client_schema.idx_uuid);
>>>>>>> origin/develop12
    }();
}

SyncUserMetadataResults SyncMetadataManager::all_unmarked_users() const
{
    return get_users(false);
}

SyncUserMetadataResults SyncMetadataManager::all_users_marked_for_removal() const
{
    return get_users(true);
}

SyncUserMetadataResults SyncMetadataManager::get_users(bool marked) const
{
    auto realm = get_realm();
    TableRef table = ObjectStore::table_for_object_type(realm->read_group(), c_sync_userMetadata);
    Query query = table->where().equal(m_user_schema.idx_marked_for_removal, marked);

    Results results(realm, std::move(query));
    return SyncUserMetadataResults(std::move(results), std::move(realm), m_user_schema);
}

SyncFileActionMetadataResults SyncMetadataManager::all_pending_actions() const
{
    auto realm = get_realm();
    TableRef table = ObjectStore::table_for_object_type(realm->read_group(), c_sync_fileActionMetadata);
    Results results(realm, table->where());
    return SyncFileActionMetadataResults(std::move(results), std::move(realm), m_file_action_schema);
}

util::Optional<SyncUserMetadata> SyncMetadataManager::get_or_make_user_metadata(const std::string& identity,
                                                                                const std::string& url,
                                                                                bool make_if_absent) const
{
    auto realm = get_realm();
    auto& schema = m_user_schema;

    // Retrieve or create the row for this object.
    TableRef table = ObjectStore::table_for_object_type(realm->read_group(), c_sync_userMetadata);
    Query query = table->where().equal(schema.idx_identity, identity).equal(schema.idx_auth_server_url, url);
    Results results(realm, std::move(query));
    REALM_ASSERT_DEBUG(results.size() < 2);
    auto row = results.first();

    if (!row) {
        if (!make_if_absent)
            return none;

        realm->begin_transaction();
        // Check the results again.
        row = results.first();
        if (!row) {
<<<<<<< HEAD
            auto row = table->get(table->add_empty_row());
            std::string uuid = util::uuid_string();
            row.set_string(schema.idx_identity, identity);
            row.set_string(schema.idx_auth_server_url, url);
            row.set_string(schema.idx_local_uuid, uuid);
            row.set_bool(schema.idx_user_is_admin, false);
            row.set_bool(schema.idx_marked_for_removal, false);
            realm->commit_transaction();
            return SyncUserMetadata(schema, std::move(realm), std::move(row));
        } else {
            // Someone beat us to adding this user.
            if (row->get_bool(schema.idx_marked_for_removal)) {
                // User is dead. Revive or return none.
                if (make_if_absent) {
                    row->set_bool(schema.idx_marked_for_removal, false);
=======
            auto obj = table->create_object();
            std::string uuid = util::uuid_string();
            obj.set(schema.idx_identity, identity);
            obj.set(schema.idx_auth_server_url, url);
            obj.set(schema.idx_local_uuid, uuid);
            obj.set(schema.idx_user_is_admin, false);
            obj.set(schema.idx_marked_for_removal, false);
            realm->commit_transaction();
            return SyncUserMetadata(schema, std::move(realm), std::move(obj));
        } else {
            // Someone beat us to adding this user.
            if (row->get<bool>(schema.idx_marked_for_removal)) {
                // User is dead. Revive or return none.
                if (make_if_absent) {
                    row->set(schema.idx_marked_for_removal, false);
>>>>>>> origin/develop12
                    realm->commit_transaction();
                } else {
                    realm->cancel_transaction();
                    return none;
                }
            } else {
                // User is alive, nothing else to do.
                realm->cancel_transaction();
            }
            return SyncUserMetadata(schema, std::move(realm), std::move(*row));
        }
    }

    // Got an existing user.
<<<<<<< HEAD
    if (row->get_bool(schema.idx_marked_for_removal)) {
        // User is dead. Revive or return none.
        if (make_if_absent) {
            realm->begin_transaction();
            row->set_bool(schema.idx_marked_for_removal, false);
=======
    if (row->get<bool>(schema.idx_marked_for_removal)) {
        // User is dead. Revive or return none.
        if (make_if_absent) {
            realm->begin_transaction();
            row->set(schema.idx_marked_for_removal, false);
>>>>>>> origin/develop12
            realm->commit_transaction();
        } else {
            return none;
        }
    }
    return SyncUserMetadata(schema, std::move(realm), std::move(*row));
}

void SyncMetadataManager::make_file_action_metadata(StringData original_name,
                                                    StringData url,
                                                    StringData local_uuid,
                                                    SyncFileActionMetadata::Action action,
                                                    StringData new_name) const
{
    // This function can't use get_shared_realm() because it's called on a
    // background thread and that's currently not supported by the libuv
    // implementation of EventLoopSignal
<<<<<<< HEAD
    std::unique_ptr<Replication> history;
    std::unique_ptr<SharedGroup> shared_group;
    std::unique_ptr<Group> read_only_group;
    Realm::open_with_config(m_metadata_config, history, shared_group, read_only_group, nullptr);

    // Retrieve or create the row for this object.
    WriteTransaction wt(*shared_group);
    TableRef table = ObjectStore::table_for_object_type(wt.get_group(), c_sync_fileActionMetadata);

    auto& schema = m_file_action_schema;
    size_t row_idx = table->find_first_string(schema.idx_original_name, original_name);
    if (row_idx == not_found) {
        row_idx = table->add_empty_row();
        table->set_string(schema.idx_original_name, row_idx, original_name);
    }
    table->set_string(schema.idx_new_name, row_idx, new_name);
    table->set_int(schema.idx_action, row_idx, static_cast<int64_t>(action));
    table->set_string(schema.idx_url, row_idx, url);
    table->set_string(schema.idx_user_identity, row_idx, local_uuid);
    wt.commit();
=======
    auto coordinator = _impl::RealmCoordinator::get_coordinator(m_metadata_config);
    auto group_ptr = coordinator->begin_read();
    auto& group = *group_ptr;
    REALM_ASSERT(typeid(group) == typeid(Transaction));
    auto& transaction = static_cast<Transaction&>(group);
    transaction.promote_to_write();

    // Retrieve or create the row for this object.
    TableRef table = ObjectStore::table_for_object_type(group, c_sync_fileActionMetadata);

    auto& schema = m_file_action_schema;
    Obj obj = table->create_object_with_primary_key(original_name);

    obj.set(schema.idx_new_name, new_name);
    obj.set(schema.idx_action, static_cast<int64_t>(action));
    obj.set(schema.idx_url, url);
    obj.set(schema.idx_user_identity, local_uuid);
    transaction.commit();
>>>>>>> origin/develop12
}

util::Optional<SyncFileActionMetadata> SyncMetadataManager::get_file_action_metadata(StringData original_name) const
{
    auto realm = get_realm();
    auto& schema = m_file_action_schema;
    TableRef table = ObjectStore::table_for_object_type(realm->read_group(), c_sync_fileActionMetadata);
<<<<<<< HEAD
    size_t row_idx = table->find_first_string(schema.idx_original_name, original_name);
    if (row_idx == not_found)
        return none;

    return SyncFileActionMetadata(std::move(schema), std::move(realm), table->get(row_idx));
=======
    auto row_idx = table->find_first_string(schema.idx_original_name, original_name);
    if (!row_idx)
        return none;

    return SyncFileActionMetadata(std::move(schema), std::move(realm), table->get_object(row_idx));
>>>>>>> origin/develop12
}

std::shared_ptr<Realm> SyncMetadataManager::get_realm() const
{
    auto realm = Realm::get_shared_realm(m_metadata_config);
    realm->refresh();
    return realm;
}

// MARK: - Sync user metadata

<<<<<<< HEAD
SyncUserMetadata::SyncUserMetadata(Schema schema, SharedRealm realm, RowExpr row)
: m_realm(std::move(realm))
, m_schema(std::move(schema))
, m_row(row)
=======
SyncUserMetadata::SyncUserMetadata(Schema schema, SharedRealm realm, const Obj& obj)
: m_realm(std::move(realm))
, m_schema(std::move(schema))
, m_obj(obj)
>>>>>>> origin/develop12
{ }

std::string SyncUserMetadata::identity() const
{
    REALM_ASSERT(m_realm);
    m_realm->verify_thread();
<<<<<<< HEAD
    return m_row.get_string(m_schema.idx_identity);
=======
    m_realm->refresh();
    return m_obj.get<String>(m_schema.idx_identity);
>>>>>>> origin/develop12
}

std::string SyncUserMetadata::local_uuid() const
{
    REALM_ASSERT(m_realm);
    m_realm->verify_thread();
<<<<<<< HEAD
    return m_row.get_string(m_schema.idx_local_uuid);
=======
    m_realm->refresh();
    return m_obj.get<String>(m_schema.idx_local_uuid);
>>>>>>> origin/develop12
}

util::Optional<std::string> SyncUserMetadata::user_token() const
{
    REALM_ASSERT(m_realm);
    m_realm->verify_thread();
<<<<<<< HEAD
    StringData result = m_row.get_string(m_schema.idx_user_token);
=======
    m_realm->refresh();
    StringData result = m_obj.get<String>(m_schema.idx_user_token);
>>>>>>> origin/develop12
    return result.is_null() ? util::none : util::make_optional(std::string(result));
}

std::string SyncUserMetadata::auth_server_url() const
{
    REALM_ASSERT(m_realm);
    m_realm->verify_thread();
<<<<<<< HEAD
    return m_row.get_string(m_schema.idx_auth_server_url);
=======
    m_realm->refresh();
    return m_obj.get<String>(m_schema.idx_auth_server_url);
>>>>>>> origin/develop12
}

bool SyncUserMetadata::is_admin() const
{
    REALM_ASSERT(m_realm);
    m_realm->verify_thread();
<<<<<<< HEAD
    return m_row.get_bool(m_schema.idx_user_is_admin);
=======
    m_realm->refresh();
    return m_obj.get<bool>(m_schema.idx_user_is_admin);
>>>>>>> origin/develop12
}

void SyncUserMetadata::set_user_token(util::Optional<std::string> user_token)
{
    if (m_invalid)
        return;

    REALM_ASSERT_DEBUG(m_realm);
    m_realm->verify_thread();
    m_realm->begin_transaction();
<<<<<<< HEAD
    m_row.set_string(m_schema.idx_user_token, *user_token);
=======
    m_obj.set(m_schema.idx_user_token, *user_token);
>>>>>>> origin/develop12
    m_realm->commit_transaction();
}

void SyncUserMetadata::set_is_admin(bool is_admin)
{
    if (m_invalid)
        return;

    REALM_ASSERT_DEBUG(m_realm);
    m_realm->verify_thread();
    m_realm->begin_transaction();
<<<<<<< HEAD
    m_row.set_bool(m_schema.idx_user_is_admin, is_admin);
=======
    m_obj.set(m_schema.idx_user_is_admin, is_admin);
>>>>>>> origin/develop12
    m_realm->commit_transaction();
}

void SyncUserMetadata::mark_for_removal()
{
    if (m_invalid)
        return;

    m_realm->verify_thread();
    m_realm->begin_transaction();
<<<<<<< HEAD
    m_row.set_bool(m_schema.idx_marked_for_removal, true);
=======
    m_obj.set(m_schema.idx_marked_for_removal, true);
>>>>>>> origin/develop12
    m_realm->commit_transaction();
}

void SyncUserMetadata::remove()
{
    m_invalid = true;
    m_realm->begin_transaction();
<<<<<<< HEAD
    TableRef table = ObjectStore::table_for_object_type(m_realm->read_group(), c_sync_userMetadata);
    table->move_last_over(m_row.get_index());
=======
    m_obj.remove();
>>>>>>> origin/develop12
    m_realm->commit_transaction();
    m_realm = nullptr;
}

// MARK: - File action metadata

<<<<<<< HEAD
SyncFileActionMetadata::SyncFileActionMetadata(Schema schema, SharedRealm realm, RowExpr row)
: m_realm(std::move(realm))
, m_schema(std::move(schema))
, m_row(row)
=======
SyncFileActionMetadata::SyncFileActionMetadata(Schema schema, SharedRealm realm, const Obj& obj)
: m_realm(std::move(realm))
, m_schema(std::move(schema))
, m_obj(obj)
>>>>>>> origin/develop12
{ }

std::string SyncFileActionMetadata::original_name() const
{
    REALM_ASSERT(m_realm);
    m_realm->verify_thread();
<<<<<<< HEAD
    return m_row.get_string(m_schema.idx_original_name);
=======
    m_realm->refresh();
    return m_obj.get<String>(m_schema.idx_original_name);
>>>>>>> origin/develop12
}

util::Optional<std::string> SyncFileActionMetadata::new_name() const
{
    REALM_ASSERT(m_realm);
    m_realm->verify_thread();
<<<<<<< HEAD
    StringData result = m_row.get_string(m_schema.idx_new_name);
=======
    m_realm->refresh();
    StringData result =m_obj.get<String>(m_schema.idx_new_name);
>>>>>>> origin/develop12
    return result.is_null() ? util::none : util::make_optional(std::string(result));
}

std::string SyncFileActionMetadata::user_local_uuid() const
{
    REALM_ASSERT(m_realm);
    m_realm->verify_thread();
<<<<<<< HEAD
    return m_row.get_string(m_schema.idx_user_identity);
=======
    m_realm->refresh();
    return m_obj.get<String>(m_schema.idx_user_identity);
>>>>>>> origin/develop12
}

SyncFileActionMetadata::Action SyncFileActionMetadata::action() const
{
    REALM_ASSERT(m_realm);
    m_realm->verify_thread();
<<<<<<< HEAD
    return static_cast<SyncFileActionMetadata::Action>(m_row.get_int(m_schema.idx_action));
=======
    m_realm->refresh();
    return static_cast<SyncFileActionMetadata::Action>(m_obj.get<Int>(m_schema.idx_action));
>>>>>>> origin/develop12
}

std::string SyncFileActionMetadata::url() const
{
    REALM_ASSERT(m_realm);
    m_realm->verify_thread();
<<<<<<< HEAD
    return m_row.get_string(m_schema.idx_url);
=======
    m_realm->refresh();
    return m_obj.get<String>(m_schema.idx_url);
>>>>>>> origin/develop12
}

void SyncFileActionMetadata::remove()
{
    REALM_ASSERT(m_realm);
    m_realm->verify_thread();
    m_realm->begin_transaction();
<<<<<<< HEAD
    TableRef table = ObjectStore::table_for_object_type(m_realm->read_group(), c_sync_fileActionMetadata);
    table->move_last_over(m_row.get_index());
=======
    m_obj.remove();
>>>>>>> origin/develop12
    m_realm->commit_transaction();
    m_realm = nullptr;
}

} // namespace realm
