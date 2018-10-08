#ifndef CHFL_SHARED_ALLOCATOR_HPP
#define CHFL_SHARED_ALLOCATOR_HPP

#include <unordered_map>
#include <functional>
#include <cassert>
#include <cstdlib>
#include <vector>
#include <atomic>

#include "chemfiles/ErrorFmt.hpp"

namespace chemfiles {

/// An atomic counter, for reference counted pointers
class atomic_count {
public:
    explicit atomic_count(long value): value_(static_cast<std::int_least32_t>(value)) {}
    ~atomic_count() = default;

    atomic_count(const atomic_count&) = delete;
    atomic_count& operator=(const atomic_count&) = delete;

    atomic_count(atomic_count&& other): value_(static_cast<std::int_least32_t>(other.load())) {
        other.value_ = 0;
    }

    atomic_count& operator=(atomic_count&& other) {
        value_ = static_cast<std::int_least32_t>(other.load());
        other.value_ = 0;
        return *this;
    }

    // Increase counter by one and return the resulting value
    long increase() {
        return value_.fetch_add(1, std::memory_order_acq_rel) + 1;
    }

    // Decrease the counter by one and return the resulting value
    long decrease() {
        return value_.fetch_sub(1, std::memory_order_acq_rel) - 1;
    }

    // Get the current counter value
    long load() const {
        return value_.load(std::memory_order_acquire);
    }

private:
    std::atomic_int_least32_t value_;
};


struct shared_metadata {
    shared_metadata(long count_, std::function<void(void)> deleter_):
        count(count_), deleter(std::move(deleter_)) {}

    /// Number of pointer sharing this reference
    atomic_count count;
    /// How to delete the pointer when we are done with it
    std::function<void(void)> deleter;
};

/// An allocator with shared_ptr like semantics, working with raw pointers.
///
/// This is used in the C API to ensure that when taking pointers to
/// atoms/residues/cell inside a frame/topology, the frame/topology is keept
/// alive even if the user call chfl_xxx_free.
class shared_allocator {
public:
    shared_allocator() = default;
    shared_allocator(const shared_allocator&) = delete;
    shared_allocator& operator=(const shared_allocator&) = delete;
    shared_allocator(shared_allocator&&) = delete;
    shared_allocator& operator=(shared_allocator&&) = delete;

    /// Like std::make_shared: create a new shared pointer by constructing a
    /// value of type T with the given arguments.
    template<class T, typename ... Args>
    static T* make_shared(Args&& ... args) {
        auto ptr = new T{std::forward<Args>(args)...};
        instance_.insert_new(ptr);
        return ptr;
    }

    /// Like std::shared_ptr<U> aliasing contructor: element and ptr will share
    /// the references count, and none will be freed while the other one is
    /// alive.
    ///
    /// `ptr` must have been allocated with make_shared.
    template<class T, class U>
    static T* shared_ptr(U* ptr, T* element) {
        instance_.insert_shared(ptr, element);
        return element;
    }

    template<class T, class U>
    static const T* shared_ptr(U* ptr, const T* element) {
        return shared_ptr(ptr, const_cast<T*>(element));
    }

    /// Decrease the reference count of `ptr`, and delete it if needed.
    static void free(const void* ptr) {
        auto it = instance_.map_.find(ptr);
        if (it == instance_.map_.end()) {
            throw chemfiles::error(
                "unknown pointer passed to shared_allocator::free: {}", ptr
            );
        }
        auto references = instance_.metadata_.at(it->second).count.decrease();
        if (references == 0) {
            instance_.metadata_.at(it->second).deleter();
            instance_.unused_.emplace_back(it->second);

            // Remove any pointer that was using the same metadata block
            auto to_remove = std::vector<const void*>();
            for (const auto& registered: instance_.map_) {
                if (registered.second == it->second) {
                    to_remove.emplace_back(registered.first);
                }
            }
            for (auto remove: to_remove) {
                instance_.map_.erase(remove);
            }
        } else if (references < 0) {
            throw chemfiles::error(
                "internal error: negative reference count for {}", ptr
            );
        }
    }

private:
    template<class T>
    void insert_new(T* ptr) {
        size_t id = get_unused_metadata();
        metadata_[id] = shared_metadata{1, [ptr](){ delete ptr; }};
        auto inserted = map_.emplace(ptr, id);
        if (!inserted.second) {
            throw chemfiles::error(
                "internal error: pointer at {} is already managed by "
                "shared_allocator", static_cast<void*>(ptr)
            );
        }
    }

    void insert_shared(const void* ptr, void* element) {
        auto it = map_.find(ptr);
        if (it == map_.end()) {
            // the main pointer is not a shared pointer
            throw chemfiles::error(
                "internal error: pointer at {} is not managed by "
                "shared_allocator", ptr
            );
        }
        auto inserted = map_.emplace(element, it->second);
        if (!inserted.second && (inserted.first->second != it->second)) {
            // the element pointer is already registered, but with a different
            // main pointer
            throw chemfiles::error(
                "internal error: element pointer at {} is already managed by "
                "shared_allocator (associated with {})", element, ptr
            );
        }
        metadata_.at(it->second).count.increase();
    }

    shared_metadata& metadata(const void* ptr) {
        auto it = map_.find(ptr);
        if (it != map_.end()) {
            return metadata_.at(it->second);
        } else {
            throw chemfiles::error(
                "internal error: unknwon pointer passed to shared_allocator::metadata"
            );
        }
    }

    size_t get_unused_metadata() {
        if (!unused_.empty()) {
            // Get an existing one
            size_t id = unused_.back();
            unused_.pop_back();
            return id;
        } else {
            // create a new one
            metadata_.emplace_back(0, [](){ throw chemfiles::error("uninitialized deleter");});
            return metadata_.size() - 1;
        }
    }

    /// A map of pointer adresses -> indexes of metadata in metadatas_
    std::unordered_map<const void*, size_t> map_;
    /// Metadata for all known pointers
    std::vector<shared_metadata> metadata_;
    /// unused indexes in metadatas_ that can be re-used. This is set by
    /// free and used by get_unused_count.
    std::vector<size_t> unused_;

    /// Global instance of the allocator
    static shared_allocator instance_;
};

}

#endif
