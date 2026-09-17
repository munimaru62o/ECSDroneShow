#include "pch.h"
#include "Engine/Utils/ServiceContainer.h"

#include <stdexcept>
#include <string>
#include <vector>

// ---------------------------------------------------------
// 1. Basic Constructor Injection Test
//    Verifies that a service's dependency is resolved
//    automatically, and that the same singleton instance is
//    handed out on every subsequent Get<T>() call.
// ---------------------------------------------------------
namespace
{
struct InputManager
{
    int value = 42;
};

class Character
{
public:
    explicit Character(InputManager& input) : m_input(input) {}
    InputManager& GetInput() { return m_input; }

private:
    InputManager& m_input;
};
} // namespace

TEST(ServiceContainerTest, ConstructorInjectionResolvesDependency)
{
    ServiceList list;
    list.RegisterSingleton<InputManager>();
    list.RegisterSingleton<Character>();

    ServiceContainer container(list);
    Character& character = container.Resolve<Character>();

    EXPECT_EQ(character.GetInput().value, 42);

    // Get<InputManager>() should return the exact same instance that
    // was injected into Character, not a freshly constructed one.
    EXPECT_EQ(&container.Resolve<InputManager>(), &character.GetInput());
}

// ---------------------------------------------------------
// 2. Multi-Dependency Constructor Injection Test
//    Ensures each constructor parameter is matched to its own
//    dependency type, not just the first one registered.
// ---------------------------------------------------------
namespace
{
struct AudioManager
{
    bool muted = false;
};

class Hud
{
public:
    Hud(InputManager& input, AudioManager& audio) : m_input(input), m_audio(audio) {}
    InputManager& m_input;
    AudioManager& m_audio;
};
} // namespace

TEST(ServiceContainerTest, ResolvesMultipleConstructorArguments)
{
    ServiceList list;
    list.RegisterSingleton<InputManager>();
    list.RegisterSingleton<AudioManager>();
    list.RegisterSingleton<Hud>();

    ServiceContainer container(list);
    Hud& hud = container.Resolve<Hud>();

    EXPECT_EQ(&hud.m_input, &container.Resolve<InputManager>());
    EXPECT_EQ(&hud.m_audio, &container.Resolve<AudioManager>());
}

// ---------------------------------------------------------
// 3. Interface / Implementation Binding Test
//    Verifies that code depending on an interface receives
//    whichever concrete implementation was registered for it.
// ---------------------------------------------------------
namespace
{
struct IRenderer
{
    virtual ~IRenderer() = default;
    virtual const char* Name() const = 0;
};

struct StandardRenderer : IRenderer
{
    const char* Name() const override { return "StandardRenderer"; }
};

struct NullRenderer : IRenderer
{
    const char* Name() const override { return "NullRenderer"; }
};
} // namespace

TEST(ServiceContainerTest, InterfaceResolvesToRegisteredImplementation)
{
    ServiceList list;
    list.RegisterSingleton<IRenderer, StandardRenderer>();

    ServiceContainer container(list);

    // Get<IRenderer>() must return the concrete StandardRenderer,
    // even though the caller only ever names the interface type.
    EXPECT_STREQ(container.Resolve<IRenderer>().Name(), "StandardRenderer");
}

TEST(ServiceContainerTest, AllowOverrideReplacesPreviousRegistration)
{
    ServiceList list;
    list.RegisterSingleton<IRenderer, StandardRenderer>();
    list.RegisterSingleton<IRenderer, NullRenderer>(ServiceList::RegistrationPolicy::AllowOverride);

    ServiceContainer container(list);

    EXPECT_STREQ(container.Resolve<IRenderer>().Name(), "NullRenderer");
}

// ---------------------------------------------------------
// 4. Duplicate Registration Guard Test
//    Registering the same type twice without explicitly
//    allowing an override is treated as a mistake, not a
//    silent replacement.
// ---------------------------------------------------------
TEST(ServiceContainerTest, DuplicateRegistrationThrowsByDefault)
{
    ServiceList list;
    list.RegisterSingleton<IRenderer, StandardRenderer>();

    EXPECT_THROW((list.RegisterSingleton<IRenderer, NullRenderer>()), std::logic_error);
}

// ---------------------------------------------------------
// 5. Transient Lifetime Test
//    A Transient registration must hand back a distinct
//    instance on every Get<T>() call, unlike Singleton.
// ---------------------------------------------------------
namespace
{
struct RequestContext
{
    static inline int s_counter = 0;
    RequestContext() { m_id = ++s_counter; }
    int m_id;
};
} // namespace

TEST(ServiceContainerTest, TransientProducesDistinctInstances)
{
    ServiceList list;
    list.RegisterTransient<RequestContext>();

    ServiceContainer container(list);
    std::shared_ptr<RequestContext> first = container.CreateNew<RequestContext>();
    std::shared_ptr<RequestContext> second = container.CreateNew<RequestContext>();

    EXPECT_NE(first->m_id, second->m_id);
}

// A Transient dependency taken by reference/pointer would be a
// "captive dependency": once embedded in a longer-lived object it
// stops being transient in any meaningful sense. Resolve<T>() refuses
// this case outright rather than silently keeping the instance alive
// for the container's whole lifetime.
namespace
{
class RefHandler
{
public:
    explicit RefHandler(RequestContext& ctx) : m_ctx(ctx) {}
    RequestContext& m_ctx;
};
} // namespace

TEST(ServiceContainerTest, TransientByReferenceThrowsAsCaptiveDependency)
{
    ServiceList list;
    list.RegisterTransient<RequestContext>();
    list.RegisterSingleton<RefHandler>();

    ServiceContainer container(list);
    EXPECT_THROW(container.Resolve<RefHandler>(), std::logic_error);
}

// The supported way to inject a Transient dependency into another
// service is std::shared_ptr<T>: ownership then belongs to whichever
// object holds the shared_ptr, not to the container.
namespace
{
class SharedPtrHandler
{
public:
    explicit SharedPtrHandler(std::shared_ptr<RequestContext> ctx) : m_ctx(std::move(ctx)) {}
    std::shared_ptr<RequestContext> m_ctx;
};
} // namespace

TEST(ServiceContainerTest, TransientBySharedPtrIsOwnedByDependent)
{
    ServiceList list;
    list.RegisterTransient<RequestContext>();
    list.RegisterSingleton<SharedPtrHandler>();

    ServiceContainer container(list);
    SharedPtrHandler& handler1 = container.Resolve<SharedPtrHandler>();
    SharedPtrHandler& handler2 = container.Resolve<SharedPtrHandler>();

    // SharedPtrHandler is itself a Singleton, so the second Resolve()
    // returns the same instance and therefore the same RequestContext
    // it was constructed with - it is not recreated on every access.
    EXPECT_EQ(&handler1, &handler2);
    EXPECT_EQ(handler1.m_ctx.get(), handler2.m_ctx.get());
}

// ---------------------------------------------------------
// 6. Pre-Existing Instance Registration Test
//    Objects the container did not create (e.g. a window
//    handle owned elsewhere) can still be injected by pointer
//    into a service's constructor.
// ---------------------------------------------------------
namespace
{
struct WindowHandle
{
    int id = 7;
};

struct InputManagerWithWindow
{
    explicit InputManagerWithWindow(WindowHandle* window) : m_window(window) {}
    WindowHandle* m_window;
};
} // namespace

TEST(ServiceContainerTest, RegisterInstanceIsInjectedByPointer)
{
    WindowHandle window;

    ServiceList list;
    list.RegisterInstance<WindowHandle>(window);
    list.RegisterSingleton<InputManagerWithWindow>();

    ServiceContainer container(list);

    // The container must not have constructed a new WindowHandle; it
    // should be exactly the pre-existing one that was registered.
    EXPECT_EQ(container.Resolve<InputManagerWithWindow>().m_window, &window);
}

// ---------------------------------------------------------
// 7. Singleton Destruction Order Test
//    Singletons must be torn down in the reverse order they
//    were created, mirroring how stack-allocated objects
//    unwind.
// ---------------------------------------------------------
namespace
{
std::vector<std::string>* g_destructionLog = nullptr;

struct LoggedInputManager
{
    ~LoggedInputManager() { g_destructionLog->push_back("InputManager"); }
};

class LoggedCharacter
{
public:
    explicit LoggedCharacter(LoggedInputManager&) {}
    ~LoggedCharacter() { g_destructionLog->push_back("Character"); }
};
} // namespace

TEST(ServiceContainerTest, SingletonsAreDestroyedInReverseOrder)
{
    std::vector<std::string> log;
    g_destructionLog = &log;

    ServiceList list;
    list.RegisterSingleton<LoggedInputManager>();
    list.RegisterSingleton<LoggedCharacter>();

    {
        ServiceContainer container(list);
        // Requesting Character first forces InputManager to be
        // constructed first as its dependency, then Character.
        container.Resolve<LoggedCharacter>();
    } // container destroyed here

    // Destruction should be the exact reverse of construction order:
    // Character (constructed last) is destroyed first.
    ASSERT_EQ(log.size(), 2u);
    EXPECT_EQ(log[0], "Character");
    EXPECT_EQ(log[1], "InputManager");

    g_destructionLog = nullptr;
}

// ---------------------------------------------------------
// 8. Unregistered Service Lookup Test
//    Requesting a type that was never registered is a
//    programming error and must fail loudly, not silently
//    return a default-constructed or null value.
// ---------------------------------------------------------
namespace
{
struct UnregisteredService
{
};
} // namespace

TEST(ServiceContainerTest, GetOnUnregisteredTypeThrows)
{
    ServiceList list;
    ServiceContainer container(list);

    EXPECT_THROW(container.Resolve<UnregisteredService>(), std::runtime_error);
}
