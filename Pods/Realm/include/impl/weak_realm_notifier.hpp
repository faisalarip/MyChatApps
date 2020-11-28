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

#ifndef REALM_WEAK_REALM_NOTIFIER_HPP
#define REALM_WEAK_REALM_NOTIFIER_HPP

<<<<<<< HEAD
#include "execution_context_id.hpp"

=======
>>>>>>> origin/develop12
#include <memory>
#include <thread>

namespace realm {
class Realm;

namespace util {
<<<<<<< HEAD
template<typename> class EventLoopSignal;
=======
class Scheduler;
>>>>>>> origin/develop12
}

namespace _impl {
// WeakRealmNotifier stores a weak reference to a Realm instance, along with all of
// the information about a Realm that needs to be accessed from other threads.
// This is needed to avoid forming strong references to the Realm instances on
// other threads, which can produce deadlocks when the last strong reference to
// a Realm instance is released from within a function holding the cache lock.
class WeakRealmNotifier {
public:
<<<<<<< HEAD
    WeakRealmNotifier(const std::shared_ptr<Realm>& realm, bool cache, bool bind_to_context);
=======
    WeakRealmNotifier(const std::shared_ptr<Realm>& realm, bool cache);
>>>>>>> origin/develop12
    ~WeakRealmNotifier();

    // Get a strong reference to the cached realm
    std::shared_ptr<Realm> realm() const { return m_realm.lock(); }

<<<<<<< HEAD
    // Does this WeakRealmNotifier store a Realm instance that should be used on the current thread?
    bool is_cached_for_execution_context(const AnyExecutionContextID& execution_context) const
    {
        return m_cache && m_execution_context == execution_context;
    }

=======
>>>>>>> origin/develop12
    // Has the Realm instance been destroyed?
    bool expired() const { return m_realm.expired(); }

    // Is this a WeakRealmNotifier for the given Realm instance?
    bool is_for_realm(Realm* realm) const { return realm == m_realm_key; }
<<<<<<< HEAD

    void notify();

    void bind_to_execution_context(AnyExecutionContextID context);

private:
    std::weak_ptr<Realm> m_realm;
    AnyExecutionContextID m_execution_context;
    void* m_realm_key;
    bool m_cache = false;

    struct Callback {
        const std::weak_ptr<Realm> weak_realm;
        void operator()() const;
    };
    std::shared_ptr<util::EventLoopSignal<Callback>> m_signal;
=======
    bool is_cached_for_scheduler(std::shared_ptr<util::Scheduler> scheduler) const;
    bool scheduler_is_on_thread() const;

    // Invoke m_realm.notify() on the Realm's thread via the scheduler.
    void notify();

    // Bind this notifier to the Realm's scheduler.
    void bind_to_scheduler();

private:
    std::weak_ptr<Realm> m_realm;
    void* m_realm_key;
    bool m_cache = false;
    std::shared_ptr<util::Scheduler> m_scheduler;
>>>>>>> origin/develop12
};

} // namespace _impl
} // namespace realm

#endif // REALM_WEAK_REALM_NOTIFIER_HPP
