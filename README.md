# Modern C++ Clean Code, Architecture & API Engineering Standards

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![Standard](https://img.shields.io/badge/C%2B%2B-17%2F20%2F23-blue.svg)](https://en.cppreference.com/)
[![Ecosystem](https://img.shields.io/badge/Agent%20Skills-Ready-green.svg)](https://skills.sh)

An industrial-grade, production-tested procedural knowledge skill for AI coding agents (**Google Antigravity**, **Cursor**, **Claude Code**, **Windsurf**, **GitHub Copilot**, and **Goose**). 

This skill transforms AI models from generic C-style code generators into expert Modern C++ software architects capable of designing robust libraries, mission-critical systems, and high-throughput concurrent architectures.

---

## 🎯 Foundations & The Six Pillars

This skill synthesizes and unifies the engineering principles from six world-class C++ authorities into a cohesive, non-contradictory ruleset:

1. **Effective Modern C++** — *Scott Meyers*
   * Type deduction engines (`auto`, `decltype`, `decltype(auto)`), Universal References (`T&&`), perfect forwarding nuances, and smart pointer lifecycle management.
2. **C++ Software Design (2022)** — *Klaus Iglberger*
   * Modern GoF patterns, Type Erasure, `std::variant` Visitor, Value Semantics vs. Reference Semantics, and solving the Expression Problem without vtable overhead.
3. **C++ Concurrency in Action (2nd Edition)** — *Anthony Williams*
   * C++ Memory Model, atomic ordering (`acquire-release`, `relaxed`, `seq_cst`), deadlock-avoidance algorithms, condition variable spurious wakeup protection, fine-grained multi-mutex locking, and hardware-conscious false-sharing prevention (`alignas`).
4. **API Design for C++** — *Martin Reddy*
   * Physical and logical information hiding, the Pimpl idiom, compilation firewalls, ABI preservation (what breaks ABI vs. what preserves ABI), and safe C-ABI dynamic plugin boundaries.
5. **Clean C++: Sustainable Software Development Patterns** — *Stephan Roth*
   * Software craftsmanship, universal RAII, Rule of Zero/Five, Copy-and-Swap idiom, Type-Rich domain modeling, and pragmatic SOLID principles in C++.
6. **C++ Core Guidelines** — *Bjarne Stroustrup & Herb Sutter*
   * Cross-cutting rules for resource safety, type safety, immutability by default, and concept-constrained templates (`P.*`, `R.*`, `F.*`, `C.*`, `T.*`, `CP.*`).

---

## 🚀 What You Can Build With This Skill

With this skill enabled, AI agents systematically generate production-grade architectures across complex domains:

* 📦 **Public Libraries & Commercial SDKs:**
  * Zero platform-specific headers in public APIs (`<windows.h>`, `<sys/socket.h>` shielded behind compilation firewalls).
  * Strict binary compatibility (ABI stability) allowing shared libraries (`.dll`/`.so`) to update without recompiling client binaries.
* ⚡ **High-Throughput Concurrent Systems:**
  * Multi-producer multi-consumer queues with fine-grained locking (separate head and tail mutexes).
  * Thread pools with work-stealing scheduling and false-sharing-immune counters aligned to CPU cache lines (`hardware_destructive_interference_size`).
  * Lock-free communication channels utilizing explicit `acquire-release` atomic operations.
* 🧩 **Extensible Plugin Architectures:**
  * Hot-reloadable shared libraries exporting clean `extern "C"` interfaces.
  * Symmetric heap management ensuring cross-module memory allocations are freed by their owning runtime heap.
* 🏛️ **Domain-Driven Value Architectures:**
  * Eliminating primitive obsession via zero-overhead strong types (`Meter`, `Second`, `Velocity`) and User-Defined Literals (`30.0_m / 2.0_s`).
  * Modern functional error handling via `Result<T, E>` / C++23 `std::expected` monadic pipelines (`and_then`, `transform`), completely removing exceptions from routine business logic.
* 🛡️ **Zero-Leak System Wrappers:**
  * Generic RAII wrappers for raw OS resources (Win32 handles, POSIX file descriptors, sockets, database cursors) guaranteeing safe cleanup on all execution paths and exception unwinds.

---

## 📁 Repository Structure

```text
cpp-clean-code/
├── SKILL.md                          # Master specification and rulebook (~44 KB)
├── README.md                         # Documentation & installation guide
└── examples/                         # 13 verified, compile-ready reference implementations
    ├── type_erasure/                 # Value-Semantic Type Erasure (Iglberger)
    │   └── any_shape.hpp
    ├── variant_visitor/              # Modern std::variant + Overloaded Visitor
    │   └── variant_visitor.hpp
    ├── concurrent_queue/             # Two-Mutex Fine-Grained Queue (Anthony Williams)
    │   └── fine_grained_queue.hpp
    ├── monadic_result/               # C++23 std::expected-style Monadic Pipeline
    │   └── result.hpp
    ├── concepts_and_constraints/     # C++20 Concepts replacing SFINAE / enable_if_t
    │   └── modern_concepts.hpp
    ├── pimpl_idiom/                  # Pimpl Compilation Firewall & ABI Stability
    │   ├── autotimer.hpp
    │   └── autotimer.cpp
    ├── extensible_factory/           # Thread-safe callback-registered factory (OCP)
    │   └── renderer_factory.hpp
    ├── c_abi_plugin/                 # ABI-safe dynamic plugin boundary (.dll / .so)
    │   └── plugin_api.h
    ├── raii_resource/                # Universal zero-cost RAII wrapper for OS handles
    │   └── raii_handle.hpp
    ├── strong_types/                 # Type-Rich domain modeling & UDL physical units
    │   └── strong_types.hpp
    ├── scoped_thread/                # RAII thread lifecycle wrapper (prevents terminate)
    │   └── scoped_thread.hpp
    ├── copy_and_swap/                # Canonical Copy-and-Swap with noexcept swap
    │   └── copy_and_swap_buffer.hpp
    └── forwarding_constructor/       # Universal reference constructor with SFINAE guard
        └── perfect_forwarding_sink.hpp
```

---

## 🛠️ Installation & Setup

### 1. Via the Open Agent Skills Ecosystem (`skills.sh`)

Install directly into any supported AI workspace with a single command:

```bash
npx skills add deep4wee/cpp-clean-code
```

### 2. Google Antigravity

* **Project-local:** Copy the `cpp-clean-code` directory into your project root:
  ```text
  <your-project>/.agents/skills/cpp-clean-code/
  ```
* **Global (all projects):** Copy into your machine configuration:
  ```text
  ~/.gemini/config/skills/cpp-clean-code/
  ```

### 3. Cursor IDE

Copy the contents of `SKILL.md` directly into your project's `.cursorrules` or save as a rule under:
```text
.cursor/rules/cpp-clean-code.mdc
```

### 4. Claude Code / Claude Projects

Upload `SKILL.md` directly into your Project Knowledge or mount as a workspace skill.

### 5. Windsurf / GitHub Copilot Workspace

Mount the skill directory inside `.agents/skills/` or reference it in your workspace configuration instructions.

---

## ⚖️ Conflict Resolution Hierarchy

When architectural rules appear to conflict, the AI agent resolves them using this strict hierarchy:

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
└───────────────────────────┴─────────────────────────────┘
```

---

## 📋 Comprehensive Rule Taxonomy

| Category | Module Tag | Focus Area |
| :--- | :--- | :--- |
| **Philosophy & Design** | `[PHIL]` | Architecture vs. Tactical Design, Zero Entropy, Boy Scout Rule, KISS/YAGNI/DRY, Knuth Principle |
| **Naming & Clarity** | `[NAME]` | Domain-Driven Design (DDD), ban Hungarian notation, context-clean fields, predicate prefixes |
| **Value vs Reference** | `[VAL]` | Value Semantics by default, non-owning views (`span`, `string_view`) lifetime traps, iteration aliasing |
| **Type System & Concepts**| `[TYPE]` | Safe `auto`, proxy types, `{}` vs `()`, `decltype(auto)`, scoped enums, `explicit`, C++20 Concepts |
| **Resource Management** | `[RES]` | Universal RAII, Rule of Zero / Five, ban raw `new`/`delete`, smart pointers, Copy-and-Swap |
| **Move Semantics** | `[MOVE]` | `std::move` vs `std::forward`, RVO/NRVO protection, `const` move trap, sink pass-by-value |
| **Clean Functions** | `[FUNC]` | SRP, no boolean flags, no out-parameters, C++23 monadic Result, parameter matrix, ranges |
| **Modern Patterns** | `[DES]` | Modern GoF: Type Erasure, `std::variant` Visitor, functional Strategy, Expression Problem matrix |
| **API & ABI Stability** | `[API]` | Meyers' Rule, Pimpl with `unique_ptr`, Partial Bridge (cache optimization), ABI Breaking vs Safe |
| **OOP & Architecture** | `[OOP]` | SRP & private method extraction, LSP (Square-Rectangle), Interface Segregation, DIP, DI vs Singleton |
| **High Concurrency** | `[CONC]` | Thread lifecycle (`std::jthread`), interface races, `scoped_lock`, memory orders, false sharing |
| **Testing & TDD** | `[TEST]` | Test Pyramid, test isolation via dependency injection, AAA pattern, Red-Green-Refactor |
| **Verification Pass** | `[REV]` | 12-point systematic verification checklist for automated code reviews and validation |

---

## 📄 License

Distributed under the MIT License. See [LICENSE](LICENSE) for details.
