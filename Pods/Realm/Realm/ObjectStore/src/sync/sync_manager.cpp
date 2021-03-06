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

#include "sync/sync_manager.hpp"

#include "sync/impl/sync_client.hpp"
#include "sync/impl/sync_file.hpp"
#include "sync/impl/sync_metadata.hpp"
#include "sync/sync_session.hpp"
#include "sync/sync_user.hpp"
<<<<<<< HEAD
=======
#include "util/uuid.hpp"
>>>>>>> origin/develop12

using namespace realm;
using namespace realm::_impl;

constexpr const char SyncManager::c_admin_identity[];

SyncManager& SyncManager::shared()
{
    // The singleton is heap-allocated in order to fix an issue when running unit tests where tests would crash after
    // they were done running because the manager was destroyed too early.
    static SyncManager& manager = *new SyncManager;
    return manager;
}

void SyncManager::configure(SyncClientConfig config)
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_config = std::move(config);
        if (m_sync_client)
            return;
    }

    struct UserCreationData {
        std::string identity;
        std::string user_token;
        std::string server_url;
        bool is_admin;
    };

    std::vector<UserCreationData> users_to_add;
    {
        std::lock_guard<std::mutex> lock(m_file_system_mutex);

        // Set up the file manager.
        if (m_file_manager) {
            // Changing the base path for tests requires calling reset_for_testing()
            // first, and otherwise isn't supported
            REALM_ASSERT(m_file_manager->base_path() == m_config.base_file_path);
        } else {
            m_file_manager = std::make_unique<SyncFileManager>(m_config.base_file_path);
        }

        // Set up the metadata manager, and perform initial loading/purging work.
        if (m_metadata_manager || m_config.metadata_mode == MetadataMode::NoMetadata) {
<<<<<<< HEAD
=======
            // No metadata means we use a new client uuid each time
            if (!m_metadata_manager)
                m_client_uuid = uuid_string();
>>>>>>> origin/develop12
            return;
        }

        bool encrypt = m_config.metadata_mode == MetadataMode::Encryption;
        try {
            m_metadata_manager = std::make_unique<SyncMetadataManager>(m_file_manager->metadata_path(),
                                                                       encrypt,
                                                                       m_config.custom_encryption_key);
        } catch (RealmFileException const& ex) {
            if (m_config.reset_metadata_on_error && m_file_manager->remove_metadata_realm()) {
                m_metadata_manager = std::make_unique<SyncMetadataManager>(m_file_manager->metadata_path(),
                                                                           encrypt,
                                                                           std::move(m_config.custom_encryption_key));
            } else {
                throw;
            }
        }

        REALM_ASSERT(m_metadata_manager);
        m_client_uuid = m_metadata_manager->client_uuid();

        // Perform our "on next startup" actions such as deleting Realm files
        // which we couldn't delete immediately due to them being in use
        std::vector<SyncFileActionMetadata> completed_actions;
        SyncFileActionMetadataResults file_actions = m_metadata_manager->all_pending_actions();
        for (size_t i = 0; i < file_actions.size(); i++) {
            auto file_action = file_actions.get(i);
            if (run_file_action(file_action)) {
                completed_actions.emplace_back(std::move(file_action));
            }
        }
        for (auto& action : completed_actions) {
            action.remove();
        }

        // Load persisted users into the users map.
        SyncUserMetadataResults users = m_metadata_manager->all_unmarked_users();
        for (size_t i = 0; i < users.size(); i++) {
            // Note that 'admin' style users are not persisted.
            auto user_data = users.get(i);
            if (auto user_token = user_data.user_token()) {
                users_to_add.push_back(UserCreationData{
                    user_data.identity(),
                    std::move(*user_token),
                    user_data.auth_server_url(),
                    user_data.is_admin()
                });
            }
        }

        // Delete any users marked for death.
        std::vector<SyncUserMetadata> dead_users;
        SyncUserMetadataResults users_to_remove = m_metadata_manager->all_users_marked_for_removal();
        dead_users.reserve(users_to_remove.size());
        for (size_t i = 0; i < users_to_remove.size(); i++) {
            auto user = users_to_remove.get(i);
            // FIXME: delete user data in a different way? (This deletes a logged-out user's data as soon as the app
            // launches again, which might not be how some apps want to treat their data.)
            try {
                m_file_manager->remove_user_directory(user.local_uuid());
                dead_users.emplace_back(std::move(user));
            } catch (util::File::AccessError const&) {
                continue;
            }
        }
        for (auto& user : dead_users) {
            user.remove();
        }
    }
    {
        std::lock_guard<std::mutex> lock(m_user_mutex);
        for (auto& user_data : users_to_add) {
            auto& identity = user_data.identity;
            auto& server_url = user_data.server_url;
            auto user = std::make_shared<SyncUser>(user_data.user_token, identity, server_url);
            user->set_is_admin(user_data.is_admin);
            m_users.insert({ {identity, server_url}, std::move(user) });
        }
    }
}

bool SyncManager::immediately_run_file_actions(const std::string& realm_path)
{
    if (!m_metadata_manager) {
        return false;
    }
    if (auto metadata = m_metadata_manager->get_file_action_metadata(realm_path)) {
        if (run_file_action(*metadata)) {
            metadata->remove();
            return true;
        }
    }
    return false;
}

// Perform a file action. Returns whether or not the file action can be removed.
bool SyncManager::run_file_action(const SyncFileActionMetadata& md)
{
    switch (md.action()) {
        case SyncFileActionMetadata::Action::DeleteRealm:
            // Delete all the files for the given Realm.
            m_file_manager->remove_realm(md.original_name());
            return true;
        case SyncFileActionMetadata::Action::BackUpThenDeleteRealm:
            // Copy the primary Realm file to the recovery dir, and then delete the Realm.
            auto new_name = md.new_name();
            auto original_name = md.original_name();
            if (!util::File::exists(original_name)) {
                // The Realm file doesn't exist anymore.
                return true;
            }
            if (new_name && !util::File::exists(*new_name) && m_file_manager->copy_realm_file(original_name, *new_name)) {
                // We successfully copied the Realm file to the recovery directory.
                m_file_manager->remove_realm(original_name);
                return true;
            }
            return false;
    }
    return false;
}

void SyncManager::reset_for_testing()
{
    std::lock_guard<std::mutex> lock(m_file_system_mutex);
    m_file_manager = nullptr;
    m_metadata_manager = nullptr;
    m_client_uuid = util::none;

    {
        // Destroy all the users.
        std::lock_guard<std::mutex> lock(m_user_mutex);
        m_users.clear();
        m_admin_token_users.clear();
    }
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        // Stop the client. This will abort any uploads that inactive sessions are waiting for.
        if (m_sync_client)
            m_sync_client->stop();

        {
            std::lock_guard<std::mutex> lock(m_session_mutex);
            // Callers of `SyncManager::reset_for_testing` should ensure there are no existing sessions
            // prior to calling `reset_for_testing`.
            bool no_sessions = !do_has_existing_sessions();
            REALM_ASSERT_RELEASE(no_sessions);

            // Destroy any inactive sessions.
            // FIXME: We shouldn't have any inactive sessions at this point! Sessions are expected to
            // remain inactive until their final upload completes, at which point they are unregistered
            // and destroyed. Our call to `sync::Client::stop` above aborts all uploads, so all sessions
            // should have already been destroyed.
            m_sessions.clear();
        }

        // Destroy the client now that we have no remaining sessions.
        m_sync_client = nullptr;

        // Reset even more state.
        m_config = {};
    }
}

void SyncManager::set_log_level(util::Logger::Level level) noexcept
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_config.log_level = level;
}

void SyncManager::set_logger_factory(SyncLoggerFactory& factory) noexcept
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_config.logger_factory = &factory;
}

std::unique_ptr<util::Logger> SyncManager::make_logger() const
{
    if (m_config.logger_factory) {
        return m_config.logger_factory->make_logger(m_config.log_level); // Throws
    }

    auto stderr_logger = std::make_unique<util::StderrLogger>(); // Throws
    stderr_logger->set_level_threshold(m_config.log_level);
    return std::unique_ptr<util::Logger>(stderr_logger.release());
}

void SyncManager::set_user_agent(std::string user_agent)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_config.user_agent_application_info = std::move(user_agent);
}

void SyncManager::set_timeouts(SyncClientTimeouts timeouts)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_config.timeouts = timeouts;
}

void SyncManager::reconnect()
{
    std::lock_guard<std::mutex> lock(m_session_mutex);
    for (auto& it : m_sessions) {
        it.second->handle_reconnect();
    }
}

util::Logger::Level SyncManager::log_level() const noexcept
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_config.log_level;
}

bool SyncManager::perform_metadata_update(std::function<void(const SyncMetadataManager&)> update_function) const
{
    std::lock_guard<std::mutex> lock(m_file_system_mutex);
    if (!m_metadata_manager) {
        return false;
    }
    update_function(*m_metadata_manager);
    return true;
}

std::shared_ptr<SyncUser> SyncManager::get_user(const SyncUserIdentifier& identifier, std::string refresh_token)
{
    std::lock_guard<std::mutex> lock(m_user_mutex);
    auto it = m_users.find(identifier);
    if (it == m_users.end()) {
        // No existing user.
        auto new_user = std::make_shared<SyncUser>(std::move(refresh_token),
                                                   identifier.user_id,
                                                   identifier.auth_server_url,
                                                   none,
                                                   SyncUser::TokenType::Normal);
        m_users.insert({ identifier, new_user });
        return new_user;
    } else {
        auto user = it->second;
        if (user->state() == SyncUser::State::Error) {
            return nullptr;
        }
        user->update_refresh_token(std::move(refresh_token));
        return user;
    }
}

std::shared_ptr<SyncUser> SyncManager::get_admin_token_user_from_identity(const std::string& identity,
                                                                          util::Optional<std::string> server_url,
                                                                          const std::string& token)
{
    if (server_url)
        return get_admin_token_user(*server_url, token, identity);

    std::lock_guard<std::mutex> lock(m_user_mutex);
    // Look up the user based off the identity.
    // No server URL, so no migration possible.
    auto it = m_admin_token_users.find(identity);
    if (it == m_admin_token_users.end()) {
        // No existing user.
        auto new_user = std::make_shared<SyncUser>(token,
                                                   c_admin_identity,
                                                   std::move(server_url),
                                                   identity,
                                                   SyncUser::TokenType::Admin);
        m_admin_token_users.insert({ identity, new_user });
        return new_user;
    } else {
        return it->second;
    }
}

std::shared_ptr<SyncUser> SyncManager::get_admin_token_user(const std::string& server_url,
                                                            const std::string& token,
                                                            util::Optional<std::string> old_identity)
{
    std::shared_ptr<SyncUser> user;
    {
        std::lock_guard<std::mutex> lock(m_user_mutex);
        // Look up the user based off the server URL.
        auto it = m_admin_token_users.find(server_url);
        if (it != m_admin_token_users.end())
            return it->second;

        // No existing user.
        user = std::make_shared<SyncUser>(token,
                                          c_admin_identity,
                                          server_url,
                                          c_admin_identity + server_url,
                                          SyncUser::TokenType::Admin);
        m_admin_token_users.insert({ server_url, user });
    }
    if (old_identity) {
        // Try renaming the user's directory to use our new naming standard, if applicable.
        std::lock_guard<std::mutex> fm_lock(m_file_system_mutex);
        if (m_file_manager)
            m_file_manager->try_rename_user_directory(*old_identity, c_admin_identity + server_url);
    }
    return user;
}

std::vector<std::shared_ptr<SyncUser>> SyncManager::all_logged_in_users() const
{
    std::lock_guard<std::mutex> lock(m_user_mutex);
    std::vector<std::shared_ptr<SyncUser>> users;
    users.reserve(m_users.size() + m_admin_token_users.size());
    for (auto& it : m_users) {
        auto user = it.second;
        if (user->state() == SyncUser::State::Active) {
            users.emplace_back(std::move(user));
        }
    }
    for (auto& it : m_admin_token_users) {
        users.emplace_back(std::move(it.second));
    }
    return users;
}

std::shared_ptr<SyncUser> SyncManager::get_current_user() const
{
    std::lock_guard<std::mutex> lock(m_user_mutex);

    auto is_active_user = [](auto& el) { return el.second->state() == SyncUser::State::Active; };
    auto it = std::find_if(m_users.begin(), m_users.end(), is_active_user);
    if (it == m_users.end())
        return nullptr;

    if (std::find_if(std::next(it), m_users.end(), is_active_user) != m_users.end())
        throw std::logic_error("Current user is not valid if more that one valid, logged-in user exists.");

    return it->second;
}

std::shared_ptr<SyncUser> SyncManager::get_existing_logged_in_user(const SyncUserIdentifier& identifier) const
{
    std::lock_guard<std::mutex> lock(m_user_mutex);
    auto it = m_users.find(identifier);
    if (it == m_users.end())
        return nullptr;

    auto user = it->second;
    return user->state() == SyncUser::State::Active ? user : nullptr;
}

std::string SyncManager::path_for_realm(const SyncUser& user, const std::string& raw_realm_url) const
{
    std::lock_guard<std::mutex> lock(m_file_system_mutex);
    REALM_ASSERT(m_file_manager);
    return m_file_manager->path(user.local_identity(), raw_realm_url);
}

std::string SyncManager::recovery_directory_path(util::Optional<std::string> const& custom_dir_name) const
{
    std::lock_guard<std::mutex> lock(m_file_system_mutex);
    REALM_ASSERT(m_file_manager);
    return m_file_manager->recovery_directory_path(custom_dir_name);
}

std::shared_ptr<SyncSession> SyncManager::get_existing_active_session(const std::string& path) const
{
    std::lock_guard<std::mutex> lock(m_session_mutex);
    if (auto session = get_existing_session_locked(path)) {
        if (auto external_reference = session->existing_external_reference())
            return external_reference;
    }
    return nullptr;
}

std::shared_ptr<SyncSession> SyncManager::get_existing_session_locked(const std::string& path) const
{
    REALM_ASSERT(!m_session_mutex.try_lock());
    auto it = m_sessions.find(path);
    return it == m_sessions.end() ? nullptr : it->second;
}

std::shared_ptr<SyncSession> SyncManager::get_existing_session(const std::string& path) const
{
    std::lock_guard<std::mutex> lock(m_session_mutex);
    if (auto session = get_existing_session_locked(path))
        return session->external_reference();

    return nullptr;
}

std::shared_ptr<SyncSession> SyncManager::get_session(const std::string& path, const SyncConfig& sync_config, bool force_client_resync)
{
    auto& client = get_sync_client(); // Throws

    std::lock_guard<std::mutex> lock(m_session_mutex);
    if (auto session = get_existing_session_locked(path)) {
        sync_config.user->register_session(session);
        return session->external_reference();
    }

    auto shared_session = SyncSession::create(client, path, sync_config, force_client_resync);
    m_sessions[path] = shared_session;

    // Create the external reference immediately to ensure that the session will become
    // inactive if an exception is thrown in the following code.
    auto external_reference = shared_session->external_reference();

    sync_config.user->register_session(std::move(shared_session));

    return external_reference;
}


bool SyncManager::has_existing_sessions()
{
    std::lock_guard<std::mutex> lock(m_session_mutex);
    return do_has_existing_sessions();
}

bool SyncManager::do_has_existing_sessions(){
    return std::any_of(m_sessions.begin(), m_sessions.end(), [](auto& element){
        return element.second->existing_external_reference();
    });
}

void SyncManager::unregister_session(const std::string& path)
{
    std::lock_guard<std::mutex> lock(m_session_mutex);
    auto it = m_sessions.find(path);
    REALM_ASSERT(it != m_sessions.end());

    // If the session has an active external reference, leave it be. This will happen if the session
    // moves to an inactive state while still externally reference, for instance, as a result of
    // the session's user being logged out.
    if (it->second->existing_external_reference())
        return;

    m_sessions.erase(path);
}

void SyncManager::enable_session_multiplexing()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_config.multiplex_sessions)
        return; // Already enabled, we can ignore

    if (m_sync_client)
        throw std::logic_error("Cannot enable session multiplexing after creating the sync client");

    m_config.multiplex_sessions = true;
}

SyncClient& SyncManager::get_sync_client() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_sync_client)
        m_sync_client = create_sync_client(); // Throws
    return *m_sync_client;
}

std::unique_ptr<SyncClient> SyncManager::create_sync_client() const
{
    REALM_ASSERT(!m_mutex.try_lock());
    return std::make_unique<SyncClient>(make_logger(), m_config);
}

std::string SyncManager::client_uuid() const
{
    REALM_ASSERT(m_client_uuid);
    return *m_client_uuid;
}
