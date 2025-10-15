#pragma once
#include <atomic>
#include "control_block.hpp"

namespace smart_ptrs {
    template <typename T>
    class shared_ptr { 
    public:
        /* Constructors */
        shared_ptr(void) constexpr noexcept : data_(nullptr) {}
        explicit shared_ptr(T* p) : data_(p), cb_(new control_block()) {}
        explicit shared_ptr(T* p, control_block *c) : data_(p), cb_(c) {}
        
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

    // problem with this is the deallocation in control_block
    // if the shared_ptr was created with make_shared , we need to call the destructors of both data and ctrl_blokc
    // but if not , it should be as it is now
    // options : 
    // let ctrl_block be only the counter, not in charge of deallocating, and have a flag so shared_ptr knows how to deallocate ?
        // bc shared_ptr doesnt need a ref to void*block right ? bc it starts in cb
    
    // BEST APPROACH (i think) :
    // ctrl_block subclasses ?i could do functions for decrementing the counts in the base class, then those ft call the destroy_data / destroy_this 
    // and each subclass will call its defined one
    
    // let make_shared do two allocations
    template<typename T, typename... Args> 
    shared_ptr<T> make_shared(Args&& ... args) {
        void *block = ::operator new(sizeof(T) + sizeof(control_block));
        T* data = new(block + sizeof(controlblock)) T(std::forward<Args>(args)...);
        control_block *cb = new(block) control_block();
        return shared_ptr(data, cb);
    }

    // comparison operators, << operator
    // make_shared
    // allocate_shared ?
    // casts to pointer ?
    // need to std::swap ?

} /* smart_ptrs */