#include <exception>
#include <utility>
#include <type_traits>

template < typename F, typename CleanupPolicy >
class scope_guard : CleanupPolicy, CleanupPolicy::installer
{
    using CleanupPolicy::cleanup;
    using CleanupPolicy::installer::install;
    
    typename std::remove_reference<F>::type f_;
    
public:
    scope_guard(F&& f) : f_(std::forward<F>(f)) { 
        install();
    }
    ~scope_guard() {
        cleanup(f_);
    }
};

//////////////////////////////////////////////
// Installation policies
//////////////////////////////////////////////

struct unchecked_install_policy {
    void install() { }
};

struct checked_install_policy {
    void install() {
        if (std::uncaught_exception()) {
            std::terminate(); // sorry
        }
    }
};

//////////////////////////////////////////////
// Cleanup policies
//////////////////////////////////////////////

struct exit_policy {
    typedef unchecked_install_policy installer;
    
    template < typename F >
    void cleanup(F& f) {
        f();
    }
};

struct failure_policy {
    typedef checked_install_policy installer;
    
    template < typename F >
    void cleanup(F& f) {
        // Only cleanup if we're exiting from an exception.
        if (std::uncaught_exception()) {
            f();
        }
    }
};

struct success_policy {
    typedef checked_install_policy installer;
    
    template < typename F >
    void cleanup(F& f) {
        // Only cleanup if we're NOT exiting from an exception.
        if (!std::uncaught_exception()) {
           f();
        }
    }
};

//////////////////////////////////////////////
// Syntactical sugar
//////////////////////////////////////////////

template < typename CleanupPolicy >
struct scope_guard_builder { };

template < typename F, typename CleanupPolicy >
scope_guard<F,CleanupPolicy> 
operator+(
    scope_guard_builder<CleanupPolicy> builder,
    F&& f
    )
{
    return std::forward<F>(f);
}

// typical preprocessor utility stuff.
#define PASTE_TOKENS2(a,b) a ## b
#define PASTE_TOKENS(a,b) PASTE_TOKENS2(a,b)

// the scope statement
#define scope(condition) \
    auto PASTE_TOKENS(_scopeGuard, __LINE__) = \
        scope_guard_builder<condition##_policy>() + [&]
