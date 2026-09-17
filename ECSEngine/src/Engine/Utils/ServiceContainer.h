// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include <any>
#include <functional>
#include <memory>
#include <stdexcept>
#include <typeindex>
#include <unordered_map>
#include <vector>

class ServiceContainer;

// ---------------------------------------------------------------------
// Automatic dependency resolution (the "universal argument" trick)
//
// C++ has no reflection, so there is no direct way to ask "how many constructor parameters does T take, and what are their types?".
// UniversalArg is a dummy argument that implicitly converts to any reference or pointer type.
// Overloading it against the real constructor T(A&, B&, ...) forces the compiler to deduce, per parameter,
// which concrete type is being requested (the template parameter T of operator T&() is deduced from context).
// That deduced type is forwarded straight into container.Resolve<T>(), which recursively resolves the rest of the dependency graph.
// ---------------------------------------------------------------------
//
// Migration note: once C++26 std::meta is available on MSVC,
// only the contents of this detail namespace (the UniversalArg machinery) need to be replaced with a std::meta-based implementation.
namespace detail
{

// Self is the type currently being constructed. Excluding it is  required: without it,
// UniversalArg would also be a viable conversion target for T's own copy/move constructor, making the constructor call ambiguous.
template <typename Self>
struct UniversalArg
{
    ServiceContainer& container;

    // Defined out-of-line, after ServiceContainer is complete.
    template <typename T, typename = std::enable_if_t<
        std::is_class_v<T> && !std::is_same_v<std::decay_t<T>, Self>>>
    operator T& () const;

    template <typename T, typename = std::enable_if_t<
        std::is_class_v<T> && !std::is_same_v<std::decay_t<T>, Self>>>
    operator T* () const;
};

// Is T constructible from N UniversalArg values?
template <typename T, std::size_t... Is>
constexpr bool IsConstructibleWithArgs(std::index_sequence<Is...>)
{
    return std::is_constructible_v<T, decltype(((void)Is, std::declval<UniversalArg<T>>()))...>;
}

constexpr std::size_t kMaxConstructorArgs = 8;

// Walks N down from kMaxConstructorArgs to 0 and returns the largest N for which T is constructible.
// This must use "if constexpr" rather than a ternary: both branches of a ternary get instantiated for type-checking regardless of which one is selected,
// so a class-template version of this recursion would keep instantiating down to 0 and hit the static_assert even after a higher N already matched.
template <typename T, std::size_t N>
constexpr std::size_t DeduceConstructorArity()
{
    if constexpr (IsConstructibleWithArgs<T>(std::make_index_sequence<N>{})) {
        return N;
    } else if constexpr (N == 0) {
        static_assert(N != 0,
                      "ServiceContainer: no viable constructor found for T "
                      "(tried up to kMaxConstructorArgs parameters, including default construction)."
                      "Make sure every dependency is registered, or raise kMaxConstructorArgs.");
        return 0;
    } else {
        return DeduceConstructorArity<T, N - 1>();
    }
}

template <typename T, std::size_t... Is>
std::shared_ptr<T> ConstructWithArgs(ServiceContainer& container, std::index_sequence<Is...>)
{
    return std::make_shared<T>(((void)Is, UniversalArg<T>{container})...);
}

// Deduces T's constructor arguments and constructs it.
// Each ServiceList creator only needs to call this.
template <typename T>
std::shared_ptr<T> Instantiate(ServiceContainer& container)
{
    constexpr std::size_t arity = DeduceConstructorArity<T, kMaxConstructorArgs>();
    return ConstructWithArgs<T>(container, std::make_index_sequence<arity>{});
}

enum class Lifetime
{
    Singleton,
    Transient,
    Instance,
};

enum class RegistrationPolicy
{
    Forbid,
    AllowOverride,
};

struct ServiceFactory
{
    Lifetime lifetime;
    std::function<std::any(ServiceContainer&)> creator;
    std::any instance;
};

/**
 * @brief Owns the type -> factory map.
 * Add() is the only way to write to it, so registration always goes through duplicate-key checking.
 */
class FactoryTable
{
public:
    void Add(std::type_index key, RegistrationPolicy policy, ServiceFactory factory)
    {
        if (policy == RegistrationPolicy::Forbid && m_factories.count(key) != 0) {
            throw std::logic_error(
                std::string("ServiceList: duplicate registration for type: ") + key.name() +
                " (pass ServiceList::RegistrationPolicy::AllowOverride to replace it intentionally)");
        }
        m_factories.insert_or_assign(key, std::move(factory));
    }

    const std::unordered_map<std::type_index, ServiceFactory>& Raw() const { return m_factories; }

private:
    std::unordered_map<std::type_index, ServiceFactory> m_factories;
};

} // namespace detail

/**
 * @brief Registry of service factories (types, their lifetimes, and how to construct them).
 * Populated ahead of time, then handed to a ServiceContainer to actually resolve and construct services from.
 */
class ServiceList
{
public:
    using Lifetime = detail::Lifetime;
    using RegistrationPolicy = detail::RegistrationPolicy;
    using ServiceFactory = detail::ServiceFactory;

    // Registers a singleton service (constructor arguments are resolved automatically).
    template <typename T>
    void RegisterSingleton(RegistrationPolicy policy = RegistrationPolicy::Forbid)
    {
        RegisterSingleton<T, T>(policy);
    }

    // Registers an implementation against an interface.
    // e.g. list.RegisterSingleton<IRenderer, StandardRenderer>();
    template <typename Interface, typename Implementation>
    void RegisterSingleton(RegistrationPolicy policy = RegistrationPolicy::Forbid)
    {
        static_assert(std::is_base_of_v<Interface, Implementation> || std::is_same_v<Interface, Implementation>,
                      "Implementation must derive from Interface.");

        m_table.Add(typeid(Interface*), policy, ServiceFactory{
            .lifetime = Lifetime::Singleton,
            .creator = [](ServiceContainer& container) -> std::any {
                std::shared_ptr<Implementation> impl = detail::Instantiate<Implementation>(container);
                return std::shared_ptr<Interface>(impl);
            },
            .instance = {},
                    });
    }

    // Registers a transient service (a new instance is created on every fetch).
    template <typename T>
    void RegisterTransient(RegistrationPolicy policy = RegistrationPolicy::Forbid)
    {
        RegisterTransient<T, T>(policy);
    }

    template <typename Interface, typename Implementation>
    void RegisterTransient(RegistrationPolicy policy = RegistrationPolicy::Forbid)
    {
        static_assert(std::is_base_of_v<Interface, Implementation> || std::is_same_v<Interface, Implementation>,
                      "Implementation must derive from Interface.");

        m_table.Add(typeid(Interface*), policy, ServiceFactory{
            .lifetime = Lifetime::Transient,
            .creator = [](ServiceContainer& container) -> std::any {
                std::shared_ptr<Implementation> impl = detail::Instantiate<Implementation>(container);
                return std::shared_ptr<Interface>(impl);
            },
            .instance = {},
                    });
    }


    template <typename T>
    void RegisterInstance(T* instance, RegistrationPolicy policy = RegistrationPolicy::Forbid)
    {
        m_table.Add(typeid(T*), policy, ServiceFactory{
            .lifetime = Lifetime::Instance,
            .creator = nullptr,
            .instance = instance,
                    });
    }

    const std::unordered_map<std::type_index, ServiceFactory>& GetFactories() const { return m_table.Raw(); }

private:
    detail::FactoryTable m_table;
};

/**
 * @brief Resolves dependencies and provides services.
 *
 * Holds a copy of the registrations from a ServiceList,
 * so multiple ServiceContainer instances built from the same ServiceList (e.g. a
 * GlobalServiceContainer and per-world WorldServiceContainer) each get
 * their own independent set of singleton instances.
 */
class ServiceContainer
{
public:
    explicit ServiceContainer(const ServiceList& list)
        : m_factories(list.GetFactories())
    {
    }

    ServiceContainer(const ServiceContainer&) = delete;
    ServiceContainer& operator=(const ServiceContainer&) = delete;

    ~ServiceContainer()
    {
        // Singletons are destroyed in the reverse order they were created.
        // unordered_map does not guarantee destruction order,
        // so creation order is tracked separately and torn down explicitly here.
        for (auto it = m_creationOrder.rbegin(); it != m_creationOrder.rend(); ++it) {
            auto factoryIt = m_factories.find(*it);
            if (factoryIt != m_factories.end()) {
                factoryIt->second.instance.reset();
            }
        }
    }

    // Resolves T, automatically constructing its dependencies as needed.
    template <typename T>
    T& Resolve()
    {
        auto it = m_factories.find(typeid(T*));
        if (it == m_factories.end()) {
            throw std::runtime_error(std::string("Service not registered: ") + typeid(T*).name());
        }

        ServiceList::ServiceFactory& factory = it->second;

        switch (factory.lifetime) {
            case ServiceList::Lifetime::Instance:
                return *std::any_cast<T*>(factory.instance);

            case ServiceList::Lifetime::Singleton:
                if (!factory.instance.has_value()) {
                    factory.instance = factory.creator(*this);
                    m_creationOrder.push_back(typeid(T*));
                }
                return *std::any_cast<std::shared_ptr<T>>(factory.instance);

            case ServiceList::Lifetime::Transient:
            {
                // Built fresh on every call. With no owner,
                // the returned reference would dangle immediately,
                // so the instance is kept alive for the container's lifetime instead.
                std::any created = factory.creator(*this);
                auto ptr = std::any_cast<std::shared_ptr<T>>(created);
                m_transients.push_back(created);
                return *ptr;
            }
        }

        throw std::logic_error("ServiceContainer::Resolve<T>(): unhandled Lifetime value.");
    }

private:
    std::unordered_map<std::type_index, ServiceList::ServiceFactory> m_factories;
    std::vector<std::type_index> m_creationOrder;
    std::vector<std::any> m_transients;
};

namespace detail
{
template <typename Self>
template <typename T, typename>
UniversalArg<Self>::operator T& () const
{
    return container.Resolve<T>();
}

template <typename Self>
template <typename T, typename>
UniversalArg<Self>::operator T* () const
{
    return &container.Resolve<T>();
}
} // namespace detail
