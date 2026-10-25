---
name: cpp-clean-code
description: >-
  Use when writing, refactoring, designing, or reviewing C++ code (C++17/20/23), building libraries, applications, or high-performance concurrent architectures. Enforces C++ Core Guidelines, Modern Design Patterns (Type Erasure, std::variant Visitor), thread safety & lock-free design, ABI stability, RAII, and SOLID principles.
---

# Modern C++ Clean Code, Architecture & API Engineering Standards

This document defines the definitive code-generation, architectural design, and code-review standard for AI models operating in modern C++ (C++17, C++20, and C++23). It synthesizes the core principles and idioms from:
1. **Effective Modern C++** (Scott Meyers)
2. **Clean C++: Sustainable Software Development Patterns and Best Practices** (Stephan Roth)
3. **API Design for C++** (Martin Reddy)
4. **C++ Software Design: Design Principles and Patterns for High-Quality Software** (Klaus Iglberger)
5. **C++ Concurrency in Action, 2nd Edition** (Anthony Williams)
6. **C++ Core Guidelines** (Bjarne Stroustrup & Herb Sutter)

When generating, modifying, or reviewing C++ code, the AI **must** enforce these rules systematically. Any deviation must have explicit, documented technical justification.

---

## When to Use

* Writing new modern C++ code (classes, functions, templates, concepts, modules).
* Reviewing, refactoring, or modernizing existing C++ codebases.
* Designing reusable libraries, public SDKs, and dynamic plugins requiring ABI stability.
* Architecting high-performance concurrent, multithreaded, or lock-free data structures.
* Making architectural trade-offs: Value Semantics vs Reference Semantics, Dynamic vs Static Polymorphism.
* Enforcing strict zero-leak, zero-UB, exception-safe code across projects.

## When NOT to Use

* Non-C++ projects (C#, Rust, Python, Go).
* Pure legacy C codebases (C89/C99) restricted from compiling under C++ compilers.
* Hard real-time or bare-metal embedded targets where dynamic memory allocation, RTTI, or C++ exception handling are strictly forbidden by hardware constraints (adapt guidelines selectively).

---

## Conflict Resolution & Rule Hierarchy

When two rules appear to conflict, the AI must resolve them using the following strict priority hierarchy:

```
┌─────────────────────────────────────────────────────────┐
│ 1. Memory, Type & Concurrency Safety                    │
│    (Zero UB, memory safety, race-free, exception-safe)  │
└───────────────────────────┬─────────────────────────────┘
                            ▼
┌─────────────────────────────────────────────────────────┐
│ 2. Public API & ABI Stability                           │
│    (Contract integrity, binary compatibility, Pimpl)    │
└───────────────────────────┬─────────────────────────────┘
                            ▼
┌─────────────────────────────────────────────────────────┐
│ 3. Software Design & Value Semantics                    │
│    (KISS, POLA, Value over Reference Semantics, SOLID)  │
└───────────────────────────┬─────────────────────────────┘
                            ▼
┌─────────────────────────────────────────────────────────┐
│ 4. Concurrency Integrity & Hardware Consciousness       │
│    (Deadlock-freedom, Cache Locality, False Sharing)    │
└───────────────────────────┬─────────────────────────────┘
                            ▼
┌─────────────────────────────────────────────────────────┐
│ 5. Performance & Micro-Optimization                     │
│    (Zero-cost abstractions; profile before optimizing)  │
└─────────────────────────────────────────────────────────┘
```

1. **Safety & Correctness:** Undefined Behavior (UB), data races, dangling views, memory leaks, and broken exception guarantees are non-negotiable defects.
2. **Public API & ABI Stability:** For reusable libraries and public interfaces, binary and source compatibility supersede internal refactoring convenience.
3. **Software Design & Value Semantics:** Clean abstractions, loose coupling, deep constness, and value semantics take precedence over raw pointer manipulation. Interfaces must be *"easy to use correctly, and hard to misuse"* (Scott Meyers).
4. **Hardware Consciousness:** Prevent cache contention, false sharing, and lock contention before tweaking single-threaded instructions.
5. **Performance:** Donald Knuth's axiom applies: *Premature optimization is the root of all evil*. Never sacrifice type safety or clarity for speculative micro-benchmarks. Rely on profile-guided optimization and zero-cost standard abstractions.

---

## 1. Engineering Philosophy & Software Design (`[PHIL]`)

### `[PHIL-01]` Software Design is Not Language Features (Iglberger Guideline 1)
* **The Core Fallacy:** Using modern language features (`auto`, smart pointers, concepts, coroutines) does not automatically make software well-designed.
* **The Distinction:**
  1. **Software Architecture (Strategic):** High-level component topology, dependency direction, physical library boundaries.
  2. **Software Design (Tactical):** Managing dependencies and abstractions between types; decoupling variation points via patterns (GoF, Type Erasure, Visitor).
  3. **Implementation Details (Local):** Class-internal choices (RAII, algorithm selection, `constexpr`, micro-optimizations).
* **Directive:** Do not deploy complex template metaprogramming or obscure patterns where simple free functions or standard compositions suffice.

### `[PHIL-02]` Zero Tolerance for Software Entropy (Core Guidelines P.8)
Never leave technical debt, unhandled compiler warnings, or "temporary hacks" in the codebase.
Apply **The Boy Scout Rule**: *Always leave the code cleaner than you found it.* Every refactoring or feature implementation must eliminate adjacent code smells.

### `[PHIL-03]` Simplicity Triad: KISS, YAGNI, DRY (Core Guidelines P.1, P.2)
- **KISS (Keep It Simple, Stupid):** Avoid homemade complexity.
- **YAGNI (You Aren't Gonna Need It):** Never build speculative extension hooks for hypothetical future requirements. Implement strictly what current specifications demand.
- **DRY (Don't Repeat Yourself) / OAOO (Once And Only Once):** Every piece of domain knowledge must have a single, authoritative representation in the system.

### `[PHIL-04]` Knuth's Optimization Principle (Core Guidelines Per.4, Per.5)
Never implement obscure pointer arithmetic, manual loop unrolling, or inline assembly without:
1. Documented performance requirements (SLA).
2. Quantifiable profiling metrics from profilers (perf, VTune, Tracy).
Compilers generate more optimal vectorized code from clean standard algorithms (`<algorithm>`, ranges) than hand-written assembly-like C-style loops.

### `[PHIL-05]` Principle of Least Astonishment (POLA) (Core Guidelines P.3, F.1)
Every function, class, and method must behave in the way users intuitively expect. An accessor (`get_...()`) must never trigger side effects, mutate observable state, or launch background threads.

---

## 2. Meaningful Naming & Expressiveness (`[NAME]`)

### `[NAME-01]` Intention-Revealing Domain Naming (Core Guidelines NL.1)
Names must state why an entity exists, what it does, and how it is used. Ban generic technical noise suffixes (`Manager`, `Data`, `Info`, `Object`, `Controller`). Adopt Domain-Driven Design (DDD) terminology (`Customer`, `BillingCycle`, `Invoice`).

- ❌ **INCORRECT:**
```cpp
class CustomerInfoManager {
    int d; // elapsed time in days
    std::vector<int*> list1;
};
```

- ✅ **CORRECT:**
```cpp
class CustomerLedger {
    int elapsed_time_in_days{0};
    std::vector<AccountBalance> active_balances;
};
```

### `[NAME-02]` Ban Hungarian Notation and Type Prefixes (Core Guidelines NL.5)
Do not prefix variable names with type indicators (`m_`, `p_`, `str_`, `i_`, `f_`). Modern IDEs, compilers, and strong types make type prefixes obsolete and prone to disinformation during refactorings.

- ❌ **INCORRECT:**
```cpp
class Order {
    std::string m_strCustomerName;
    int* m_pItemCount;
    bool m_bIsDelivered;
};
```

- ✅ **CORRECT:**
```cpp
class Order {
    std::string customer_name;
    int item_count{0};
    bool is_delivered{false};
};
```

### `[NAME-03]` Context Eliminates Redundancy
Do not repeat the enclosing class name in member variables.

- ❌ **INCORRECT:** `struct Movie { std::string movie_title; int movie_release_year; };`
- ✅ **CORRECT:** `struct Movie { std::string title; int release_year{0}; };`

### `[NAME-04]` Boolean Predicates
Methods returning `bool` must read as predicate assertions prefixed with `is_`, `has_`, `can_`, or `should_`.

```cpp
[[nodiscard]] bool is_connected() const noexcept;
[[nodiscard]] bool has_pending_transactions() const noexcept;
```

---

## 3. Value Semantics vs. Reference Semantics (`[VAL]`)

### `[VAL-01]` Prefer Value Semantics over Pointer Semantics (Iglberger Guideline 22, Core Guidelines C.10)
Reference semantics (raw pointers, `std::unique_ptr` polymorphic hierarchies) scatter objects across the heap, cause CPU cache misses, provide shallow constness, and obscure ownership.
**Modern C++ favors Value Semantics:**
* **Deep Constness:** Marking a value object `const` protects its entire state.
* **Deterministic Lifetime:** No `nullptr`, no shared ownership confusion.
* **Data Locality:** Storing values in contiguous `std::vector` dramatically accelerates CPU cache prefetching.

### `[VAL-02]` Non-Owning Reference Views Lifetime Trap (`std::string_view`, `std::span`)
* **Use as parameters:** `std::string_view` and `std::span` are outstanding for passing non-owning slices into functions.
* **DO NOT store in long-lived objects:** Never store `std::string_view` or `std::span` as class data members unless the class itself is a short-lived view or lifetimes are strictly guaranteed. Reallocations in source containers immediately invalidate views into dangling references!

- ❌ **INCORRECT (Dangling View in State):**
```cpp
class UserProfile {
    std::string_view username; // DANGER: Dangling pointer if source string moves or dies!
};
```

- ✅ **CORRECT:**
```cpp
class UserProfile {
    std::string username; // Owns its memory safely
};
```

### `[VAL-03]` The Iteration Aliasing Pitfall
Never pass elements of a container by reference to algorithms that mutate or reorder that same container.

- ❌ **INCORRECT:**
```cpp
auto const pos = std::max_element(begin(v), end(v));
// BUG: *pos passed by const&, but std::remove overwrites elements during execution!
v.erase(std::remove(begin(v), end(v), *pos), end(v));
```

- ✅ **CORRECT:**
```cpp
auto const max_val = *std::max_element(begin(v), end(v)); // Copy the value first
v.erase(std::remove(begin(v), end(v), max_val), end(v));
```

---

## 4. Modern Type System, Deduction & Concepts (`[TYPE]`)

### `[TYPE-01]` Safe `auto` & Invisible Proxy Types (Scott Meyers Items 5 & 6)
Use `auto` by default to eliminate type redundancy, prevent silent 64-bit truncation (`unsigned` vs `size_t`), and prevent accidental temporary copies in container iterations.
**CRITICAL EXCEPTION:** Beware invisible proxy types like `std::vector<bool>::reference` or expression template proxies (Eigen, Blaze). In these cases, use the **Explicitly Typed Initializer Idiom**.

- ❌ **INCORRECT:**
```cpp
std::vector<bool> flags = get_flags();
auto is_active = flags[5]; // DANGER: is_active is std::vector<bool>::reference!
// If flags dies or reallocates, is_active becomes a dangling pointer!
```

- ✅ **CORRECT:**
```cpp
// Explicitly Typed Initializer Idiom
auto is_active = static_cast<bool>(flags[5]); // Guaranteed safe bool value

for (const auto& [id, account] : account_map) { // C++17 Structured Binding
    // Zero allocations, direct reference binding
}
```

### `[TYPE-02]` Uniform Initialization `{}` vs `()` (Scott Meyers Item 7)
- Use braced initialization `{}` as the default initialization style:
  1. Prevents narrowing conversions (`int x{2.5};` triggers a compilation error).
  2. Immunizes against the *Most Vexing Parse* (`Widget w();` is a function declaration; `Widget w{};` constructs an object).
- **CRITICAL TRAP:** If a class provides a constructor accepting `std::initializer_list`, the compiler aggressively prioritizes it with `{}` over other matching constructors!
  * `std::vector<int> v1(10, 20);` -> 10 elements with value 20.
  * `std::vector<int> v2{10, 20};` -> 2 elements: 10 and 20.

### `[TYPE-03]` `decltype` and `decltype(auto)` (Scott Meyers Item 3)
`decltype(e)` yields the exact declared type. However, for lvalue expressions with parentheses `((x))`, it yields `T&`.
When forwarding return types without losing reference and `const` qualifiers (C++14), use `decltype(auto)`:

```cpp
template <typename Container, typename Index>
decltype(auto) auth_and_access(Container& c, Index i) {
    authenticate_user();
    return c[i]; // Preserves lvalue reference (T&) or rvalue without copying!
}
```

### `[TYPE-04]` Type Aliasing: Prefer `using` over `typedef` (Scott Meyers Item 9)
`using` supports template aliasing directly, whereas `typedef` requires verbose struct wrappers. In template metaprogramming, prefer modern `_t` (type) and `_v` (value) aliases (e.g., `std::decay_t<T>`, `std::is_same_v<T, U>`).

### `[TYPE-05]` Scoped Enums (`enum class`) over Raw Enums (Core Guidelines Enum.3)
Never use unscoped `enum`. Scoped enums prevent namespace pollution, prohibit implicit conversions to integer types, and can be forward-declared with an explicit underlying storage type (`enum class Color : uint8_t`).

### `[TYPE-06]` Mandatory `explicit` for Single-Argument Constructors
Any constructor callable with a single argument must be marked `explicit` to prevent accidental implicit type conversions. Also mark single-argument user-defined conversion operators as `explicit`.

### `[TYPE-07]` Strong Typing & Barton-Nackman Mixins
*Axiom: Don't trust names. Trust types. Types don't lie.*
Never practice **Primitive Obsession** by using raw scalars (`double`, `int`) for domain dimensions. Wrap domain concepts in zero-overhead strong types with User-Defined Literals (UDLs).

*Reference Implementation:* [strong_types.hpp](./examples/strong_types/strong_types.hpp)

### `[TYPE-08]` C++20 Concepts over SFINAE (Core Guidelines T.10-T.13)
Completely replace unreadable `std::enable_if_t` with C++20 **Concepts & Constraints**.
* Express requirements directly via `concept` and `requires`.
* Errors pinpoint unsatisfied expressions rather than vomiting pages of template trace.
* Compilers automatically perform concept-based overload resolution via subsumption.

*Reference Implementation:* [modern_concepts.hpp](./examples/concepts_and_constraints/modern_concepts.hpp)

- ❌ **INCORRECT (Obsolete C++14 SFINAE):**
```cpp
template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
void process(T val);
```

- ✅ **CORRECT (C++20 Concept):**
```cpp
template <std::integral T>
void process(T val);

// Or terse syntax:
void process(std::integral auto val);
```

### `[TYPE-09]` Always Initialize Variables (Core Guidelines ES.20)
Never leave primitive scalar variables uninitialized (`int count;` leads to undefined behavior on read). Always initialize with default values:
```cpp
int counter{0};
double balance{0.0};
auto flags = StatusFlags::None;
```

### `[TYPE-10]` Do Not Specialize Function Templates — Overload Instead (Core Guidelines T.144)
Function template specializations do not participate in overload resolution in the way developers expect (Herb Sutter's rule). The compiler first selects the base template overload, and only then checks if a specialization matches.
* **Rule:** If you need type-specific behavior for functions, use **function overloading** or delegate to a class template trait.

- ❌ **INCORRECT (Specializing a function template):**
```cpp
template <typename T> void process(T val);
template <> void process<int*>(int* ptr); // Specialization hazard!
```

- ✅ **CORRECT (Overloading):**
```cpp
template <typename T> void process(T val);
void process(int* ptr); // True overload participates cleanly in overload resolution
```

---

## 5. Memory Management & Universal RAII (`[RES]`)

### `[RES-01]` Universal RAII (Core Guidelines R.1, R.11, E.6)
Every system resource (heap memory, POSIX file descriptor, Win32 `HANDLE`, database connection, mutex lock, socket) **must** be acquired in an object's constructor and released in its destructor. Manual resource cleanup calls (`free`, `delete`, `CloseHandle`, `fclose`) in business logic are strictly forbidden.

*Reference Implementation:* [raii_handle.hpp](./examples/raii_resource/raii_handle.hpp)

### `[RES-02]` The Rule of Zero as Primary Architecture Goal
Design domain classes so that they require **no custom destructor, copy/move constructors, or copy/move assignment operators**. Rely entirely on STL containers and smart pointers to handle resource lifecycles automatically.

### `[RES-03]` The Rule of Five / Three
If a class directly manages a raw resource handle, you **must explicitly declare or delete all five** special member functions: Destructor, Copy Constructor, Copy Assignment, Move Constructor, and Move Assignment.

### `[RES-04]` Elimination of `new` and `delete`
Never invoke `new` or `delete` directly. Use `std::make_unique` (C++14) and `std::make_shared` (C++11).
- **Exceptions where `make_*` cannot be used:**
  1. Specifying a custom deleter (`std::unique_ptr<Widget, Deleter>(new Widget(), del)`).
  2. Initializing with braced initializer lists (`std::initializer_list`).
  3. `make_shared` with huge objects referenced long-term by `std::weak_ptr` (pins memory block until weak references hit zero).

### `[RES-05]` Smart Pointer Ownership Semantics
- **`std::unique_ptr<T>` (Default Choice):** Exclusive, unambiguous ownership. Zero runtime overhead (identical to a raw pointer).
- **`std::shared_ptr<T>`:** Genuine shared ownership where multiple independent actors co-own the resource's lifetime.
- **`std::weak_ptr<T>`:** Non-owning observer of a `shared_ptr`. Used for caching, observer lists, and **breaking circular references**.

### `[RES-06]` Canonical Copy-and-Swap Idiom (Strong Exception Guarantee)
When writing a custom resource managing class requiring copy-assignment, use Copy-and-Swap. Pass the parameter by value and swap state using a non-throwing `swap`.

*Reference Implementation:* [copy_and_swap_buffer.hpp](./examples/copy_and_swap/copy_and_swap_buffer.hpp)

---

## 6. Move Semantics, Universal References & Sinks (`[MOVE]`)

### `[MOVE-01]` Nature of `std::move` and `std::forward` (Scott Meyers Item 23)
- `std::move` and `std::forward` generate **zero machine instructions** at runtime. They are compile-time type casts.
- `std::move(x)` unconditionally casts `x` to an rvalue reference (`std::remove_reference_t<T>&&`).
- `std::forward<T>(x)` conditionally casts `x` to an rvalue reference *only if* its type argument `T` resolved to an rvalue.

### `[MOVE-02]` Universal (Forwarding) References vs Rvalue References (Item 24)
`T&&` is a Universal (Forwarding) Reference **only when type deduction is taking place** in the exact syntax `T&&`:
```cpp
template <typename T> void f(T&& param); // Universal Reference (deduction happens)
auto&& var = value;                     // Universal Reference (deduction happens)
void f(Widget&& param);                 // Rvalue Reference (NO deduction)
```

### `[MOVE-03]` The Return Value Optimization (RVO / NRVO) Anti-Pattern
**NEVER** write `return std::move(local_var);` when returning a local object by value!
Doing so disables compiler copy elision (RVO/NRVO) and forces a move operation instead of constructing the object directly in the caller's storage.

- ❌ **INCORRECT:** `return std::move(result);`
- ✅ **CORRECT:** `return result;`

### `[MOVE-04]` The `const` Move Trap
Moving a `const` object silently invokes its **copy constructor** because `T(T&&)` accepts a non-const rvalue reference! Never mark objects intended for moving as `const`.

### `[MOVE-05]` Member Function Reference Qualifiers (Item 12)
Overload accessors on reference qualifiers (`&` and `&&`) to optimize data extraction from rvalues.

```cpp
class Document {
public:
    using Content = std::vector<std::string>;
    [[nodiscard]] const Content& content() const & noexcept { return m_content; }
    [[nodiscard]] Content content() && noexcept { return std::move(m_content); }
private:
    Content m_content;
};
```

### `[MOVE-06]` Pass-by-Value for Inexpensive Movable Sink Parameters (Item 41)
When a function always copies an argument into internal storage (a "sink" parameter) and the type is cheap to move (e.g., `std::string`, `std::vector`), take the argument **by value** and move it into place:

```cpp
class Employee {
public:
    explicit Employee(std::string name) : m_name(std::move(name)) {}
private:
    std::string m_name;
};
```

---

## 7. Functions, Ranges & Monadic Error Handling (`[FUNC]`)

### `[FUNC-01]` Single Responsibility & Function Dimensions (Core Guidelines F.2, F.3)
A function must do **one thing**, do it completely, and do it well.
- Functions should ideally be 5–15 lines. Any function exceeding 25 lines must be scrutinized for decomposition.
- Arity: Monadic (1) or niladic (0) preferred; dyadic (2) acceptable; polyadic (>3) forbidden (group parameters into an aggregate struct).

### `[FUNC-02]` Ban Boolean Flag Arguments
Passing raw `bool` flags to functions makes call sites illegible and violates SRP. Replace flags with dedicated functions or explicit scoped enums (`render_view(ScreenMode::Fullscreen, ShadowState::Disabled);`).

### `[FUNC-03]` Elimination of Raw Out-Parameters
Never use non-const reference parameters (`Type& out_val`) to pass values out of a function. Return results by value (leveraging move semantics / RVO), or return structured types or `std::optional`.

### `[FUNC-04]` Modern Monadic Error Handling (`std::expected` / `Result<T, E>`)
Exceptions are reserved for truly unexpected system failures (out of memory, corrupted hardware). For normal domain/business failures (user not found, validation error):
* Do not return `std::optional` (loses the reason for failure).
* Do not throw exceptions (kills performance, disrupts control flow).
* Use **`Result<T, E>` / C++23 `std::expected<T, E>`** with monadic pipelines (`and_then`, `transform`, `or_else`).

*Reference Implementation:* [result.hpp](./examples/monadic_result/result.hpp)

```cpp
auto outcome = fetch_user_by_id(42)
    .and_then(verify_adult)
    .transform([](const std::string& name) { return "Welcome, " + name; });
```

### `[FUNC-05]` Canonical Parameter Passing Matrix

| Data Type | Purpose | Signature Pattern |
| :--- | :--- | :--- |
| **Primitives & cheap types** (`int`, `float`, `enum`, `string_view`, `span`) | In / Read-only | `T` (Pass by value) |
| **Heavy types** (`std::string`, `std::vector`, large structs) | In / Read-only | `const T&` |
| **Movable sink arguments** (object stored in member/collection) | In / Consume | `T` (By value + `std::move`) |
| **Forwarding templates** | In / Forward | `T&&` (Universal ref + `std::forward<T>`) |
| **Non-owning read-only string** | In / Slice | `std::string_view` |
| **Non-owning contiguous sequence** | In / Slice | `std::span<const T>` |
| **Mutation target** | In-Out / Mutate | `T&` (Clear naming: `update_...`) |

### `[FUNC-06]` Lambda Capture Hygiene (Scott Meyers Items 31 & 32)
- **NEVER use default reference capture `[&]`:** Massive hazard for dangling references when the lambda outlives local scope.
- **NEVER use default value capture `[=]` in member functions:** Captures the hidden `this` pointer by value, not the members!
- **Use Init Capture (C++14):** Capture by moving values into the closure: `[data = std::move(data)]() { ... }`.

### `[FUNC-07]` Pervasive Const-Correctness
- Mark non-mutating member functions `const`.
- Mark non-throwing functions `noexcept`.
- Mark pure observer functions `[[nodiscard]]`.

### `[FUNC-08]` Ban Raw Loops: Use `<algorithm>` and C++20 Ranges Pipelines
Before writing a raw `for` or `while` loop, use standard algorithms or C++20 Ranges pipelines:

```cpp
// C++20 Ranges Pipeline
auto even_squares = numbers 
                  | std::views::filter([](int n) { return n % 2 == 0; })
                  | std::views::transform([](int n) { return n * n; });
```

### `[FUNC-09]` Complete Banishment of C-Style Artifacts
1. **No C-arrays (`int arr[10];`):** Use `std::array<int, 10>` or `std::vector<int>`.
2. **No raw character buffers (`char buf[256];`):** Use `std::string` or `std::string_view`.
3. **No C-style casts (`(int)x`):** Use `static_cast<int>(x)`, `const_cast`, or `std::bit_cast`.
4. **No macro constants or function macros:** Use `inline constexpr` or `constexpr` functions.

### `[FUNC-10]` Exception Hygiene: Hierarchy & Catching by Const Reference (Core Guidelines E.14, E.15)
When exceptions are necessary for unrecoverable errors:
1. Always inherit custom exception classes from `std::runtime_error` or `std::exception`.
2. **Throw by value:** `throw NetworkError("Connection refused", 503);`
3. **Catch by `const &`:** `catch (const NetworkError& e)` (catching by value causes object slicing and strips derived diagnostic data!).
4. Never throw primitive types (`int`, `const char*`). Never leave catch blocks empty.

---

## 8. Modern Software Design Patterns (`[DES]`)

### `[DES-01]` The Modern Visitor Pattern via `std::variant` (Iglberger Guidelines 15–18)
* **The Expression Problem:** Closed Set of Types, Open Set of Operations.
* Classic GoF Visitor is cyclical, requires intrusive `accept()` methods on shape classes, and uses double-dispatch vtables.
* **Modern C++ Solution:** Store types in `std::variant<Circle, Rectangle, Triangle>`. Add operations using `std::visit` and the `Overloaded` helper struct.
* **Advantages:** Zero vtables, zero inheritance, deep value semantics, and elements stored contiguously in memory.

*Reference Implementation:* [variant_visitor.hpp](./examples/variant_visitor/variant_visitor.hpp)

```cpp
template <typename... Ts> struct Overloaded : Ts... { using Ts::operator()...; };
template <typename... Ts> Overloaded(Ts...) -> Overloaded<Ts...>;

double total_area = std::visit(Overloaded{
    [](const Circle& c) { return 3.14159 * c.radius * c.radius; },
    [](const Rectangle& r) { return r.width * r.height; }
}, shape);
```

### `[DES-02]` The Modern Type Erasure Pattern (Iglberger Guidelines 32–34)
* **The Expression Problem:** Open Set of Types, Closed Set of Operations with Value Semantics.
* **Architecture:** Compound Pattern = **External Polymorphism + Bridge + Prototype**.
* **Result:** A class (`AnyShape`) that acts as a true Value Type. It holds any type satisfying an interface without requiring client classes to inherit from anything (like `std::function` or `std::any`).

*Reference Implementation:* [any_shape.hpp](./examples/type_erasure/any_shape.hpp)

### `[DES-03]` Value-Based Strategy via `std::function` & Lambdas (Guideline 23)
Instead of creating rigid abstract strategy base classes and passing raw pointers, pass callable objects:
```cpp
class PaymentProcessor {
public:
    using TaxStrategy = std::function<double(double)>;
    explicit PaymentProcessor(TaxStrategy tax_calculator) : m_tax(std::move(tax_calculator)) {}
private:
    TaxStrategy m_tax;
};
```

### `[DES-04]` CRTP vs C++20 Concepts vs C++23 Deducing `this`
1. **CRTP as Static Interface:** Largely superseded by C++20 Concepts.
2. **CRTP as Implementation Mixin (Barton-Nackman):** Excellent for injecting operators (`operator==`, `operator+`) via hidden friends.
3. **C++23 Deducing `this`:** Simplifies CRTP by passing `this self` as an explicit object parameter, eliminating casting boilerplates.

### `[DES-05]` Design Pattern Decision Matrix

```
[Need to extend or structure system]
   │
   ├──► Open Types, Closed Operations (New types added frequently)
   │     ├── Dynamic Polymorphism + Value Semantics: TYPE ERASURE
   │     ├── Dynamic Polymorphism + Reference Semantics: Abstract Base Class
   │     └── Compile-Time Zero-Overhead: C++20 CONCEPTS
   │
   ├──► Closed Types, Open Operations (New operations added frequently)
   │     └── std::variant + std::visit (MODERN VISITOR)
   │
   ├──► Dynamic Custom Algorithm Injection
   │     └── Value-Based STRATEGY (std::function / Lambdas)
   │
   └──► Compilation Firewall / ABI Protection
         ├── General API: PIMPL IDIOM
         └── High-frequency loops: PARTIAL BRIDGE (hot data in class, cold data in Impl)
```

---

## 9. API Design & ABI Stability (`[API]`)

### `[API-01]` The Scott Meyers Rule for Public Interfaces
Design public APIs so that they are **easy to use correctly and difficult to misuse**. Reject invalid combinations at compile time through strong types.

### `[API-02]` Physical & Logical Information Hiding
- **Zero OS / internal headers in public `.h` files:** Never include `<windows.h>`, `<sys/socket.h>`, or internal vendor headers in public headers.
- **Zero private member variables in public classes:** Use the **Pimpl Idiom** to create a compilation firewall.

### `[API-03]` Modern Pimpl Idiom with `std::unique_ptr` (Scott Meyers Item 22)
Pimpl eliminates compile-time header dependencies and preserves ABI compatibility.

*Reference Implementation:* [autotimer.hpp](./examples/pimpl_idiom/autotimer.hpp) and [autotimer.cpp](./examples/pimpl_idiom/autotimer.cpp)

**Pimpl Checklist:**
1. In public `.hpp`: Forward-declare `class Impl;`.
2. Store `std::unique_ptr<Impl> m_impl;`.
3. **DECLARE** destructor and move operations in `.hpp`. Do NOT `= default` them in `.hpp`!
4. In private `.cpp`: Fully define `class Impl`.
5. In private `.cpp`: **DEFINE** destructor, move constructor, and move assignment using `= default` *after* `Impl` is a complete type.

### `[API-04]` Partial Bridge for Cache-Sensitive Pimpl
When profiling reveals that pointer indirection to `Impl` degrades CPU cache hit rates in tight loops, apply the **Partial Bridge**: keep hot primitive variables inside the main class layout and move only cold data/complex dependencies into `m_impl`.

### `[API-05]` Free Non-Friend Functions Over Member Functions (Meyers' Rule)
If a method can be implemented strictly via the public interface of a class, implement it as a **non-member, non-friend free function** within the same namespace to maximize encapsulation.

### `[API-06]` Strict Ban on `using namespace` in Headers
Never write `using namespace std;` in any header file. This pollutes consumer namespaces and triggers silent compilation ambiguities.

### `[API-07]` ABI Stability Rules (Binary Backward Compatibility)
When developing shared libraries (`.dll` / `.so`):

#### ❌ ABI BREAKING Changes:
1. Adding, removing, or reordering member variables of a class.
2. Changing the type of any member variable.
3. Adding, removing, or changing the order of virtual functions in `vtable`.
4. Adding the *first* virtual function to a non-polymorphic class.
5. Changing function signatures or default arguments.

#### ✅ ABI SAFE Changes:
1. Adding new standalone classes, free functions, or non-virtual methods.
2. Adding static variables.
3. Adding enumerators to the *end* of an existing enum.
4. **Any modification to the internal `Impl` class when using Pimpl!**

### `[API-08]` C-ABI Boundary for Dynamic Plugins
When creating dynamic plugin architectures loaded via `dlopen`/`LoadLibrary`:
1. Use `extern "C"` functions for exported factory entry points.
2. Use abstract pure virtual interfaces (`struct IPlugin`).
3. **Symmetric Memory Management:** Ensure memory allocated by the plugin is freed by the plugin.
4. Never pass STL containers (`std::string`, `std::vector`) or throw C++ exceptions across DLL boundaries.

*Reference Implementation:* [plugin_api.h](./examples/c_abi_plugin/plugin_api.h)

---

## 10. Object-Oriented Design & SOLID (`[OOP]`)

### `[OOP-01]` Single Responsibility & Private Method Extraction
Classes should have only one reason to change.
* **Testability Guideline:** If you feel the urge to test a `private` method, it is a glaring smell that the class violates SRP. Do NOT make tests `friend` or expand access to `protected`. **Extract that functionality into a free function or separate helper class.**

### `[OOP-02]` LSP & The Square-Rectangle Fallacy (Core Guidelines C.35)
Inheritance models an **"IS-SUBSTITUTABLE-FOR"** relationship, not just an "IS-A" relationship.
* Prefer composition over inheritance.
* **Base Class Destructor Rule:** Polymorphic base classes **MUST** declare either a `public virtual` destructor or a `protected non-virtual` destructor.
* Never inherit from standard STL containers (`std::vector`, `std::string`) because they lack virtual destructors.

### `[OOP-03]` Interface Segregation Principle (ISP) for Classes and Concepts
Split monolithic interfaces into focused, cohesive contracts. Do not force clients to implement or depend on methods they do not use.

### `[OOP-04]` Dependency Inversion & Ownership of Abstraction (Guideline 9)
High-level modules must not depend on low-level modules; both depend on abstractions.
**CRITICAL:** The abstraction belongs to the **high-level module**. The editor owns the `PluginInterface`, not the plugin!

### `[OOP-05]` Dependency Injection (Constructor Injection) over Singleton
Singletons introduce hidden dependencies, mutable global state, and destroy parallel test isolation.
* Instantiate single instances in an application composition root.
* Inject dependencies via interfaces or callable objects in constructors.

---

## 11. Concurrency & Multithreading (`[CONC]`)

### `[CONC-01]` Thread Lifecycle & RAII Management (Anthony Williams Chapter 2)
If a `std::thread` destructor runs while the thread is still `.joinable()`, the C++ runtime immediately invokes `std::terminate()`, aborting the process!
- In C++17: Always wrap raw `std::thread` instances in an RAII container (`scoped_thread` or `joining_thread`).
- In C++20: Use **`std::jthread`** (joins automatically on destruction and supports cooperative cancellation tokens).

*Reference Implementation:* [scoped_thread.hpp](./examples/scoped_thread/scoped_thread.hpp)

### `[CONC-02]` Elimination of Interface Race Conditions (Williams Section 3.1)
Separate `empty()`, `top()`, and `pop()` methods create unavoidable race conditions in multithreaded environments between check and retrieval.
* Combine query and extraction into atomic operations returning `std::shared_ptr<T>` or taking a reference destination: `bool try_pop(T& value)`.

### `[CONC-03]` Deadlock Prevention via `std::scoped_lock` (Williams Section 3.2)
Never acquire multiple mutexes sequentially using individual `std::lock_guard`. Use C++17 **`std::scoped_lock`**, which employs a deadlock-avoidance algorithm to lock all mutexes atomically.

```cpp
// Locks both mutexes simultaneously without deadlock hazard:
std::scoped_lock guard(lhs.m_mutex, rhs.m_mutex);
```

### `[CONC-04]` Spurious Wakeup Guard for Condition Variables
Always wait on `std::condition_variable` using the predicate loop overload to protect against spurious wakeups:

```cpp
std::unique_lock<std::mutex> lock(m_mutex);
m_cv.wait(lock, [this] { return !m_queue.empty(); });
```

### `[CONC-05]` `const` Member Functions Must Be Thread-Safe (Scott Meyers Item 16)
Clients treat `const` member functions as concurrent read operations safe to execute simultaneously from multiple threads.
If a `const` member function mutates internal cached state via `mutable`:
- Synchronize scalars using `std::atomic<T>`.
- Synchronize complex states using `std::mutex` or `std::shared_mutex`.

### `[CONC-06]` C++ Memory Model & Atomics (Williams Chapter 5)
* **Data Race = Undefined Behavior (UB):** Two threads accessing the same memory location concurrently without synchronization where at least one is a write.
* **`memory_order_seq_cst` (Default):** Establishes a single globally consistent order of execution. Safest, but carries micro-overheads on weak memory architectures (ARM).
* **`memory_order_acquire` & `memory_order_release`:** Synchronizes write operations in thread A with read operations in thread B without enforcing a global timeline.
* **`memory_order_relaxed`:** Guarantees only atomicity of the single variable; zero ordering synchronization across threads.
* **`volatile` is NOT for concurrency:** `volatile` does not provide thread safety or memory fences. Use `std::atomic<T>`.

### `[CONC-07]` Fine-Grained Locking & The Dummy Node Technique
To prevent producers and consumers from contending for the same lock in concurrent queues, separate head and tail modifications using a **Dummy Node** and two distinct mutexes (`head_mutex` and `tail_mutex`).

*Reference Implementation:* [fine_grained_queue.hpp](./examples/concurrent_queue/fine_grained_queue.hpp)

### `[CONC-08]` Hardware-Conscious Concurrency & False Sharing (Williams Chapter 7)
When multiple threads on different CPU cores modify independent variables that reside on the same 64-byte CPU cache line, the cores constantly invalidate each other's cache lines (**False Sharing / Cache Ping-Pong**), degrading performance by 10x–100x.
* **Fix:** Align concurrent variables to cache line boundaries using `alignas(std::hardware_destructive_interference_size)`.

```cpp
struct alignas(std::hardware_destructive_interference_size) ThreadCounter {
    std::atomic<uint64_t> count{0};
};
```

### `[CONC-09]` Work-Stealing Queues and Thread Pools (Williams Chapter 8)
For scalable task parallelism without thread-creation overhead, implement thread pools where each thread maintains a local LIFO deque for cache locality, and idle threads steal FIFO tasks from the back of other threads' queues.

### `[CONC-10]` C++17 Parallel Execution Policies
When using STL algorithms with execution policies:
* `std::execution::par`: Code within lambdas must be thread-safe (use locks/atomics for shared state).
* `std::execution::par_unseq`: Code is vectorized (SIMD). **Strictly forbid all mutexes, atomics, or memory allocations** inside the loop, or deadlocks/UB will occur.

### `[CONC-11]` The Unnamed Lock Trap & Alien Code in Critical Sections (Core Guidelines CP.44, CP.22)
1. **The Unnamed Lock Guard Trap (CP.44):**
   Never create unnamed temporary lock objects. Writing `std::lock_guard<std::mutex>(m_mutex);` creates an anonymous temporary that is destroyed at the end of that statement, leaving the subsequent critical section completely unlocked! Always provide a named variable:
   ```cpp
   std::scoped_lock lock(m_mutex); // Named! Protects until the enclosing block ends
   ```
2. **Alien Code in Critical Sections (CP.22):**
   Never call user-supplied callbacks, virtual functions, or unknown external code while holding a mutex lock. The external code may attempt to re-acquire the lock (deadlock) or cause reentrancy issues. Copy data locally and invoke callbacks *outside* the locked scope.

---

## 12. Testing & Test-Driven Development (`[TEST]`)

### `[TEST-01]` The Test Pyramid
- **Base (70-80%):** Fast, fully isolated Unit Tests executing in seconds.
- **Middle (15-20%):** Component and integration tests validating subsystem interaction.
- **Top (5-10%):** System, end-to-end, or UI tests.

### `[TEST-02]` Isolation & Dependency Injection in Tests
Unit tests must **never** make real network requests, write to production databases, or rely on system clock ticks. Abstract all non-deterministic external dependencies behind pure virtual interfaces or callable strategies and inject Test Doubles (Mocks, Fakes, Stubs).

### `[TEST-03]` Single Logical Assertion & AAA Pattern
Every unit test must verify exactly one behavioral scenario following **Arrange-Act-Assert (AAA)**.

### `[TEST-04]` The Red-Green-Refactor Cycle
1. **RED:** Write a small failing unit test.
2. **GREEN:** Write minimal production code to pass the test.
3. **REFACTOR:** Clean up production and test code while keeping all tests green.

---

## 13. Production Pattern Reference Implementations

All patterns are implemented as verified, standalone, production-ready code inside the `./examples/` directory:

1. **[Type Erasure](./examples/type_erasure/any_shape.hpp)**: Klaus Iglberger's canonical Value-Semantic Type Erasure (External Polymorphism + Bridge + Prototype).
2. **[Modern Visitor](./examples/variant_visitor/variant_visitor.hpp)**: Expression Problem solution for closed types using `std::variant` and `Overloaded`.
3. **[Fine-Grained Queue](./examples/concurrent_queue/fine_grained_queue.hpp)**: Two-mutex thread-safe queue with dummy node and condition variable.
4. **[Monadic Result](./examples/monadic_result/result.hpp)**: Modern C++23 `std::expected`-style monadic error handling pipeline.
5. **[Concepts & Constraints](./examples/concepts_and_constraints/modern_concepts.hpp)**: C++20 concepts replacing SFINAE with concept-based overloading.
6. **[Pimpl Idiom](./examples/pimpl_idiom/autotimer.hpp)**: Compilation firewall and ABI preservation with `std::unique_ptr`.
7. **[Extensible Factory](./examples/extensible_factory/renderer_factory.hpp)**: Thread-safe callback registration implementing the Open/Closed Principle.
8. **[C-ABI Dynamic Plugin](./examples/c_abi_plugin/plugin_api.h)**: ABI-safe interface for shared libraries (`.dll`/`.so`) with symmetric memory deallocation.
9. **[Generic RAII Handle](./examples/raii_resource/raii_handle.hpp)**: Zero-cost policy-based RAII wrapper for foreign OS handles.
10. **[Strong Types & Units](./examples/strong_types/strong_types.hpp)**: Zero-overhead compile-time unit dimensional analysis.
11. **[Scoped RAII Thread](./examples/scoped_thread/scoped_thread.hpp)**: Exception-safe wrapper guaranteeing non-joinable destruction.
12. **[Canonical Copy-and-Swap](./examples/copy_and_swap/copy_and_swap_buffer.hpp)**: Strong exception safety guarantee with non-throwing `swap`.
13. **[SFINAE-Constrained Constructor](./examples/forwarding_constructor/perfect_forwarding_sink.hpp)**: Universal reference constructor avoiding greedy overload collisions.

---

## 14. Systematic Code Review & Generation Checklist (`[REV]`)

When generating, refactoring, or auditing C++ code, execute this 12-point verification pass sequentially:

```
┌────┬─────────────────────────────┬────────────────────────────────────────────────────────┐
│ #  │ Verification Check          │ Criteria / Rule                                        │
├────┼─────────────────────────────┼────────────────────────────────────────────────────────┤
│ 1  │ Memory & Resource Safety    │ Are all resources bound to RAII? No raw new/delete?    │
│ 2  │ Special Member Functions    │ Does the class follow Rule of Zero or Rule of Five?    │
│ 3  │ Value vs Reference Lifetime │ Are views (span/string_view) strictly short-lived?     │
│ 4  │ Move Semantics Correctness  │ No std::move on local returns? Sinks taken by value?   │
│ 5  │ Const & Thread Safety       │ Are non-mutating methods const? Mutable data locked?   │
│ 6  │ Concurrency Integrity       │ Are threads joinable on exit? Are locks scoped?        │
│ 7  │ Type System & Concepts      │ Are concepts used instead of unreadable SFINAE?        │
│ 8  │ Error Handling Model        │ Are domain errors handled via Result/expected?         │
│ 9  │ API & ABI Stability         │ No private fields in public headers? Is Pimpl needed?  │
│ 10 │ Header Hygiene              │ No 'using namespace' in headers? Minimal includes?     │
│ 11 │ Design Patterns             │ Is Type Erasure or std::variant preferred over deep OOP?│
│ 12 │ Testability & Injection     │ Are dependencies injected via constructors? No Globals?│
└────┴─────────────────────────────┴────────────────────────────────────────────────────────┘
```
