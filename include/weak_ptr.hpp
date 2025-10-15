#pragma once
#include "control_block.hpp"

namespace smart_ptrs {
    template <typename T>
    class weak_ptr {
    public:
        /* Constructors */
        constexpr weak_ptr(void) noexcept : data_(nullptr), cb_(nullptr) {}
        
        weak_ptr(const weak_ptr& r) noexcept {
            *this = r;
            if (cb_) cb_->increment_weak();
        }

        weak_ptr(weak_ptr&& r) noexcept {
            *this = r;
        }
        // templated constructors ?

        /* Destructor */
        ~weak_ptr() {if (cb_) cb_->release_weak(); }

        /* Operators */
        weak_ptr& operator=(const weak_ptr& r) noexcept {
            if (*this != r) {
                release();
                cb_ = r.cb_;
                data_ = r.data_;
            }
            return *this;
        }

        weak_ptr& operator=(weak_ptr&& r) noexcept{
            if (*this != r) {
                cb_ = r.cb_;
                data_ = r.data_;
                // set r stuff to null, without changing count in the cb
            }
        }

        /* Modifiers */
        // reset
        // swap

        /* Observers */
        // use count
        // expired
        // lock
        // owner_before

    private:
        T* data_;
        control_block_base *cb_;

        void release(void) {
            if (cb_) {
                cb_->release_weak();
                cb_ = nullptr;
                data_ = nullptr;
            }
        }
    }

}; /* smart_ptrs */