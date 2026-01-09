#pragma once

#include "kimoyooju/Types.hpp"
#include <vector>
#include <optional>
#include <mutex>
#include <cassert>

namespace kimoyooju {

template<typename T>
class HandleRegistry {
public:
    explicit HandleRegistry(uint32_t maxCapacity = constants::MAX_NODES)
        : maxCapacity_(maxCapacity) {
        slots_.reserve(1024);
        slots_.emplace_back(); // Slot 0 is reserved (invalid handle)
    }
    
    Handle allocate(T&& value) {
        std::lock_guard lock(mutex_);
        
        uint32_t slotId;
        
        if (!freeList_.empty()) {
            slotId = freeList_.back();
            freeList_.pop_back();
            assert(slotId < slots_.size());
            assert(!slots_[slotId].value.has_value());
        } else {
            if (slots_.size() >= maxCapacity_) {
                return Handle::invalid();
            }
            slotId = static_cast<uint32_t>(slots_.size());
            slots_.emplace_back();
        }
        
        Slot& slot = slots_[slotId];
        slot.value = std::move(value);
        // Generation already incremented when released
        
        return Handle{slotId, slot.generation};
    }
    
    std::optional<T*> get(Handle h) {
        std::lock_guard lock(mutex_);
        return getUnlocked(h);
    }
    
    std::optional<const T*> get(Handle h) const {
        std::lock_guard lock(mutex_);
        return getUnlocked(h);
    }
    
    bool release(Handle h) {
        std::lock_guard lock(mutex_);
        
        if (!h.isValid() || h.id >= slots_.size()) {
            return false;
        }
        
        Slot& slot = slots_[h.id];
        
        // Generation mismatch means already released
        if (slot.generation != h.generation) {
            return false;
        }
        
        // Already released
        if (!slot.value.has_value()) {
            return false;
        }
        
        // Clear value (destructor runs here)
        slot.value.reset();
        
        // Increment generation for next use
        slot.generation++;
        
        // Add to free list
        freeList_.push_back(h.id);
        
        return true;
    }
    
    bool isValid(Handle h) const {
        std::lock_guard lock(mutex_);
        
        if (!h.isValid() || h.id >= slots_.size()) {
            return false;
        }
        
        const Slot& slot = slots_[h.id];
        return slot.generation == h.generation && slot.value.has_value();
    }
    
    size_t activeCount() const {
        std::lock_guard lock(mutex_);
        return slots_.size() - 1 - freeList_.size(); // -1 for reserved slot 0
    }
    
    void clear() {
        std::lock_guard lock(mutex_);
        slots_.clear();
        freeList_.clear();
        slots_.emplace_back(); // Re-add reserved slot 0
    }

private:
    struct Slot {
        std::optional<T> value;
        uint32_t generation = 1; // Start at 1 so gen 0 is always invalid
    };
    
    std::optional<T*> getUnlocked(Handle h) {
        if (!h.isValid() || h.id >= slots_.size()) {
            return std::nullopt;
        }
        
        Slot& slot = slots_[h.id];
        
        if (slot.generation != h.generation || !slot.value.has_value()) {
            return std::nullopt;
        }
        
        return &slot.value.value();
    }
    
    std::optional<const T*> getUnlocked(Handle h) const {
        if (!h.isValid() || h.id >= slots_.size()) {
            return std::nullopt;
        }
        
        const Slot& slot = slots_[h.id];
        
        if (slot.generation != h.generation || !slot.value.has_value()) {
            return std::nullopt;
        }
        
        return &slot.value.value();
    }
    
    mutable std::mutex mutex_;
    std::vector<Slot> slots_;
    std::vector<uint32_t> freeList_;
    uint32_t maxCapacity_;
};

} // namespace kimoyooju
