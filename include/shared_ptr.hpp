#pragma once
#include "control_block.hpp"
#include "weak_ptr.hpp"
#include <compare>
#include <ostream>

namespace smart_ptrs {
    template<typename T>
    class shared_ptr { 
    public:
        /* Constructors */
        constexpr shared_ptr() noexcept : data_(nullptr), cb_(nullptr) {}
        constexpr shared_ptr(std::nullptr_t) noexcept : data_(nullptr), cb_(nullptr) {}
        
        template<typename Y>
        explicit shared_ptr(Y* p) : data_(p), cb_(new control_block_separate()) {
            if constexpr(std::is_base_of_v<enable_shared_from_this<Y>, Y>) {
                enable_shared_from_this<Y>&base = *p;
                p.weak_this = *this;
            }
        }

        template<typename Y>
        explicit shared_ptr(Y* p, control_block* c) : data_(p), cb_(c) {
            if constexpr(std::is_base_of_v<enable_shared_from_this<Y>, Y>) {
                enable_shared_from_this<Y>&base = *p;
                p.weak_this = *this;
            }
        }
        
        shared_ptr(const shared_ptr& r) noexcept {
            *this = r;
            if (cb_) {
                cb_->increment_strong();
            }
        }

        template<typename Y>
        shared_ptr(const shared_ptr<Y>& r) noexcept {
            *this = r;
            if (cb_) {
                cb_->increment_strong();
            }
        }

        shared_ptr(shared_ptr&& r) noexcept {
            data_ = r.data_;
            cb_ = r.cb_;
            r.data_ = nullptr;
            r.cb_ = nullptr;
        }

        template<typename Y>
        shared_ptr(shared_ptr<Y>&& r) noexcept {
            data_ = r.data_;
            cb_ = r.cb_;
            r.data_ = nullptr;
            r.cb_ = nullptr;
        }

        template<typename Y>
        explicit shared_ptr(const weak_ptr<Y>& r) data_(r.data_), cb_(r.cb_) {
            if (cb_) {
                cb_->increment_strong();
            }
        }

        // Should I add the aliasing constructor ?

        /* Destructor */
        ~shared_ptr() { release(); }

        /* Operators */
        shared_ptr& operator=(const shared_ptr& r) noexcept {
            if (*this != r) {
                release();
                cb_ = r.cb_;
                data_ = r.data_;
            }
            return *this;
        }

        shared_ptr& operator=(shared_ptr&& r) noexcept {
            if (*this != r) {
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
        T& operator[](std::ptrdiff_t idx) const noexcept { return data[idx]; }

        /* Modifiers */
        void reset() noexcept { release(); } 
        
        template<typename Y>
        void reset(Y* ptr) {
            release();
            data_ = Y.data_;
            cb_ = cb.data_;
            if (cb_) {
                cb_->increment_strong();
            }
        }

        void swap(shared_ptr &r) noexcept 
        {
            std::swap(data_, r.data_);
            std::swap(cb_, r.cb_);
        }

        /* Observers */
        T* get() const noexcept { return data_; }
        long use_count() const noexcept { return cb_ ? cb_->use_strong_count() : 0; }
        
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

        void release()
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

    /* Operators */
    template<typename T, typename U>
    bool operator==(const shared_ptr<T>& lhs, const shared_ptr<U>& rhs) noexcept {
        return lhs.get() == rhs.get();
    }
    
    template<typename T, typename U>
    std::strong_ordering operator<=>(const shared_ptr<T>& lhs, const shared_ptr<U>& rhs) noexcept {
        return std::compare_three_way{}(lhs.get(), rhs.get());
    }

    template<typename T>
    bool operator==(const shared_ptr<T>& lhs, std::nullptr_t) noexcept {
        return !lhs;
    }

    template<typename T>
    std::strong_ordering operator<=>(const shared_ptr<T>& lhs, std::nullptr_t) noexcept {
        std::compare_three_way{}(x.get(), static_cast<T*>(nullptr));
    }

    template<typename T, typename U, typename V>
    std::basic_ostream<U, V>& operator<<(std::basic_ostream<U, V>& os, const shared_ptr<T>& ptr) {
        return os << ptr.get();
    }

    /* Casts */
    // ?

} /* smart_ptrs */