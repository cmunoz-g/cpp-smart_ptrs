#pragma once
#include "control_block.hpp"
#include "weak_ptr.hpp"

namespace smart_ptrs {
    template <typename T>
    class shared_ptr { 
    public:
        /* Constructors */
        constexpr shared_ptr(void) noexcept : data_(nullptr), cb_(nullptr) {}
        explicit shared_ptr(T* p) : data_(p), cb_(new control_block_separate()) {}
        explicit shared_ptr(T* p, control_block *c) : data_(p), cb_(c) {}
        
        shared_ptr(const shared_ptr &r) noexcept {
            *this = r;
            if (cb_) cb_->increment_strong();
        }

        shared_ptr(shared_ptr &&r) noexcept {
            //
        }

        template<typename Y>
        explicit shared_ptr(const weak_ptr<Y>& r) { // review
            shared_ptr(r.data_, r.cb_); 
        }

        // copy constructor to convert other types ? !!! review implementation details on cppreference

        /* Destructor */
        // doubt: why not just call release()
        ~shared_ptr() { if (cb_) cb_->release_strong(); }

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
                cb_ = r.cb_;
                data_ = r.data_;
                // set r stuff to null, without changing count in the cb . maybe i can call swap ?
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
        void reset(void) noexcept { release(); } 
        // add : another reset() that replaces the managed object

        void swap(shared_ptr &r) noexcept // check implementation
        {
            std::swap(data_, r.data_);
            std::swap(cb_, r.cb_);
        }

        /* Observers */
        T* get(void) const noexcept { return data_; }
        long use_count(void) const noexcept { return cb_ ? cb_->use_strong_count() : 0; } // Correct ? what does "returns 0 if there is no managed object." exactly mean?
        
        template<typename Y>
        bool owner_before(const shared_ptr<Y> &other) const noexcept {
            return cb_ < other.cb_;
        }

        template<typename Y>
        bool owner_before(const weak_ptr<Y> &other) const noexcept {
            return cb_ < other.cb_;
        }
        
    private:
        control_block_base *cb_;
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

    template<typename T, typename... Args> 
    shared_ptr<T> make_shared(Args&& ... args) {
        void *block = ::operator new(sizeof(T) + sizeof(control_block));
        T* data = new(block + sizeof(controlblock)) T(std::forward<Args>(args)...);
        control_block_adjacent *cb = new(block) control_block_adjacent();
        return shared_ptr(data, cb);
    }

    // comparison operators, << operator
    // make_shared
    // allocate_shared ?
    // casts to pointer ?
    // need to std::swap ? diff between std::swap and swap method ?

} /* smart_ptrs */