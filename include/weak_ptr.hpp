#pragma once
#include "control_block.hpp"
#include "shared_ptr.hpp"

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
        void reset() noexcept { release(); }
        
        void swap(weak_ptr& r) noexcept {
            std::swap(cb_, r._cb);
            std::swap(data_, r._data);
        } 

        /* Observers */
        long use_count(void) const noexcept { return cb_ ? cb_->use_strong_count() : 0; }
        bool expired(void) const noexcept { return use_count() == 0; }
        
        shared_ptr<T> lock() const noexcept {
            return expired() ? shared_ptr<T>() : shared_ptr<T>(*this);
        }
        
        template<class Y>
        bool owner_before(const weak_ptr<Y>& other) const noexcept {
            return cb_ < other.cb_;
        }

        template<class Y>
        bool owner_before(const shared_ptr<Y>& other) const noexcept {
            return cb_ < other.cb_;
        }

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
    };

}; /* smart_ptrs */