#pragma once

#include <cstdint>
#include <cstddef>

/**
 * @file plugin_api.h
 * @brief ABI-Safe C-Interface for Dynamically Loaded Shared Libraries (.dll / .so).
 *
 * Rules for Cross-Boundary DLL/DSO Architecture:
 * 1. C-Linkage (extern "C"): Prevents compiler-specific C++ name mangling issues.
 * 2. Pure Virtual Interfaces: Relies only on standard vtable layout.
 * 3. Symmetric Heap Management: The module that allocates memory MUST free it.
 *    Never call 'delete' across DLL boundaries (mismatched CRT heaps cause crashes).
 * 4. Zero STL / Zero Exceptions across ABI boundary:
 *    Never pass std::string, std::vector, or throw C++ exceptions across DLL boundaries.
 * 5. Strict Error Codes: Use plain integer or enum return codes for cross-boundary errors.
 */

#if defined(_WIN32) || defined(__CYGWIN__)
    #ifdef BUILDING_PLUGIN_DLL
        #define PLUGIN_API __declspec(dllexport)
    #else
        #define PLUGIN_API __declspec(dllimport)
    #endif
#else
    #if __GNUC__ >= 4
        #define PLUGIN_API __attribute__((visibility("default")))
    #else
        #define PLUGIN_API
    #endif
#endif

namespace abi_plugin {

// 1. Cross-Boundary Error Codes
enum class PluginStatus : int32_t {
    Success = 0,
    InvalidArguments = 1,
    ExecutionFailed = 2,
    UnsupportedVersion = 3
};

// 2. Pure Virtual Interface Contract
// All methods use C-compatible types only!
struct IAudioProcessor {
    virtual ~IAudioProcessor() = default;

    // ABI-safe data transfer
    virtual PluginStatus process_samples(const float* input, float* output, size_t sample_count) noexcept = 0;
    virtual const char* get_name() const noexcept = 0;
    virtual uint32_t get_version() const noexcept = 0;

    // SYMMETRIC DESTRUCTION:
    // Destroys instance within the plugin's own CRT heap.
    virtual void release() noexcept = 0;
};

// 3. Plugin Initialization Information
struct PluginDescriptor {
    uint32_t api_major_version;
    uint32_t api_minor_version;
    const char* plugin_name;
    const char* author;
};

// Constant API version for handshake verification
constexpr uint32_t API_MAJOR_VERSION = 1;
constexpr uint32_t API_MINOR_VERSION = 0;

// 4. Function Pointer Signatures for Dynamic Symbol Resolution (GetProcAddress / dlsym)
using CreateProcessorFunc = IAudioProcessor* (*)();
using GetDescriptorFunc = const PluginDescriptor* (*)();

} // namespace abi_plugin

// 5. C-Linkage Export Declarations
extern "C" {
    PLUGIN_API const abi_plugin::PluginDescriptor* get_plugin_descriptor() noexcept;
    PLUGIN_API abi_plugin::IAudioProcessor* create_audio_processor() noexcept;
}
