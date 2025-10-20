#pragma once
#include <atomic>
#include <mutex>

namespace smart_ptrs {
    template <typename T>
    class control_block_base { 
    protected:
        std::mutex m;
        size_t strong_count_(1);
        size_t weak_count_(0);

        /* Increments, Decrements */
        void increment_strong() noexcept { 
            std::lock_guard<std::mutex> lg(m);
            strong_count_++;
        }
        
        void increment_weak() noexcept {
            std::lock_guard<std::mutex> lg(m);
            weak_count_++;
        }

    public:
        control_block() {}
        virtual ~control_block() = default;

        long use_strong_count() const noexcept {
            std::lock_guard<std::mutex> lg(m);
            return strong_count_;
        }

        virtual void release_strong(T* p) = 0;
        virtual void release_weak(T* p) = 0;
    };

    template <typename T>
    class control_block_separate final : public control_block_base {
    public:
        void release_strong(T* p) override {
            std::lock_guard<std::mutex> lg(m);
            if (strong_count_ == 1) {
                delete p;
                if (weak_count_ == 0) {
                    delete this;
                }
            }
        }

        void release_weak(T* p) override {
            std::lock_guard<std::mutex> lg(m);
            if (weak_count_ == 1 && strong_count_ == 0) {
                delete this;
            }
        }
    };

    template <typename T>
    class control_block_adjacent final : public control_block_base {
    public:
        void release_strong(T* p) override {
            std::lock_guard<std::mutex> lg(m);
            if (strong_count_ == 1) {
                p->~T();
                if (weak_count_ == 0) {
                    ::operator delete(this); // will this free everything (data + cb) ?
                }
            }
        }

        void release_weak(T* p) override {
            std::lock_guard<std::mutex> lg(m);
            if (weak_count_ == 1 && strong_count_ == 0) {
                ::operator delete(this);
            }
        }
    };

} /* smart_ptrs */