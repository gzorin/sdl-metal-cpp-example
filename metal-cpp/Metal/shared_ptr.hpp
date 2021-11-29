#pragma once

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

    shared_ptr(nullptr_t) : d_ptr(nullptr) {
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

    typename std::add_lvalue_reference<T>::type& operator*() const noexcept {
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