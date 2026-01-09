#pragma once

#include <atomic>
#include <cstdint>

namespace kimoyooju {

struct MemoryCounters {
    std::atomic<uint32_t> liveNodes{0};
    std::atomic<uint32_t> liveTextures{0};
    std::atomic<uint32_t> liveBuffers{0};
    std::atomic<uint32_t> liveSwapchains{0};
    std::atomic<uint32_t> liveShaders{0};
    std::atomic<uint64_t> gpuMemoryBytes{0};
    std::atomic<uint64_t> cpuMemoryBytes{0};
    
    struct Snapshot {
        uint32_t liveNodes;
        uint32_t liveTextures;
        uint32_t liveBuffers;
        uint32_t liveSwapchains;
        uint32_t liveShaders;
        uint64_t gpuMemoryBytes;
        uint64_t cpuMemoryBytes;
    };
    
    Snapshot snapshot() const {
        return Snapshot{
            liveNodes.load(std::memory_order_relaxed),
            liveTextures.load(std::memory_order_relaxed),
            liveBuffers.load(std::memory_order_relaxed),
            liveSwapchains.load(std::memory_order_relaxed),
            liveShaders.load(std::memory_order_relaxed),
            gpuMemoryBytes.load(std::memory_order_relaxed),
            cpuMemoryBytes.load(std::memory_order_relaxed)
        };
    }
    
    void reset() {
        liveNodes = 0;
        liveTextures = 0;
        liveBuffers = 0;
        liveSwapchains = 0;
        liveShaders = 0;
        gpuMemoryBytes = 0;
        cpuMemoryBytes = 0;
    }
    
    bool hasLeaks() const {
        return liveNodes > 0 || liveTextures > 0 || liveBuffers > 0 || 
               liveSwapchains > 0 || liveShaders > 0;
    }
};

inline MemoryCounters& globalMemoryCounters() {
    static MemoryCounters instance;
    return instance;
}

class ScopedNodeCounter {
public:
    ScopedNodeCounter() { globalMemoryCounters().liveNodes++; }
    ~ScopedNodeCounter() { globalMemoryCounters().liveNodes--; }
    ScopedNodeCounter(const ScopedNodeCounter&) = delete;
    ScopedNodeCounter& operator=(const ScopedNodeCounter&) = delete;
};

class ScopedTextureCounter {
public:
    ScopedTextureCounter() { globalMemoryCounters().liveTextures++; }
    ~ScopedTextureCounter() { globalMemoryCounters().liveTextures--; }
    ScopedTextureCounter(const ScopedTextureCounter&) = delete;
    ScopedTextureCounter& operator=(const ScopedTextureCounter&) = delete;
};

class ScopedBufferCounter {
public:
    ScopedBufferCounter() { globalMemoryCounters().liveBuffers++; }
    ~ScopedBufferCounter() { globalMemoryCounters().liveBuffers--; }
    ScopedBufferCounter(const ScopedBufferCounter&) = delete;
    ScopedBufferCounter& operator=(const ScopedBufferCounter&) = delete;
};

class ScopedSwapchainCounter {
public:
    ScopedSwapchainCounter() { globalMemoryCounters().liveSwapchains++; }
    ~ScopedSwapchainCounter() { globalMemoryCounters().liveSwapchains--; }
    ScopedSwapchainCounter(const ScopedSwapchainCounter&) = delete;
    ScopedSwapchainCounter& operator=(const ScopedSwapchainCounter&) = delete;
};

} // namespace kimoyooju
