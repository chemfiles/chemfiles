#ifndef CHFL_SHARED_ALLOCATOR_HPP
#define CHFL_SHARED_ALLOCATOR_HPP

#include <unordered_map>
#include <functional>
#include <cassert>
#include <vector>
#include <atomic>

#include "chemfiles/ErrorFmt.hpp"

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
        auto& count = instance_.count(ptr);
        count.increase();
        return element;
    }

    template<class T, class U>
    static const T* shared_ptr(U* ptr, const T* element) {
        return shared_ptr(ptr, const_cast<T*>(element));
    }

    /// Decrease the reference count of `ptr`, and delete it if needed.
    static void free(const void* ptr) {
        auto it = instance_.map_.find(ptr);
        auto& count = instance_.counts_.at(it->second);
        auto references = count.decrease();
        if (references == 0) {
            instance_.deleters_.at(it->second)();
            instance_.unused_counts_.emplace_back(it->second);
        } else if (references < 0) {
            throw chemfiles::error(
                "internal errpr: negative reference count for {}", ptr
            );
        }
        instance_.map_.erase(it);
    }

private:
    template<class T>
    void insert_new(T* ptr) {
        size_t id = get_unused_count();
        counts_[id] = atomic_count(1);
        deleters_[id] = [ptr](){ delete ptr; };
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
            throw chemfiles::error(
                "internal error: pointer at {} is not managed by "
                "shared_allocator", ptr
            );
        }
        auto inserted = map_.emplace(element, it->second);
        if (!inserted.second) {
            if (inserted.first->second == it->second) {
                // We already have a shared pointer to this element, just
                // increase the reference count
                counts_.at(it->second).increase();
            } else {
                throw chemfiles::error(
                    "internal error: pointer at {} is already managed by "
                    "shared_allocator", ptr
                );
            }
        }
    }

    atomic_count& count(const void* ptr) {
        auto it = map_.find(ptr);
        if (it != map_.end()) {
            return counts_.at(it->second);
        } else {
            throw std::runtime_error("unknwon pointer");
        }
    }

    size_t get_unused_count() {
        assert(counts_.size() == deleters_.size());
        if (!unused_counts_.empty()) {
            // Get an existing one
            size_t id = unused_counts_.back();
            unused_counts_.pop_back();
            return id;
        } else {
            // create a new one
            size_t id = counts_.size();
            counts_.emplace_back(0);
            deleters_.emplace_back([](){ throw std::runtime_error("uninitialized deleter"); });
            return id;
        }
    }

    /// A map of pointer adresses -> indexes of reference count in counts_ and
    /// deleter function in deleters_
    std::unordered_map<const void*, size_t> map_;
    /// References counts
    std::vector<atomic_count> counts_;
    /// Deleter functions
    std::vector<std::function<void(void)>> deleters_;
    /// unused indexes in counts_/deleters_ that can be re-used. This is set by
    /// free and used by get_unused_count.
    std::vector<size_t> unused_counts_;

    /// Global instance of the allocator
    static shared_allocator instance_;
};

#endif
