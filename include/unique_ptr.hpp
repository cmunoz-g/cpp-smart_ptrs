#pragma once

namespace smart_ptrs
{
    /* Single-object version */
    template<typename T>
    class unique_ptr {
    public:
    /* Constructors*/
    constexpr unique_ptr() noexcept : data_(nullptr) {}
    constexpr unique_ptr(std::nullptr_t) noexcept : data_(nullptr) {}
    explicit unique_ptr(T* p) noexcept : data_(p) {}

    unique_ptr(unique_ptr&& u) : data_(u.data_) {
        u.data_ = nullptr;
    }

    unique_ptr(const unique_ptr&) = delete;

    /* Destructor */
    ~unique_ptr() { delete_raw_pointer(); }

    /* Operators */
    unique_ptr& operator=(const unique_ptr&) = delete;

    unique_ptr& operator=(unique_ptr&& r) noexcept {
        if (*this != r) {
            delete_raw_pointer();
            // Some implementations use std::exchange for the 2 lines below
            this->data_ = r.data_;
            r.data_ = nullptr;
        }
        return *this;
    }

    unique_ptr& operator=(std::nullptr_t) noexcept {
        delete_raw_pointer();
        return *this;
    }

    explicit operator bool() const noexcept {
        return _data != nullptr;
    }

    T& operator*() const noexcept { return *data_; }
    T* operator->() const noexcept { return data_; }

    /* Modifiers */
    T* release() noexcept {
        T* p = get();
        data_ = nullptr;
        return p;
    }

    void reset(T* p = nullptr) noexcept {
        delete_raw_pointer();
        data_ = p;
    }

    void swap(unique_ptr& other) noexcept {
        std::swap(data_, other.data_);
    }

    /* Observers */
    T* get() const noexcept {
        return data_;
    }

    // get_deleter()

    private:
    T* data_;

    /* in case i want to add custom deleters, this is where it would call it*/
    void delete_raw_pointer() {
        if (data_) {
            delete data_;
            data_ = nullptr;
        }
    }
    };

    /* Array version */
    template<typename T> 
    class unique_ptr<T[]> {
    public:
        /* Constructors*/
        constexpr unique_ptr() noexcept : data_(nullptr) {}
        constexpr unique_ptr(std::nullptr_t) noexcept : data_(nullptr) {}
        explicit unique_ptr(T* p) noexcept : data_(p) {}

        unique_ptr(unique_ptr&& u) : data_(u.data_) {
            u.data_ = nullptr;
        }

        unique_ptr(const unique_ptr&) = delete;

        /* Destructor */
        ~unique_ptr() { delete_raw_pointer(); }

        /* Operators */
        unique_ptr& operator=(const unique_ptr&) = delete;

        unique_ptr& operator=(unique_ptr&& r) noexcept {
            if (*this != r) {
                delete_raw_pointer();
                // Some implementations use std::exchange for the 2 lines below
                this->data_ = r.data_;
                r.data_ = nullptr;
            }
            return *this;
        }

        unique_ptr& operator=(std::nullptr_t) noexcept {
            delete_raw_pointer();
            return *this;
        }

        explicit operator bool() const noexcept {
            return _data != nullptr;
        }

        T& operator[](std::size_t i) const {
            return data_[i];
        }

        /* Modifiers */
        T* release() noexcept {
            T* p = get();
            data_ = nullptr;
            return p;
        }

        void reset(T* p = nullptr) noexcept {
            delete_raw_pointer();
            data_ = p;
        }

        void swap(unique_ptr& other) noexcept {
            std::swap(data_, other.data_);
        }

        /* Observers */
        T* get() const noexcept {
            return data_;
        }
        // get_deleter()

    private:
        T* data_;

        void delete_raw_pointer() {
            if (data_) {
                delete[] data_;
                data_ = nullptr;
            }
        }
    };

    // make_unique
    // operator overloads
    
} /* smart_ptrs */
