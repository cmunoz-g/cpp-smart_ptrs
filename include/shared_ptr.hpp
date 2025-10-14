#pragma once

namespace ptrs {

    template <typename T>
    class control_block { // add: custom allocator, deleter ? weak ref count ?
    private:
        uint64_t count_;
        //weak ref count_
    public:
        control_block(void) : count_(1) {}
        uint64_t use_count(void) const noexcept { return count_; }

        void release(T* p) {
            count_--;
            if (count_ == 0) {
                delete p;
                delete this;
            }
        }

        void increment(void) const noexcept { count_++; } // change name to acquire() ?
    };

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
        // alias constructor ?
        // constructor from weak_ptr

        /* Destructor */
        ~shared_ptr(void) { release(); }

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

        /* Modifiers */
        
        
    private:
        control_block *cb_;
        T* data_;

        void release(void) {
            if (cb_) {
                cb_->release();
                cb_ = nullptr;
                data_ = nullptr;
            }
        }


    };

} /* ptrs */