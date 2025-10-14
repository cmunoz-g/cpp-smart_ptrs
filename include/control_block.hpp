#pragma once
namespace ptrs {
    template <typename T>
    class control_block { 
    private:
        std::atomic<size_t> strong_count_(1);
        std::atomic<size_t> weak_count_(0);

        /* Increments, Decrements */
        void increment_strong(void) noexcept { 
            strong_count_.fetch_add(1, std::memory_order_relaxed);
        }

        void increment_weak(void) noexcept {
            weak_count_.fetch_add(1, std::memory_order_relaxed);
        }

    public:
        control_block(void) {}

        long use_strong_count(void) const noexcept {
            return strong_count_.load(std::memory_order_acquire);
        }
        
        long use_weak_count(void) const noexcept {
            return weak_count_.load(std::memory_order_acquire)
        }

        void release_strong(T* p) {
            if (strong_count_.load(std::memory_order_acquire) == 1) {
                delete p;
                if (weak_count_.load(std::memory_order_acquire == 0)) {
                    delete this;
                }
            }
        }

        void release_weak(T* p) {
            if (weak_count_.load(std::memory_order_acquire) == 1 &&
                strong_count_.load(std::memory_order_acquire) == 0) { 
                    delete this;
                }
        }
    };
} /* ptrs */