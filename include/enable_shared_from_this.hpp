#pragma once
#include "shared_ptr.hpp"
#include "weak_ptr.hpp"

namespace smart_ptrs {

    template<typename T>
    class enable_shared_from_this {
    protected:
        constexpr enable_shared_from_this() noexcept {} = default;
        enable_shared_from_this(const enable_shared_from_this& other) noexcept {} = default;
        ~enable_shared_from_this() {} = default;
        enable_shared_from_this& operator=(const enable_shared_from_this &rhs) noexcept {
            return *this; // std:: implementation does nothing
        }

    public:

        shared_ptr<T> shared_from_this() {
            return shared_ptr<T>(weak_this_);
        }

        shared_ptr<const T> shared_from_this() const {
            return shared_ptr<const T>(weak_this_);
        }

        weak_ptr<T> weak_from_this() noexcept {
            return weak_this_;
        }

        weak_ptr<T const> weak_from_this() const noexcept {
            return weak_this_;
        }

    private:
        mutable weak_ptr<T> weak_this_;
        friend class shared_ptr<T>;
    };

} /* smart_ptrs */

