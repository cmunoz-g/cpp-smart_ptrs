#pragma once
#include <atomic>

namespace ptrs {
    template <typename T>
    class shared_ptr { 
    public:
        /* Constructors */
        shared_ptr(void) constexpr noexcept : data_(nullptr) {}
        explicit shared_ptr(T* p) : data_(p), cb_(new control_block()) {}
        
        shared_ptr(const shared_ptr &r) noexcept {
            *this = r;
            if (cb_) cb_->increment();
        }

        shared_ptr(shared_ptr &&r) noexcept {
            *this = r;
        }

        // copy constructor to convert other types ?
        // constructor from weak_ptr

        /* Destructor */
        ~shared_ptr(void) { if (cb_) release_strong(); }

        /* Operators */
        shared_ptr& operator=(const shared_ptr &r) noexcept {
            if (*this != r) {
                release();
                cb_ = r.cb_;
                data_ = r.data_;
            }
            return *this;
        }

        shared_ptr& operator=(shared_ptr &&r) noexcept {
            if (*this != r) {
                r.release();
                cb_ = r.cb_;
                data_ = r.data_;
                r.cb_ = nullptr;
                r.data_ = nullptr;
            }
            return *this;
        }

        shared_ptr& operator=(std::nullptr_t) noexcept {
            release();
            cb_ = nullptr;
            data_ = nullptr;
            return *this;
        }

        T& operator*() const noexcept { return *data_; }
        T* operator->() const noexcept { return data_; }
        operator bool() const noexcept { return data_ != nullptr; }
        // operator[] ?

        /* Modifiers */
        void reset(void) noexcept { ~shared_ptr(); } // correct ?

        void swap(shared_ptr &r) noexcept
        {
            std::swap(data_, r.data_);
            std::swap(cb_, r.cb_);
        }

        /* Observers */
        T* get(void) const noexcept { return data_; }
        long use_count(void) const noexcept { return cb_ ? cb_->use_strong_count() : 0; } // Correct ? what does "returns 0 if there is no managed object." exactly mean?
        // owner_before()
        
    private:
        control_block *cb_;
        T* data_;

        void release(void)
        {
            if (cb_) {
                cb_->release_strong();
                cb_ = nullptr;
                data_ = nullptr;
            }
        }
    };

    // comparison operators, << operator
    // make_shared
    // allocate_shared ?
    // casts to pointer ?
    // need to std::swap ?

} /* ptrs */