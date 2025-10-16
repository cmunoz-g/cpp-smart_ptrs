#pragma once
#include <atomic>

namespace smart_ptrs {
    template <typename T>
    class control_block_base { 
    protected:
        std::atomic<size_t> strong_count_(1);
        std::atomic<size_t> weak_count_(0);

        /* Increments, Decrements */
        void increment_strong() noexcept { 
            strong_count_.fetch_add(1, std::memory_order_relaxed);
        }

        void increment_weak() noexcept {
            weak_count_.fetch_add(1, std::memory_order_relaxed);
        }

    public:
        control_block() {}
        virtual ~control_block() = default;

        long use_strong_count() const noexcept {
            return strong_count_.load(std::memory_order_acquire);
        }
        
        // i think this is not needed
        long use_weak_count() const noexcept {
            return weak_count_.load(std::memory_order_acquire)
        }

        // for both this functions: can we check two counters atomically ?
        // check notes on TODO
        virtual void release_strong(T* p) = 0;
        virtual void release_weak(T* p) = 0;
    };

    template <typename T>
    class control_block_separate final : public control_block_base {
    public:
        void release_strong(T* p) override {
            if (strong_count_.load(std::memory_order_acquire) == 1) {
                delete p;
                if (weak_count_.load(std::memory_order_acquire) == 0) {
                    delete this;
                }
            }
        }

        void release_weak(T* p) override {
            if (weak_count_.load(std::memory_order_acquire) == 1 &&
                strong_count_.load(std::memory_order_acquire) == 0) {
                    delete this;
            }
        }
    };

    template <typename T>
    class control_block_adjacent final : public control_block_base {
    public:
        void release_strong(T* p) override {
            if (strong_count_.load(std::memory_order_acquire) == 1) {
                p->~T();
                if (weak_count_.load(std::memory_order_acquire == 0)) {
                    ::operator delete(this); // will this free everything (data + cb) ?
                }
            }
        }

        void release_weak(T* p) override {
            if (weak_count_.load(std::memory_order_acquire) == 1 &&
                strong_count_.load(std::memory_order_acquire) == 0) {
                    ::operator delete(this);
                }
        }
    };

} /* smart_ptrs */