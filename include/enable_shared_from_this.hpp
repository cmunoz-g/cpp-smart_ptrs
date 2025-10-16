#pragma once
#include "shared_ptr.hpp"
#include "weak_ptr.hpp"

namespace smart_ptrs {

    template<typename T>
    class enable_shared_from_this {
    public:
        /* Member functions*/
        constexpr enable_shared_from_this() noexcept {

        }

        enable_shared_from_this(const enable_shared_from_this& other) noexcept {

        }

        ~enable_shared_from_this() {}

        enable_shared_from_this& operator=(const enable_shared_from_this &rhs) noexcept {
            return *this; // std:: implementation does nothing
        }

        shared_ptr<T> shared_from_this() {
            return shared_ptr(weak_this);
        }

        shared_ptr<T const> shared_from_this() const {
            
        }

    private:
        mutable weak_ptr<T> weak_this;

    };

} /* smart_ptrs */

