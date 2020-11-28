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

#include "impl/weak_realm_notifier.hpp"

#include "shared_realm.hpp"
<<<<<<< HEAD
#include "util/event_loop_signal.hpp"
=======
#include "util/scheduler.hpp"
>>>>>>> origin/develop12

using namespace realm;
using namespace realm::_impl;

<<<<<<< HEAD
WeakRealmNotifier::WeakRealmNotifier(const std::shared_ptr<Realm>& realm, bool cache, bool bind_to_context)
: m_realm(realm)
, m_execution_context(realm->config().execution_context)
, m_realm_key(realm.get())
, m_cache(cache)
, m_signal(bind_to_context ? std::make_shared<util::EventLoopSignal<Callback>>(Callback{realm}) : nullptr)
{
=======

WeakRealmNotifier::WeakRealmNotifier(const std::shared_ptr<Realm>& realm, bool cache)
: m_realm(realm)
, m_realm_key(realm.get())
, m_cache(cache)
{
    bind_to_scheduler();
>>>>>>> origin/develop12
}

WeakRealmNotifier::~WeakRealmNotifier() = default;

<<<<<<< HEAD
void WeakRealmNotifier::Callback::operator()() const
{
    if (auto realm = weak_realm.lock()) {
        realm->notify();
    }
}

void WeakRealmNotifier::notify()
{
    if (m_signal)
        m_signal->notify();
}

void WeakRealmNotifier::bind_to_execution_context(AnyExecutionContextID context)
{
    REALM_ASSERT(!m_signal);
    m_signal = std::make_shared<util::EventLoopSignal<Callback>>(Callback{m_realm});
    m_execution_context = context;
=======
void WeakRealmNotifier::notify()
{
    if (m_scheduler)
        m_scheduler->notify();
}

void WeakRealmNotifier::bind_to_scheduler()
{
    REALM_ASSERT(!m_scheduler);
    m_scheduler = realm()->scheduler();
    if (m_scheduler) {
        m_scheduler->set_notify_callback([weak_realm = m_realm] {
            if (auto realm = weak_realm.lock()) {
                realm->notify();
            }
        });
    }
}

bool WeakRealmNotifier::is_cached_for_scheduler(std::shared_ptr<util::Scheduler> scheduler) const
{
    return m_cache && (m_scheduler && scheduler) && (m_scheduler->is_same_as(scheduler.get()));
}

bool WeakRealmNotifier::scheduler_is_on_thread() const
{
    return m_scheduler && m_scheduler->is_on_thread();
>>>>>>> origin/develop12
}
