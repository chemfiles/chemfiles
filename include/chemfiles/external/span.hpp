// Based on gsl-lite: https://github.com/martinmoene/gsl-lite
//
// Copyright(c) 2016 Guillaume Fraux
// Copyright(c) 2015 Martin Moene
// Copyright(c) 2015 Microsoft Corporation. All rights reserved.
//
// This code is licensed under the MIT License(MIT).
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

// clang-format off
#ifndef SPAN_HPP
#define SPAN_HPP

#include <iterator>
#include <utility>
#include <array>
#include <cstdint>
#include <cassert>
#include <cstddef>
#include <algorithm>
#include <type_traits>

namespace chemfiles {
enum class byte : std::uint8_t {};

// span<> - A 1D view of contiguous T's, replace(*,len).
template< class T >
class span {
    template< class U > friend class span;
public:
    typedef size_t size_type;
    typedef T value_type;
    typedef T & reference;
    typedef T * pointer;
    typedef T const * const_pointer;
    typedef T const & const_reference;
    typedef pointer       iterator;
    typedef const_pointer const_iterator;
    typedef std::reverse_iterator<iterator>       reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef typename std::iterator_traits< iterator >::difference_type difference_type;

    span(): begin_(nullptr), end_(nullptr) {
        assert(size() == 0);
    }

    span(std::nullptr_t, size_type size): begin_(nullptr), end_(nullptr) {
        assert(size == 0);
    }

    span(reference data): span(&data, 1) {}

    span(value_type &&) = delete;

    span(pointer begin, pointer end): begin_(begin), end_(end) {
        assert(begin <= end);
    }

    span(pointer data, size_type size): begin_(data), end_(data + size) {
        assert(size == 0 ||(size > 0 && data != nullptr));
    }

    template< class U, size_t N >
    span(U(&arr)[N]): begin_(arr), end_(arr + N) {}

    template< class U, size_t N >
    span(std::array< U, N > & arr): begin_(arr.data()), end_(arr.data() + N) {}

    // SFINAE enable only if Cont has a data() member function
    template< class Cont, typename = decltype(std::declval<Cont>().data()) >
    span(Cont & cont): begin_(cont.data()), end_(cont.data() + cont.size()) {}

    span(span &&) = default;
    span(span const &) = default;

    template<typename U>
    span(span<U> const & other): begin_(other.begin()), end_(other.end()) {}

    span & operator=(span &&) = default;
    span & operator=(span const &) = default;

    iterator begin() {
        return iterator(begin_);
    }

    iterator end() {
        return iterator(end_);
    }

    const_iterator begin() const {
        return iterator(begin_);
    }

    const_iterator end() const {
        return iterator(end_);
    }

    const_iterator cbegin() const {
        return const_iterator(begin());
    }

    const_iterator cend() const {
        return const_iterator(end());
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    const_reverse_iterator crbegin() const {
        return const_reverse_iterator(cend());
    }

    const_reverse_iterator crend() const {
        return const_reverse_iterator(cbegin());
    }

    operator bool() const noexcept {
        return begin_ != nullptr;
    }

    reference operator[](size_type index) {
        return at(index);
    }

    const_reference operator[](size_type index) const {
       return at(index);
    }

    bool operator==(span const & other) const noexcept {
        return size() == other.size()
            &&(begin_ == other.begin_ || std::equal(this->begin(), this->end(), other.begin()));
    }

    bool operator!=(span const & other) const noexcept {
        return !(*this == other);
    }

    bool operator<(span const & other) const noexcept {
        return std::lexicographical_compare(this->begin(), this->end(), other.begin(), other.end());
    }

    bool operator<=(span const & other) const noexcept {
        return !(other < *this);
    }

    bool operator>(span const & other) const noexcept {
        return(other < *this);
    }

    bool operator>=(span const & other) const noexcept {
        return !(*this < other);
    }

    reference at(size_type index) {
        assert(index < size());
        return begin_[index];
    }

    const_reference at(size_type index) const {
       assert(index < size());
       return begin_[index];
    }

    pointer data() noexcept {
        return begin_;
    }

    const_pointer data() const noexcept {
        return begin_;
    }

    bool empty() const noexcept {
        return size() == 0;
    }

    size_type size() const noexcept {
        return static_cast<size_type>(std::distance(begin_, end_));
    }

    size_type length() const noexcept {
        return size();
    }

    size_type used_length() const noexcept {
        return length();
    }

    size_type bytes() const noexcept {
        return sizeof(value_type) * size();
    }

    size_type used_bytes() const noexcept {
        return bytes();
    }

    void swap(span & other) noexcept {
        using std::swap;
        swap(begin_, other.begin_);
        swap(end_  , other.end_);
    }

    span< const byte > as_bytes() const noexcept {
        return span< const byte >(reinterpret_cast<const byte *>(data()), bytes());
    }

    span< byte > as_writeable_bytes() const noexcept {
        return span< byte >(reinterpret_cast<byte *>(data()), bytes());
    }

    template< typename U >
    span< U > as_span() const noexcept {
        assert((this->bytes() % sizeof(U)) == 0);
        return span< U >(reinterpret_cast<U *>(this->data()), this->bytes() / sizeof(U));
    }

private:
    // helpers for member as_span()
    template< typename U >
    span(U * & data, size_type size): begin_(data), end_(data + size) {
        assert(size == 0 ||(size > 0 && data != nullptr));
    }

    template< typename U >
    span(U * const & data, size_type size): begin_(data), end_(data + size) {
        assert(size == 0 ||(size > 0 && data != nullptr));
    }

private:
    pointer begin_;
    pointer end_;
};

// span creator functions
template< typename T >
span<T> as_span(T * begin, T * end) {
    return span<T>(begin, end);
}

template< typename T >
span<T> as_span(T * begin, size_t size) {
    return span<T>(begin, size);
}

template< typename T, size_t N >
span<T> as_span(T(&arr)[N]) {
    return span<T>(arr, N);
}

template< typename T, size_t N >
span<T> as_span(std::array<T,N> & arr) {
    return span<T>(arr);
}

template< class Cont >
span<typename Cont::value_type> as_span(Cont & cont) {
    return span<typename Cont::value_type>(cont);
}

} // namespace chemfiles

#endif
// clang-format on
