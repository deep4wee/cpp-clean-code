#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

/**
 * @file renderer_factory.hpp
 * @brief Extensible Callback-Based Factory Pattern (Open/Closed Principle).
 *
 * Demonstrates:
 * 1. Open/Closed Principle (OCP): New types can be registered at runtime without
 *    modifying or recompiling the factory (eliminates switch/case ladders).
 * 2. Modern Resource Management: Returns std::unique_ptr<IRenderer>.
 * 3. Thread Safety: Safe registration and creation using std::shared_mutex.
 * 4. Automatic Self-Registration: Helper struct to register plugins statically.
 */

namespace patterns::factory {

// 1. Abstract Pure Virtual Interface (with virtual destructor)
class IRenderer {
public:
    virtual ~IRenderer() = default;

    virtual void load_scene(std::string_view scene_path) = 0;
    virtual void render_frame() = 0;
    [[nodiscard]] virtual std::string_view name() const noexcept = 0;
};

// 2. Extensible Factory
class RendererFactory final {
public:
    using CreatorCallback = std::function<std::unique_ptr<IRenderer>()>;

    RendererFactory() = delete; // Pure static utility class

    // Register a new type creator
    static bool register_renderer(std::string_view type_id, CreatorCallback creator) {
        std::unique_lock<std::shared_mutex> lock(s_mutex);
        if (creator == nullptr) {
            return false;
        }
        return s_registry.emplace(std::string(type_id), std::move(creator)).second;
    }

    // Unregister a type
    static bool unregister_renderer(std::string_view type_id) {
        std::unique_lock<std::shared_mutex> lock(s_mutex);
        return s_registry.erase(std::string(type_id)) > 0;
    }

    // Create an instance of the requested renderer
    [[nodiscard]] static std::unique_ptr<IRenderer> create(std::string_view type_id) {
        std::shared_lock<std::shared_mutex> lock(s_mutex);
        const auto it = s_registry.find(std::string(type_id));
        if (it != s_registry.end()) {
            return (it->second)(); // Invoke creator callback
        }
        return nullptr;
    }

    // Inspect registered types (useful for UI menus or introspection)
    [[nodiscard]] static std::vector<std::string> registered_types() {
        std::shared_lock<std::shared_mutex> lock(s_mutex);
        std::vector<std::string> types;
        types.reserve(s_registry.size());
        for (const auto& [key, _] : s_registry) {
            types.push_back(key);
        }
        return types;
    }

private:
    inline static std::unordered_map<std::string, CreatorCallback> s_registry;
    inline static std::shared_mutex s_mutex;
};

// 3. Helper for Automated Self-Registration
// Place in .cpp file:
// static patterns::factory::RendererRegistrar<VulkanRenderer> s_vulkan_reg("vulkan");
template <typename ConcreteRenderer>
struct RendererRegistrar {
    explicit RendererRegistrar(std::string_view type_id) {
        RendererFactory::register_renderer(type_id, []() -> std::unique_ptr<IRenderer> {
            return std::make_unique<ConcreteRenderer>();
        });
    }
};

} // namespace patterns::factory
