//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// Metal/shared_ptr.hpp
//
// Copyright 2021-2023 Alex Betts
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once

#include <functional>
#include <type_traits>

namespace MTL {

template<typename T> class shared_ptr;

template<typename T>
shared_ptr<T> make_owned(T *);

template<typename T>
class shared_ptr {
public:

    using element_type = std::remove_extent_t<T>;

    constexpr shared_ptr() noexcept : d_ptr(nullptr) {
    }

    template<class Y>
    explicit shared_ptr(Y *that) : d_ptr(that) {
        if (d_ptr) {
            d_ptr->retain();
        }
    }

    shared_ptr(const shared_ptr& other) noexcept : d_ptr(other.d_ptr) {
        if (d_ptr) {
            d_ptr->retain();
        }
    }

    template<class Y>
    shared_ptr(const shared_ptr<Y>& other) noexcept : d_ptr(other.d_ptr) {
        if (d_ptr) {
            d_ptr->retain();
        }
    }

    shared_ptr(shared_ptr&& other) noexcept : d_ptr(other.d_ptr) {
        other.d_ptr = nullptr;
    }

    template<class Y>
    shared_ptr(shared_ptr<Y>&& other) noexcept : d_ptr(other.d_ptr) {
        other.d_ptr = nullptr;
    }

    shared_ptr(std::nullptr_t) : d_ptr(nullptr) {
    }

    ~shared_ptr() {
        if (d_ptr) {
            d_ptr->release();
        }
    }

    shared_ptr& operator=(const shared_ptr& other) noexcept {
        shared_ptr(other).swap(*this);
        return *this;
    }

    template<class Y>
    shared_ptr& operator=(const shared_ptr<Y>& other) noexcept {
        shared_ptr(other).swap(*this);
        return *this;
    }

    shared_ptr& operator=(shared_ptr&& other) noexcept {
        shared_ptr(std::move(other)).swap(*this);
        return *this;
    }

    template<class Y>
    shared_ptr& operator=(shared_ptr<Y>&& other) noexcept {
        shared_ptr(std::move(other)).swap(*this);
        return *this;
    }

    void swap(shared_ptr& other) noexcept {
        std::swap(d_ptr, other.d_ptr);
    }

    void reset() noexcept {
        shared_ptr().swap(*this);
    }

    template<class Y>
    void reset(Y *other) {
        shared_ptr(other).swap(*this);
    }

    element_type *get() const noexcept {
        return d_ptr;
    }

    typename std::add_lvalue_reference<element_type>::type operator*() const noexcept {
        return *d_ptr;
    }

    element_type* operator->() const noexcept {
        return d_ptr;
    }

    long use_count() const noexcept {
        if (d_ptr) {
            return d_ptr->retainCount();
        }

        return 0;
    }

    bool unique() const noexcept {
        return use_count() == 1;
    }

    explicit operator bool() const noexcept {
        return get() != nullptr;
    }

    template<class U>
    bool owner_before(const shared_ptr<U>& other) const noexcept {
        return d_ptr < other.d_ptr;
    }

protected:

    element_type *d_ptr;

    template<typename Y> friend class shared_ptr;

    friend shared_ptr MTL::make_owned(T *);
};

} // End namespace MTL

// Construct a `shared_ptr` without increasing the object's retain count.
// Intended to be used with new/alloc/etc functions.
template<typename T>
MTL::shared_ptr<T>
MTL::make_owned(T *that) {
    shared_ptr<T> result;
    result.d_ptr = that;
    return result;
}

template<typename X, typename Y>
bool operator==(const MTL::shared_ptr<X>& lhs, const MTL::shared_ptr<Y>& rhs) {
    return lhs.get() == rhs.get();
}

template<typename X, typename Y>
bool operator!=(const MTL::shared_ptr<X>& lhs, const MTL::shared_ptr<Y>& rhs) {
    return !(lhs == rhs);
}

template<typename X, typename Y>
bool operator<(const MTL::shared_ptr<X>& lhs, const MTL::shared_ptr<Y>& rhs) {
    return std::less<>()(lhs.get(), rhs.get());
}

template<typename X, typename Y>
bool operator>(const MTL::shared_ptr<X>& lhs, const MTL::shared_ptr<Y>& rhs) {
    return rhs < lhs;
}

template<typename X, typename Y>
bool operator<=(const MTL::shared_ptr<X>& lhs, const MTL::shared_ptr<Y>& rhs) {
    return !(rhs < lhs);
}

template<typename X, typename Y>
bool operator>=(const MTL::shared_ptr<X>& lhs, const MTL::shared_ptr<Y>& rhs) {
    return !(lhs < rhs);
}

template<typename X>
bool operator==(const MTL::shared_ptr<X>& lhs, std::nullptr_t) {
    return !lhs;
}

template<typename X>
bool operator==(std::nullptr_t, const MTL::shared_ptr<X>& rhs) {
    return !rhs;
}

template<typename X>
bool operator!=(const MTL::shared_ptr<X>& lhs, std::nullptr_t) {
    return static_cast<bool>(lhs);
}

template<typename X>
bool operator!=(std::nullptr_t, const MTL::shared_ptr<X>& rhs) {
    return static_cast<bool>(rhs);
}

template<typename X>
bool operator<(const MTL::shared_ptr<X>& lhs, std::nullptr_t) {
    return std::less(lhs.get(), nullptr);
}

template<typename X>
bool operator<(std::nullptr_t, const MTL::shared_ptr<X>& rhs) {
    return std::less(nullptr, rhs.get());
}

template<typename X>
bool operator>(const MTL::shared_ptr<X>& lhs, std::nullptr_t) {
    return nullptr < lhs;
}

template<typename X>
bool operator>(std::nullptr_t, const MTL::shared_ptr<X>& rhs) {
    return rhs < nullptr;
}

template<typename X>
bool operator<=(const MTL::shared_ptr<X>& lhs, std::nullptr_t) {
    return !(nullptr < lhs);
}

template<typename X>
bool operator<=(std::nullptr_t, const MTL::shared_ptr<X>& rhs) {
    return !(rhs < nullptr);
}

template<typename X>
bool operator>=(const MTL::shared_ptr<X>& lhs, std::nullptr_t) {
    return !(lhs < nullptr);
}

template<typename X>
bool operator>=(std::nullptr_t, const MTL::shared_ptr<X>& rhs) {
    return !(nullptr < rhs);
}

namespace std {

template<typename T>
void
swap(MTL::shared_ptr<T>& lhs, MTL::shared_ptr<T>& rhs) {
    lhs.swap(rhs);
}

}