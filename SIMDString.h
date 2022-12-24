#pragma once
/*
MIT License

Copyright (c) 2022 Morgan McGuire and Zander Majercik

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#define USE_SSE_MEMCPY 1

#include <string>
#include <stdint.h>
#include <assert.h>
#include <algorithm>
#include <cstring>
#include <iterator>
#include <cstddef>
#include <limits>
#include <ios>
#include <iostream>
#include <string_view>
#include <initializer_list>

#if defined(USE_SSE_MEMCPY) && USE_SSE_MEMCPY
#   if (defined(__arm__) || defined(__arm64__)) 
#       if !defined(__ARM_NEON) || !defined(__ARM_NEON__)
#           error "You must enable NEON instructions (e.g. -mfpu=neon) to use SIMDString with SIMD optimizations."
#       else
//          From https://github.com/DLTcollab/sse2neon/blob/master/sse2neon.h
#           include <arm_neon.h>
#           if (!defined(__aarch64__) && (__ARM_ARCH == 8)) && defined(__has_include) && __has_include(<arm_acle.h>)
#              include <arm_acle.h>
#           endif
#       endif
        typedef int64x2_t __m128i;
#       if ! defined(NO_G3D_ALLOCATOR) || (NO_G3D_ALLOCATOR == 0)
            // SIMDString only uses _m128i, but G3D requries that __m128 also be defined
            // for that reason, this typedef has to be before G3D is included
            typedef float32x4_t __m128;
#       endif
#   else
#        include <smmintrin.h>
#   endif
#endif


#if ! defined(NO_G3D_ALLOCATOR) || (NO_G3D_ALLOCATOR == 0)
#   include <G3D-base/System.h>
#endif

#ifdef G3D_System_h
#define TEMPLATE template<size_t INTERNAL_SIZE = 64, class Allocator = G3D::g3d_allocator<char>>
#else
#define TEMPLATE template<size_t INTERNAL_SIZE = 64, class Allocator = ::std::allocator<char>>
#endif

bool inConstSegment(const char* c);

constexpr size_t SSO_ALIGNMENT = 16;

/**
   \brief Very fast string class that follows the std::string/std::basic_string interface.

   - Recognizes constant segment strings and avoids copying them
   - Stores small strings internally to avoid heap allocation
   - Uses SSE instructions to copy internal strings
   - Uses the G3D free-list/block allocator when heap allocation is required

   INTERNAL_SIZE is in bytes. It should be chosen to be a multiple of 16.
*/
TEMPLATE
class
    /** This inline storage is used when strings are small */
#   ifdef _MSC_VER
    __declspec(align(SSO_ALIGNMENT))
#   else
    alignas(SSO_ALIGNMENT)
#   endif
    SIMDString {
public:

    typedef char                                    value_type;
    typedef std::char_traits<value_type>            traits_type;
    typedef value_type&                             reference;
    typedef const value_type&                       const_reference;
    typedef value_type*                             pointer;
    typedef const value_type*                       const_pointer;
    typedef ptrdiff_t                               difference_type;
    typedef size_t                                  size_type;
    typedef const_pointer                           const_iterator;
    typedef pointer                                 iterator;
    typedef std::reverse_iterator<const_pointer>    const_reverse_iterator;
    typedef std::reverse_iterator<pointer>          reverse_iterator;

protected:
    // Throw compile time error if INTERNAL_SIZE is not a multiple of SSO_ALIGNMENT
    static_assert(INTERNAL_SIZE % SSO_ALIGNMENT == 0, "SIMDString Internal Size must be a multiple of 16");

    // This is intentionally char, as it is bytes
    char            m_buffer[INTERNAL_SIZE];

    /** Includes \0 termination */
    value_type* m_data;

    /** Bytes to but not including '\0' */
    size_type   m_length;

    /** Total size of m_data including '\0', or 0 if m_data is in a const segment. This is actually
        implemented in the m_hider.data property in practice. */
        // size_t          m_allocated;

        // Uses the empty-base optimization trick from the standard library: http://www.cantrip.org/emptyopt.html,
        // which unfortunately requires slightly obfuscating the code by sticking the
        // m_allocated member into the data property, which we try to minimize the visibility
        // of using the m_allocated macro.

#define m_allocated m_hider.data
#define m_allocator m_hider
    mutable struct _AllocHider : public Allocator {
        _AllocHider() { }
        _AllocHider(size_t data) : data(data) { }
        size_t      data;
    } m_hider;

    inline static void memcpy(void* dst, const void* src, size_t count) {
        ::memcpy(dst, src, count);
    }

    /** Requires 128-bit alignment */
    inline static void swapBuffer(void* buf1, void* buf2) {
#       if USE_SSE_MEMCPY
            // Can assume that INTERNAL_SIZE % SSO_ALIGNMENT == 0 because of the static assertion on line 115
            __m128i* d = reinterpret_cast<__m128i*>(buf1);
            __m128i* s = reinterpret_cast<__m128i*>(buf2);

            for (int i = 0; i < INTERNAL_SIZE/SSO_ALIGNMENT; ++i) {
                __m128i tmp = d[i];
                d[i] = s[i];
                s[i] = tmp;
            }
#       else
            std::swap(buf1, buf2);
#       endif
    }

    /** Requires 128-bit alignment */
    inline static void memcpyBuffer(void* dst, const void* src, size_t count = INTERNAL_SIZE) {
#       if USE_SSE_MEMCPY
            // Can assume that INTERNAL_SIZE % SSO_ALIGNMENT == 0 because of the static assertion on line 115
            __m128i* d = reinterpret_cast<__m128i*>(dst);
            const __m128i* s = reinterpret_cast<const __m128i*>(src);
            int iterations = (int) (count/SSO_ALIGNMENT);
            for (int i = 0; i < iterations; ++i) {
                d[i] = s[i];
            }
#       else
            ::memcpy(dst, src, count);
#       endif
    }

    constexpr inline void* alloc(size_t b) {
        if (b <= INTERNAL_SIZE) {
            return m_buffer;
        }
        else {
            return m_allocator.allocate(b);
        }
    }

    constexpr void free(void* p, size_t oldSize) const {
        if (p != m_buffer) {
            return m_allocator.deallocate(static_cast<value_type*>(p), oldSize);
        }
    }

    constexpr bool inConst() const {
        return !m_allocated && m_data;
    }

    /** Choose the number of bytes to allocate to hold a string of length L 
     *  Note: Calling functions are expected to +1 for the null terminator */
    constexpr inline static size_t chooseAllocationSize(size_t L) {
        // Avoid allocating more than internal size unless required, but always allocate at least the internal size
        return (L <= INTERNAL_SIZE) ? INTERNAL_SIZE : std::max((size_t)(2 * L + 1), (size_t)64);
    }

    constexpr void prepareToMutate() {
        if (inConst()) {
            const value_type* old = m_data;
            m_allocated = chooseAllocationSize(m_length + 1);
            m_data = (value_type*)alloc(m_allocated);
            memcpy(m_data, old, m_length + 1);
        }
    }

    /** Ensure enough bytes are allocated to hold a string of length newSize
     *  and copies the old string over
     *  Note: Calling functions are expected to +1 for the null terminator */
    constexpr void ensureAllocation(size_t newSize) {
        if ((m_allocated < newSize) && !((m_data == m_buffer) && (newSize < INTERNAL_SIZE))) {
            value_type* old = m_data;
            size_t oldSize = m_allocated;
            m_allocated = chooseAllocationSize(newSize);
            m_data = (value_type*)alloc(m_allocated);
            memcpy(m_data, old, m_length);
            if (!::inConstSegment(old)) { free(old, oldSize); }
        }
    }

    constexpr inline void maybeDeallocate() {
        if (m_allocated) {
            // Free previously allocated data
            free(m_data, m_allocated);
            m_data = nullptr;
            m_allocated = 0;
        }
    }

    /** Ensure enough bytes are allocated to hold a string of length newSize.
     *  Note: Calling functions are expected to +1 for the null terminator */
    constexpr inline void maybeReallocate(size_t newSize) {
        // Don't waste an allocation if the memory already allocated is large enough to hold the new data
        if (m_allocated && m_allocated >= newSize) {
            return;
        }

        // free the old data, if applicable
        if (m_allocated) {
            // Free previously allocated data
            free(m_data, m_allocated);
        }

        // allocate memory
        m_allocated = chooseAllocationSize(newSize);
        m_data = (value_type*)alloc(m_allocated);
    }

public:

    static const size_type npos = size_type(-1);

    /** Creates a zero-length string */
    constexpr inline SIMDString() : m_data(m_buffer), m_length(0), m_hider(INTERNAL_SIZE) {
        m_buffer[0] = '\0';
    }

    /** \param count Copy this many characters.  */
    constexpr SIMDString(size_type count, value_type c) : m_length(count) {
        // Allocate more than needed for fast append
        m_allocated = chooseAllocationSize(m_length + 1);
        m_data = static_cast<value_type*>(alloc(m_allocated));
        ::memset(m_data, c, m_length);
        m_data[m_length] = '\0';
    }

    constexpr inline SIMDString(const value_type c) : m_data(m_buffer), m_length(1), m_hider(INTERNAL_SIZE) {
        m_buffer[0] = c;
        m_buffer[1] = '\0';
    }

    constexpr SIMDString(const SIMDString& str, size_type pos = 0) {
        m_length = str.m_length - pos;
        if (str.inConst()) {
            // Share this const_seg value
            m_data = str.m_data + pos;
            m_allocated = 0;
        }
        else {
            m_allocated = str.m_allocated - pos;
            // Clone the value, putting it in the internal storage if possible
            m_data = (value_type*)alloc(m_allocated);

            // memcpyBuffer assumes SSE so this needs to be aligned to SSO_ALIGNMENT 
            // Since INTERNAL_SIZE is a multiple of 2, the compiler will optimize `% SSO_ALIGNMENT` to `& (SSO_ALIGNMENT - 1)`
            if ((m_data == m_buffer) && (str.m_data == str.m_buffer) && !(pos % SSO_ALIGNMENT)) {
                memcpyBuffer(m_data, str.m_data + pos, INTERNAL_SIZE - pos);
            }
            else {
                // + 1 is for the '\0'
                memcpy(m_data, str.m_data + pos, m_length + 1);
            }
        }
    }

    constexpr SIMDString(const SIMDString& str, size_type pos, size_type count) {
        // cannot point to const string 
        m_length = (count == npos || pos + count >= str.size()) ? str.size() - pos : count;
        m_allocated = m_length + 1;
        m_data = (value_type*)alloc(m_allocated);
        if ((m_data == m_buffer) && (str.m_data == str.m_buffer) && !(pos % SSO_ALIGNMENT)) {
            memcpyBuffer(m_data, str.m_data + pos, INTERNAL_SIZE - pos);
        }
        else {
            // + 1 is for the '\0'
            memcpy(m_data, str.m_data + pos, m_length + 1);
        }
        m_data[m_length] = '\0';
    }

    constexpr SIMDString(const value_type* s) : m_length(::strlen(s)) {
        if (::inConstSegment(s)) {
            m_data = const_cast<value_type*>(s);
            m_allocated = 0;
        }
        else {
            // Allocate more than needed for fast append
            m_allocated = chooseAllocationSize(m_length + 1);
            m_data = (value_type*)alloc(m_allocated);
            memcpy(m_data, s, m_length + 1);
        }
    }

    /** \param count Copy this many characters. The result is always copied because it is unsafe to
        check past the end of s for a null terminator.*/
    constexpr SIMDString(const value_type* s, size_type count) : m_length(count) {
        // Allocate more than needed for fast append
        m_allocated = chooseAllocationSize(m_length + 1);
        m_data = (value_type*)alloc(m_allocated);
        memcpy(m_data, s, m_length);
        m_data[m_length] = '\0';
    }

    constexpr SIMDString(SIMDString&& str) {
        swap(str);
    }

    // These aren't passed by reference because this was the signature on basic_string
    constexpr SIMDString(const iterator first, const iterator last) {
        m_length = last - first;
        m_allocated = m_length;

        // Copy the value, since it's unknown if it's a substring or not
        m_data = (value_type*)alloc(m_allocated);
        memcpy(m_data, first, m_length);
        m_data[m_length] = '\0';
    }

    // explicit to prevent auto casting
    explicit constexpr SIMDString(const std::string& str, size_type pos = 0, size_type count = npos) {
        m_length = (count == npos || pos + count >= str.size()) ? str.size() - pos : count;
        // Allocate more than needed for fast append
        m_allocated = chooseAllocationSize(m_length + 1);
        m_data = (value_type*)alloc(m_allocated);
        memcpy(m_data, str.c_str() + pos, m_length);
        m_data[m_length] = '\0';
    }

    constexpr SIMDString(std::initializer_list<value_type> ilist) : m_length(ilist.size()) {
        // The initializer list points to a list of const elements
        // They can't be moved and they're not null terminated
        m_allocated = chooseAllocationSize(m_length + 1);
        m_data = (value_type*)alloc(m_allocated);
        memcpy(m_data, ilist.begin(), m_length);
        m_data[m_length] = '\0';
    }

    explicit constexpr SIMDString(std::string_view& sv, size_type pos = 0) : m_length(sv.size() - pos) {
        if (::inConstSegment(sv.data())) {
            m_data = sv.data() + pos;
            m_allocated = 0;
        }
        else {
            // Allocate more than needed for fast append
            m_allocated = chooseAllocationSize(m_length + 1);
            m_data = (value_type*)alloc(m_allocated);
            memcpy(m_data, sv.data() + pos, m_length);
            m_data[m_length] = '\0';
        }
    }

    constexpr SIMDString(std::string_view& sv, size_type pos, size_type count) {
        m_length = (count == npos || pos + count >= sv.size()) ? sv.size() - pos : count;
        // Allocate more than needed for fast append
        m_allocated = chooseAllocationSize(m_length + 1);
        m_data = (value_type*)alloc(m_allocated);
        memcpy(m_data, sv.data() + pos, m_length);
        m_data[m_length] = '\0';
    }

    ~SIMDString() {
        if (m_data && m_allocated) {
            // Note that this calls the method, not ::free 
            free(m_data, m_allocated);
        }
    }

    constexpr SIMDString& operator=(const SIMDString& str) {
        if (&str == this) {
            return *this;
        }
        // Constant storage
        else if (str.inConst()) {
            maybeDeallocate();
            // Share this const_seg value
            m_data = str.m_data;
            m_length = str.m_length;
            m_allocated = str.m_allocated;

        }
        // Buffer and heap storage
        else {
            m_length = str.m_length;

            // free and/or allocate memory if necessary. 
            maybeReallocate(m_length + 1);

            // Clone the other value, putting it in the internal storage if possible
            if ((m_data == m_buffer) && (str.m_data == str.m_buffer)) {
                memcpyBuffer(m_data, str.m_data);
            }
            else {
                memcpy(m_data, str.m_data, m_length + 1);
            }
        }

        return *this;
    }

    constexpr SIMDString& operator=(SIMDString&& str) {
        swap(str);
        return *this;
    }

    constexpr SIMDString& operator=(const value_type* s) {
        m_length = ::strlen(s);

        if (::inConstSegment(s)) {
            maybeDeallocate();
            // Share this const_seg value
            m_data = const_cast<value_type*>(s);
            m_allocated = 0;
        }
        else {
            // free and/or allocate memory if necessary. 
            maybeReallocate(m_length + 1);
            // Clone the other value, putting it in the internal storage if possible
            memcpy(m_data, s, m_length + 1);
        }
        return *this;
    }

    constexpr SIMDString& operator=(const value_type c) {
        maybeDeallocate();
        m_length = 1;
        m_allocated = INTERNAL_SIZE;
        m_data = (value_type*)alloc(m_allocated);
        m_data[0] = c;
        m_data[1] = '\0';
        return *this;
    }

    constexpr SIMDString& operator=(std::initializer_list<value_type> ilist) {
        m_length = ilist.size();

        // free and/or allocate memory if necessary. 
        maybeReallocate(m_length + 1);
        
        // Clone the other value, putting it in the internal storage if possible
        memcpy(m_data, ilist.begin(), m_length);
        m_data[m_length] = '\0';
        return *this;
    }

    constexpr SIMDString& operator=(const std::string_view& sv) {
        m_length = sv.size();

        // free and/or allocate memory if necessary. 
        maybeReallocate(m_length + 1);

        // have to copy over because string_view doesn't use null terminators
        memcpy(m_data, sv.data(), m_length);
        m_data[m_length] = '\0';
        return *this;
    }

    constexpr SIMDString& assign(const SIMDString& str, size_type pos = 0, size_type count = npos) {
        size_type copy_len = (count == npos || pos + count >= str.size()) ? str.size() - pos : count;
        if (pos == 0 && copy_len == str.size()) {
            return (*this) = str;
        }
        // Constant storage and can use the same null terminator
        if (str.inConst() && pos + copy_len == str.size()) {
            maybeDeallocate();
            // Share this const_seg value
            m_data = str.m_data + pos;
            m_length = copy_len;
            m_allocated = str.m_allocated;
        }
        // Buffer and heap storage or a substring
        else {
            m_length = copy_len;
            // free and/or allocate memory if necessary. 
            maybeReallocate(m_length + 1);

            // Clone the other value, putting it in the internal storage if possible
            // memcpyBuffer assumes SSE this needs be aligned to SSO_ALIGNMENT 
            // Since INTERNAL_SIZE is a multiple of 2, the compiler will optimize `% SSO_ALIGNMENT` to `& (SSO_ALIGNMENT - 1)`
            if ((m_data == m_buffer) && (str.m_data == str.m_buffer) && !(pos % SSO_ALIGNMENT)) {
                // can copy over entire buffer because the string gets null terminated anyway
                memcpyBuffer(m_data, str.m_data + pos, INTERNAL_SIZE - pos);
            }
            else {
                memcpy(m_data, str.m_data + pos, m_length);
            }
            m_data[m_length] = '\0';
        }
        return *this;
    }

    constexpr SIMDString& assign(const value_type* s, size_type count) {
        m_length = count;
        // free and/or allocate memory if necessary. 
        maybeReallocate(m_length + 1);

        // Clone the other value, putting it in the internal storage if possible
        memcpy(m_data, s, m_length);
        m_data[m_length] = '\0';
        return *this;
    }

    constexpr SIMDString& assign(const value_type* s) {
        return (*this = s);
    }

    constexpr SIMDString& assign(size_type count, const value_type c) {
        m_length = count;
        // free and/or allocate memory if necessary. 
        maybeReallocate(m_length + 1);

        // Clone the other value, putting it in the internal storage if possible
        ::memset(m_data, c, m_length);
        m_data[m_length] = '\0';
        return *this;
    }

    constexpr SIMDString& assign(const SIMDString&& str) {
        return (*this) = str;
    }

    constexpr SIMDString& assign(const_iterator first, const_iterator last) {
        m_length = last - first;
        // free and/or allocate memory if necessary. 
        maybeReallocate(m_length + 1);

        // Clone the other value, putting it in the internal storage if possible
        memcpy(m_data, first, m_length);
        m_data[m_length] = '\0';
        return *this;
    }

    constexpr SIMDString& assign(std::initializer_list<value_type> ilist) {
        return (*this) = ilist;
    }

    constexpr SIMDString& assign(std::string_view& sv) {
        return (*this) = sv;
    }

    constexpr SIMDString& assign(std::string_view& sv, size_type pos, size_type count) {
        if (pos == 0 && count == sv.size()) {
            return (*this = sv);
        }
        m_length = (count == npos || pos + count >= sv.size()) ? sv.size() - pos : count;
        
        // free and/or allocate memory if necessary. 
        maybeReallocate(m_length + 1);
        
        // have to copy over because string_view doesn't use null terminators
        memcpy(m_data, sv.data(), m_length);
        m_data[m_length] = '\0';
        return *this;
    }


    constexpr Allocator get_allocator() {
        return m_allocator;
    }

    // access
    constexpr const value_type* c_str() const {
        return m_data;
    }

    constexpr const value_type* data() const {
        return m_data;
    }

    constexpr const_reference operator[](size_type x) const {
        assert(x < m_length&& x >= 0); // "Index out of bounds");
        return m_data[x];
    }

    constexpr reference operator[](size_type x) {
        assert(x < m_length&& x >= 0); // "Index out of bounds");
        prepareToMutate();
        return m_data[x];
    }

    constexpr const_reference at(size_type x) const {
        assert(x < m_length&& x >= 0); // "Index out of bounds");
        return m_data[x];
    }

    constexpr reference at(size_type x) {
        assert(x < m_length&& x >= 0); // "Index out of bounds");
        prepareToMutate();
        return m_data[x];
    }

    constexpr const_reference front() const {
        assert(m_data); // "Empty string"
        return m_data[0];
    }

    constexpr reference front() {
        assert(m_data); // "Empty string"
        prepareToMutate();
        return m_data[0];
    }

    constexpr const_reference back() const {
        assert(m_data); // "Empty string"
        return m_data[m_length - 1];
    }

    constexpr reference back() {
        assert(m_data); // "Empty string"
        prepareToMutate();
        return m_data[m_length - 1];
    }

    explicit constexpr inline operator std::basic_string_view<value_type>() {
        return std::string_view(m_data, m_length);
    }

    // iterators
    constexpr iterator begin() {
        prepareToMutate();
        return m_data;
    }

    constexpr iterator end() {
        prepareToMutate();
        return m_data ? m_data + m_length : nullptr;
    }

    constexpr const_iterator cbegin() const {
        return const_iterator(m_data);
    }

    constexpr const_iterator cend() const {
        return m_data ? m_data + m_length : nullptr;
    }

    // std::reverse_iterator in cpp2017 or older don't have constexpr constructors
    // thus these functions can't be constexpr for now
    reverse_iterator rbegin() {
        prepareToMutate();
        return reverse_iterator(m_data + m_length);
    }

    reverse_iterator rend() {
        prepareToMutate();
        return m_data ? reverse_iterator(m_data) : reverse_iterator();
    }

    const_reverse_iterator crbegin() const
    {
        return const_reverse_iterator(m_data + m_length);
    }

    const_reverse_iterator crend() const {
        return m_data ? const_reverse_iterator(m_data) : const_reverse_iterator();
    }

    constexpr size_type size() const {
        return m_length;
    }

    constexpr size_type length() const {
        return m_length;
    }

    constexpr size_type capacity() const {
        if (m_data == m_buffer) {
            return INTERNAL_SIZE;
        }
        else if (inConst()) {
            return m_length;
        }
        else {
            return m_allocated; // 0 when isConst() is true
        }
    }

    constexpr size_type max_size() const {
        return size_type(-1);
    }

    constexpr bool empty() const {
        return (m_length == 0);
    }

    constexpr void reserve(size_type newLength) {
        if (newLength + 1 > m_allocated) {
            // Reserve more space
            if (newLength + 1 > INTERNAL_SIZE) {
                // Need heap allocation
                value_type* old = m_data;
                // Allocate the exact size required
                m_data = (value_type*)alloc(newLength + 1);
                size_type oldSize = m_allocated;
                m_allocated = newLength + 1;
                memcpy(m_data, old, m_length + 1);

                // Maybe free the old buffer, if it was not m_buffer
                free(old, oldSize);
            }
            else if (m_data != m_buffer) {
                // Must be in a const segment, because small and not in the buffer. Just copy to the internal buffer.
                memcpy(m_buffer, m_data, m_length + 1);
                m_allocated = INTERNAL_SIZE;
                m_data = m_buffer;
            }
            else {
                // Should already have been in the internal buffer and fitting
                assert(false); // "Should not reach this case if the new length is less than the internal buffer"
            }
        }
    }

    constexpr void reserve() {
        shrink_to_fit();
    }

    constexpr void shrink_to_fit() {
        if (m_allocated != m_length + 1) {
            value_type* old = m_data;
            size_type oldSize = m_allocated;
            m_allocated = chooseAllocationSize(m_length + 1);
            m_data = (value_type*)alloc(m_allocated);
            memcpy(m_data, old, m_length);
            if (!::inConstSegment(old)) { free(old, oldSize); }
        }
    }

    constexpr SIMDString& insert(size_type pos, const SIMDString& str, size_type pos2, size_type count = npos) {
        if (pos == m_length) {
            return append(str, pos2, count);
        }
        return replace(pos, 0, str, pos2, count);
    }

    constexpr SIMDString& insert(size_type pos, const SIMDString& str) {
        if (pos == m_length) {
            return append(str);
        }
        return replace(pos, 0, str);
    }

    constexpr SIMDString& insert(size_type pos, const value_type* s) {
        if (pos == m_length) {
            return append(s);
        }
        return replace(pos, 0, s, ::strlen(s));
    }

    constexpr SIMDString& insert(size_type pos, const value_type* s, size_type count) {
        if (pos == m_length) {
            return append(s, count);
        }
        return replace(pos, 0, s, count);
    }

    constexpr SIMDString& insert(size_type pos, size_type count, value_type c) {
        if (pos == m_length) {
            return append(count, c);
        }
        return replace(pos, 0, count, c);
    }

    constexpr iterator insert(const_iterator pos, value_type c) {
        if (pos == end()) {
            return append(c);
        }
        replace(pos - m_data, 0, 1, c);
        return m_data + pos;
    }

    constexpr SIMDString& insert(const_iterator pos, std::initializer_list<value_type> ilist) {
        if (pos == end()) {
            return append(ilist.begin());
        }
        return replace(pos - m_data, 0, ilist.begin(), ilist.size());
    }

    constexpr SIMDString& insert(size_type pos, std::string_view& sv) {
        if (pos == end()) {
            return append(sv.begin());
        }
        return replace(pos, 0, sv.begin(), sv.size());

    }

    constexpr SIMDString& insert(size_type pos, std::string_view& sv, size_type pos2, size_type count) {
        if (pos == end()) {
            return append(sv.begin() + pos2, count);
        }
        return replace(pos, 0, sv.begin() + pos2, count);
    }

    constexpr void resize(size_type count, value_type c = 0) {
        if (count < m_length) {
            m_data[m_length = count] = '\0';
        }
        else if (count > m_length) {
            append(count - m_length, c);
        }
    }

    constexpr size_type copy(pointer dest, size_type count, size_type pos = 0) {
        size_type cpyCount = pos + count > m_length ? m_length - pos : count;

        // the resulting string of copy is not null terminated
        memcpy(dest, m_data + pos, cpyCount);
        return cpyCount;
    }

    constexpr SIMDString& replace(size_type pos, size_type count, const SIMDString& str) {
        if (pos == m_length) {
            return append(str.m_data, str.m_length);
        }
        return replace(pos, count, str.m_data, str.m_length);
    }

    constexpr SIMDString& replace(const_iterator first, const_iterator last, const SIMDString& str) {
        if (last == end()) {
            return append(str.m_data, str.m_length);
        }
        return replace(first - m_data, last - first, str.m_data, str.m_length);
    }

    constexpr SIMDString& replace(size_type pos, size_type count, const SIMDString& str, size_type pos2, size_type count2 = npos) {
        return replace(pos, count, str.m_data + pos2, count2);
    }

    constexpr SIMDString& replace(size_type pos, size_type count, const value_type* s, size_type count2) {
        assert(pos <= m_length && max_size() >= m_length + std::abs((int)(count - count2))); // "Index out of bounds");
        if (pos == m_length) {
            return append(s, count2);
        }

        long sizeDiff = (long) (count2 - count);

        if (sizeDiff > 0) { // count < count2 -> insert
            size_type newSize = m_length + sizeDiff + 1;
            if (((m_allocated < newSize) && !((m_data == m_buffer) && (newSize <= INTERNAL_SIZE))) || inConst()) {
                // Allocate a new string and copy over first n values
                value_type* old = m_data;
                size_type oldSize = m_allocated;
                m_allocated = chooseAllocationSize(newSize);
                m_data = (value_type*)alloc(m_allocated);
                // copy [old, old + pos) to [m_data, m_data + pos)
                memcpy(m_data, old, pos);
                // copy [old + pos + count, old + m_length) to [m_data + pos + count2, m_data + newSize)
                memcpy(m_data + pos + count2, old + pos + count, m_length - pos - count + 1);
                if (!::inConstSegment(old)) { free(old, oldSize); }
            }
            else {
                // move [m_data + pos + count, m_data + m_length) to [m_data + pos + count2, m_data + newSize)
                memmove(m_data + pos + count2, m_data + pos + count, m_length - pos - count + 1);
            }
            memcpy(m_data + pos, s, count2);
            m_length += sizeDiff;
        }
        else if (sizeDiff < 0) { // count > count2 
            prepareToMutate();
            memcpy(m_data + pos, s, count2);
            memmove(m_data + pos + count2, m_data + pos + count, m_length - pos - count + 1);
            m_length += sizeDiff;
        }
        else {
            prepareToMutate();
            memcpy(m_data + pos, s, count2);
        }
        return (*this);
    }

    constexpr SIMDString &replace(const_iterator first, const_iterator last, const value_type *s, size_type count2) {
        if (last == end()) {
            return append(s, count2);
        }
        return replace(first - m_data, last - first, s, count2);
    }

    constexpr SIMDString &replace(const_iterator first, const_iterator last, iterator first2, iterator last2) {
        if (last == end()) {
            return append(first2, last2 - first2);
        }
        return replace(first - m_data, last - first, first2, last2 - first2);
    }

    constexpr SIMDString &replace(size_type pos, size_type count, const value_type *s) {
        if (pos == m_length) {
            return append(s, ::strlen(s));
        }
        return replace(pos, count, s, ::strlen(s));
    }

    constexpr SIMDString &replace(const_iterator first, const_iterator last, const value_type *s) {
        if (last == end()) {
            return append(s, ::strlen(s));
        }
        return replace(first - m_data, last - first, s, ::strlen(s));
    }

    constexpr SIMDString& replace(size_type pos, size_type count, size_type count2, value_type c) {
        assert(pos <= m_length && max_size() >= m_length + std::abs((int)(count - count2))); // "Index out of bounds");
        if (pos == m_length) {
            return append(count2, c);
        }

        long sizeDiff = (long) (count2 - count);

        // count < count2
        if (sizeDiff > 0) { 
            size_type newSize = m_length + sizeDiff + 1;
            if (((m_allocated < newSize + 1) && !((m_data == m_buffer) && (newSize < INTERNAL_SIZE))) || inConst()) {
                // Allocate a new string and copy over first n values
                value_type* old = m_data;
                size_type oldSize = m_allocated;
                m_allocated = chooseAllocationSize(newSize);
                m_data = (value_type*)alloc(m_allocated);
                // copy [old, old + pos) to [m_data, m_data + pos)
                memcpy(m_data, old, pos);
                // copy [old + pos + count, old + m_length) to [m_data + pos + count2, m_data + newSize)
                memcpy(m_data + pos + count2, old + pos + count, m_length - pos - count + 1);
                if (!::inConstSegment(old)) { free(old, oldSize); }
            }
            else {
                // move [m_data + pos + count, m_data + m_length) to [m_data + pos + count2, m_data + newSize)
                memmove(m_data + pos + count2, m_data + pos + count, m_length - pos - count + 1);
            }
            ::memset(m_data + pos, c, count2);
            m_length += sizeDiff;
        }
        // count > count2 
        else if (sizeDiff < 0) {
            prepareToMutate();
            ::memset(m_data + pos, c, count2);
            memmove(m_data + pos + count2, m_data + pos + count, m_length - pos - count + 1);
            m_length += sizeDiff;
        }
        else {
            prepareToMutate();
            ::memset(m_data + pos, c, count2);
        }
        return (*this);
    }

    constexpr SIMDString& replace(const_iterator first, const_iterator last, size_type count2, value_type c) {
        if (last == end()) {
            return append(count2, c);
        }
        return replace(first - m_data, last - first, count2, c);
    }

    constexpr SIMDString& replace(const_iterator first, const_iterator last, std::initializer_list<value_type> ilist) {
        if (last == end()) {
            return append(ilist.begin(), ilist.size());
        }
        return replace(first - m_data, first - last, ilist.begin(), ilist.size());
    }

    constexpr SIMDString& replace(const_iterator first, const_iterator last, std::string_view& sv) {
        if (last == end()) {
            return append(sv.begin(), sv.size());
        }
        return replace(first - m_data, first - last, sv.begin(), sv.size());
    }

    constexpr SIMDString& replace(size_type pos, size_type count, std::string_view& sv) {
        if (pos == m_length) {
            return append(sv.begin(), sv.size());
        }
        return replace(pos, count, sv.data(), sv.size());
    }

    constexpr SIMDString& replace(size_type pos, size_type count, std::string_view& sv, size_type pos2, size_type count2) {
        if (pos == m_length) {
            return append(sv.begin() + pos2, count2);
        }
        return replace(pos, count, sv.begin() + pos2, count2);
    }

    constexpr void clear() {
        *this = SIMDString();
    }

    constexpr SIMDString& erase(size_type pos = 0, size_type count = npos) {
        if ((count == npos) || (pos + count > m_length)) {
            count = m_length - pos;
        }

        if ((pos == 0) && (count == m_length)) {
            // Optimize erasing the entire string
            clear();
        }
        else if (count > 0) {
            if (inConst()) {
                const value_type* old = m_data;
                m_allocated = chooseAllocationSize(m_length - count + 1);
                m_data = (value_type*)alloc(m_allocated);
                // copy over [old, old + pos)
                memcpy(m_data, old, pos);
                // copy over [old + pos + count, old + m_length] <- includes 0
                memcpy(m_data + pos, old + pos + count, m_length - (pos + count) + 1);
            }
            else {
                // move [old + pos + count, old + m_length] up by count
                memmove(m_data + pos, m_data + pos + count, m_length - (pos + count) + 1);
            }
            m_length -= count;
        }

        return *this;
    }

    constexpr iterator erase(const_iterator pos) {
        size_type n = pos - m_data;
        erase(n, 1);
        return m_data + n;
    }

    constexpr iterator erase(const_iterator first, const_iterator last) {
        size_type n = first - m_data;
        if (n == m_length) {
            clear();
        }
        else {
            erase(n, last - first);
        }
        return m_data + n;
    }

    constexpr SIMDString operator+(const SIMDString& str) const {
        SIMDString result;
        result.m_length = m_length + str.m_length;
        result.m_allocated = chooseAllocationSize(result.m_length + 1);
        result.m_data = (value_type*)result.alloc(result.m_allocated);
        
        if ((result.m_data == result.m_buffer) && (m_data == m_buffer)) {
            memcpyBuffer(result.m_data, m_data);
        }
        else {
            memcpy(result.m_data, m_data, m_length);
        }

        memcpy(result.m_data + m_length, str.m_data, str.m_length + 1);
        return result;
    }

    constexpr SIMDString operator+(const value_type* s) const {
        const size_type L(::strlen(s));
        SIMDString result;
        result.m_length = m_length + L;
        result.m_allocated = chooseAllocationSize(result.m_length + 1);
        result.m_data = (value_type*)result.alloc(result.m_allocated);

        // Copy this to output string
        if ((result.m_data == result.m_buffer) && (m_data == m_buffer)) {
            memcpyBuffer(result.m_data, m_data);
        }
        else {
            memcpy(result.m_data, m_data, m_length);
        }

        // Copy s to output string
        memcpy(result.m_data + m_length, s, L + 1);
        return result;
    }

    constexpr SIMDString operator+(const value_type c) const {
        SIMDString result;
        result.m_length = m_length + 1;
        result.m_allocated = chooseAllocationSize(result.m_length + 1);
        result.m_data = (value_type*)result.alloc(result.m_allocated);

        if ((result.m_data == result.m_buffer) && (m_data == m_buffer)) {
            memcpyBuffer(result.m_data, m_data);
        }
        else {
            memcpy(result.m_data, m_data, m_length);
        }
        result.m_data[result.m_length - 1] = c;
        result.m_data[result.m_length] = '\0';
        return result;
    }

    constexpr SIMDString& operator+=(const SIMDString& str) {
        ensureAllocation(m_length + str.m_length + 1);
        memcpy(m_data + m_length, str.m_data, str.m_length + 1);
        m_length += str.m_length;
        return *this;
    }

    constexpr SIMDString& operator+=(const value_type c) {
        // +1 for c, +1 for null operator
        ensureAllocation(m_length + 2);
        m_data[m_length] = c;
        m_data[++m_length] = '\0';
        return *this;
    }

    constexpr SIMDString& operator+=(const value_type* s) {
        const size_type t = ::strlen(s);
        ensureAllocation(m_length + t + 1);
        memcpy(m_data + m_length, s, t + 1);
        m_length += t;
        return *this;
    }

    constexpr SIMDString& operator+=(std::initializer_list<value_type> ilist) {
        return this->append(ilist.begin(), ilist.size());
    }

    constexpr SIMDString& operator+=(std::string_view sv) {
        return this->append(sv.begin(), sv.size());
    }

    constexpr void push_back(value_type c) {
        *this += c;
    }

    constexpr void pop_back() {
        m_data[--m_length] = '\0';
    }

    constexpr SIMDString& append(const SIMDString& str, size_type pos, size_type count = npos) {
        size_type copy_len = (count == npos || pos + count >= str.size()) ? str.size() - pos : count;
        ensureAllocation(m_length + copy_len + 1);
        memcpy(m_data + m_length, str.m_data + pos, copy_len);
        m_data[m_length += copy_len] = '\0';
        return *this;
    }

    constexpr SIMDString& append(const SIMDString& str) {
        return (*this += str);
    }

    constexpr SIMDString& append(size_type count, value_type c) {
        ensureAllocation(m_length + count + 1);
        ::memset(m_data + m_length, c, count);
        m_length += count;
        m_data[m_length] = '\0';
        return *this;
    }

    constexpr SIMDString& append(const value_type* s, size_type t) {
        ensureAllocation(m_length + t + 1);
        memcpy(m_data + m_length, s, t);
        m_length += t;
        m_data[m_length] = '\0';
        return *this;
    }

    constexpr SIMDString& append(const value_type* s) {
        return (*this) += s;
    }

    constexpr SIMDString& append(iterator first, iterator last) {
        return append(first, last - first);
    }

    constexpr SIMDString& append(std::initializer_list<value_type> ilist) {
        return this->append(ilist.begin(), ilist.size());
    }

    constexpr SIMDString& append(std::string_view& sv) {
        return this->append(sv.begin(), sv.size());
    }

    constexpr SIMDString& append(std::string_view& sv, size_type pos, size_type count) {
        return this->append(sv.begin() + pos, count);
    }

    constexpr void swap(SIMDString& str) {
        std::swap<size_type>(m_allocated, str.m_allocated);
        std::swap<Allocator>(m_allocator, str.m_allocator);

        // this has to be swapped first
        if (m_data != m_buffer && str.m_data != str.m_buffer) {
            std::swap<value_type*>(m_data, str.m_data);
        }
        else if (m_data != m_buffer) {
            str.m_data = m_data;
            m_data = m_buffer;
        }
        else if (str.m_data != str.m_buffer) {
            m_data = str.m_data;
            str.m_data = str.m_buffer;
        } // if both store strings in buffer, don't swap m_data, otherwise swapping buffers will swap it back

        swapBuffer(m_buffer, str.m_buffer);
        std::swap<size_type>(m_length, str.m_length);
    }

    constexpr bool starts_with(value_type c) const {
        return m_length > 0 && *m_data == c;
    }

    constexpr bool starts_with(const value_type* s) const {
        size_type n = ::strlen(s);
        return m_length >= n && memcmp(s, m_data, n) == 0;
    }

    constexpr bool starts_with(std::string_view sv) const {
        return m_length >= sv.size() && memcmp(sv.data(), m_data, sv.size()) == 0;
    }

    constexpr bool ends_with(value_type c) const {
        return m_length > 0 && *(m_data + m_length - 1) == c;
    }

    constexpr bool ends_with(const value_type* s) const {
        size_type n = ::strlen(s);
        return m_length >= n && memcmp(s, m_data + m_length - n, n) == 0;
    }

    constexpr bool ends_with(std::string_view sv) const {
        return m_length >= sv.size() && memcmp(sv.data(), m_data + m_length - sv.size(), sv.size()) == 0;
    }

    constexpr SIMDString substr(size_type pos, size_type count = npos) const {
        const size_type slen = std::max((size_type)0, std::min(m_length - pos, count));
        if (slen == 0) { return SIMDString(); }

        // If copying from a const segment and ending at the end of the string, do not allocate
        if (inConst() && (m_length == pos + slen)) {
            return SIMDString(m_data + pos);
        }
        else {
            return SIMDString(m_data + pos, slen);
        }
    }

    constexpr bool contains(std::string_view sv) const {
        return find(sv.begin(), 0) != 0;
    }

    constexpr bool contains(value_type c) const {
        return find(c, 0) != 0;
    }

    constexpr bool contains(const value_type* s) const {
        return find(s, 0, ::strlen(s)) != 0;
    }

    constexpr size_type find(const SIMDString& str, size_type pos = 0) const {
        return find(str.c_str(), pos, str.m_length);
    }

    constexpr size_type find(const value_type* s, size_type pos = 0) const {
        return find(s, pos, ::strlen(s));
    }

    constexpr size_type find(const value_type* s, size_type pos, size_type count) const
    {
        if (pos + count > m_length) {
            return npos;
        }

        if (count == 0) {
            return pos;
        }

        value_type* pFound = static_cast<value_type*>(memchr(m_data + pos, *s, m_length));
        size_type i = static_cast<size_type>(pFound - m_data);

        while (pFound && (i + count) <= m_length) {
            if (memcmp(pFound, s, count) == 0) {
                return i;
            }
            pFound = static_cast<value_type*>(memchr(pFound + 1, *s, m_length));
            i = static_cast<size_type>(pFound - m_data);
        }
        return npos;
    }

    constexpr size_type find(value_type c, size_type pos = 0) const {
        value_type* pFound = (value_type*)memchr(m_data + pos, c, m_length);
        return static_cast<size_type>(pFound - m_data);
    }

    constexpr size_type find(const std::string_view& sv, size_type pos = 0) const {
        return find(sv.begin(), 0, sv.size());
    }

    constexpr size_type rfind(const SIMDString& str, size_type pos = npos) const {
        return rfind(str.c_str(), pos, str.m_length);
    }

    constexpr size_type rfind(const value_type* s, size_type pos = npos) const {
        return rfind(s, pos, ::strlen(s));
    }

    constexpr size_type rfind(const value_type* s, size_type pos, size_type count) const {
        if (count > m_length) {
            return npos;
        }

        size_type n_1 = count - 1;
        size_type i = std::min(m_length - count, pos);
        value_type* leftBound = m_data + n_1;
        value_type endVal = *(s + n_1);

        if (count == 0) {
            return i;
        }

        do {
            if (*(leftBound + i) == endVal && !memcmp(m_data + i, s, count)) {
                return i;
            }
        } while (i--);

        return npos;
    }

    constexpr size_type rfind(value_type c, size_type pos = npos) const {
        size_type start = pos >= m_length ? m_length - 1 : pos;
        do {
            if (m_data[start] == c) { return start; }
        } while (start--);
        return npos;
    }

    constexpr size_type rfind(const std::string_view& sv, size_type pos = 0) const {
        return rfind(sv.begin(), 0, sv.size());
    }

    constexpr size_type find_first_of(const value_type* s, size_type pos, size_type count) const {
        if (pos > m_length) {
            return npos;
        }

        size_type i = pos;

        do {
            // search for current letter in the string of letters
            if (memchr(s, *(m_data + i), count)) {
                return i;
            }
        } while (++i < m_length);

        return npos;
    }

    constexpr size_type find_first_of(const SIMDString& str, size_type pos = 0) const {
        return find_first_of(str.c_str(), pos, str.length());
    }

    constexpr size_type find_first_of(const value_type* s, size_type pos = 0) const {
        return find_first_of(s, pos, ::strlen(s));
    }

    constexpr size_type find_first_of(value_type c, size_type pos = 0) const {
        if (pos > m_length) {
            return npos;
        }

        value_type* pFound = static_cast<value_type*>(memchr(m_data + pos, c, m_length));
        if (pFound) {
            return pFound - m_data;
        }
        return npos;
    }

    constexpr size_type find_first_of(const std::string_view& sv, size_type pos = 0) const {
        return find_first_of(sv.begin(), 0, sv.size());
    }

    constexpr size_type find_first_not_of(const value_type* s, size_type pos, size_type count) const {
        if (pos > m_length) {
            return npos;
        }

        size_type i = pos;

        do {
            // search for current letter in the string of letters
            if (!memchr(s, *(m_data + i), count)) {
                return i;
            }
        } while (++i < m_length);

        return npos;
    }

    constexpr size_type find_first_not_of(const SIMDString& str, size_type pos = 0) const {
        return find_first_not_of(str.c_str(), pos, str.length());
    }

    constexpr size_type find_first_not_of(const value_type* s, size_type pos = 0) const {
        return find_first_not_of(s, pos, ::strlen(s));
    }

    constexpr size_type find_first_not_of(value_type c, size_type pos = 0) const {
        if (pos > m_length) {
            return npos;
        }

        size_type i = pos;

        do {
            if (c != *(m_data + i)) {
                return i;
            }
        } while (++i < m_length); // do not want to run when i=m_length

        return npos;
    }

    constexpr size_type find_first_not_of(const std::string_view& sv, size_type pos = 0) const {
        return find_first_not_of(sv.begin(), 0, sv.size());
    }

    constexpr size_type find_last_of(const value_type* s, size_type pos, size_type count) const {
        // search [m_data, m_data + pos]
        size_type i = std::min(m_length - 1, pos);

        do {
            if (memchr(s, *(m_data + i), count)) {
                return i;
            }
        } while (i--);

        return npos;
    }

    constexpr size_type find_last_of(const SIMDString& str, size_type pos = npos) const {
        return find_last_of(str.c_str(), pos, str.length());
    }

    constexpr size_type find_last_of(const value_type* s, size_type pos = npos) const {
        return find_last_of(s, pos, ::strlen(s));
    }

    constexpr size_type find_last_of(value_type c, size_type pos = npos) const {
        size_type i = std::min(m_length - 1, pos);

        do {
            if (c == *(m_data + i)) {
                return i;
            }
        } while (i--); // want to run on i=0

        return npos;
    }

    constexpr size_type find_last_of(const std::string_view& sv, size_type pos = 0) const {
        return find_last_of(sv.begin(), 0, sv.size());
    }

    constexpr size_type find_last_not_of(const value_type* s, size_type pos, size_type count) const {
        // search [m_data, m_data + pos]
        size_type i = std::min(m_length - 1, pos);
        value_type* leftBound = m_data;

        do {
            if (!memchr(s, *(leftBound + i), count)) {
                return i;
            }
        } while (i--);

        return npos;
    }

    constexpr size_type find_last_not_of(const SIMDString& str, size_type pos = npos) const {
        return find_last_not_of(str.c_str(), pos, str.length());
    }

    constexpr size_type find_last_not_of(const value_type* s, size_type pos = npos) const {
        return find_last_not_of(s, pos, ::strlen(s));
    }

    constexpr size_type find_last_not_of(value_type c, size_type pos = npos) const {
        size_type i = std::min(m_length - 1, pos);
        value_type* leftBound = m_data;

        do {
            if (c != *(leftBound + i)) {
                return i;
            }
        } while (i--);

        return npos;
    }

    constexpr size_type find_last_not_of(const std::string_view& sv, size_type pos = 0) const {
        return find_last_not_of(sv.begin(), 0, sv.size());
    }

private:

    // Does not stop for internal null terminators.  Does not include the null
    // terminators.
    // See http://www.cplusplus.com/reference/string/string/compare/
    constexpr int compare(const value_type* a, size_type alen, const value_type* b, size_type blen) const noexcept {
        const size_type count = std::min(alen, blen);
        int res = memcmp(a, b, count);
        return res ? res : (int) (alen - blen);
    }

public:

    constexpr int compare(const SIMDString& str) const {
        if (m_data == str.m_data && m_length == str.m_length) {
            return 0;
        }
        else {
            return compare(m_data, m_length, str.m_data, str.m_length);
        }
    }

    constexpr int compare(size_type pos, size_type count, const SIMDString& str) const {
        return compare(m_data + pos, std::min(m_length - pos, count), str.m_data, str.m_length);
    }

    constexpr int compare(size_type pos, size_type count1, const SIMDString& str, size_type pos2, size_type count2) const {
        return compare(m_data + pos, std::min(m_length - pos, count1), str.m_data + pos2, std::min(str.m_length - pos2, count2));
    }

    constexpr int compare(const value_type* s) const {
        return compare(m_data, m_length, s, ::strlen(s));
    }

    constexpr int compare(size_type pos, size_type count, const value_type* s) const {
        return compare(m_data + pos, std::min(m_length - pos, count), s, ::strlen(s));
    }

    constexpr int compare(size_type pos, size_type count1, const value_type* s, size_type count2) const {
        return compare(m_data + pos, std::min(m_length - pos, count1), s, count2);
    }

    constexpr int compare(const std::string_view& sv) const noexcept {
        return compare(m_data, m_length, sv.begin(), sv.size());
    }

    constexpr int compare(size_type pos, size_type count, const std::string_view& sv) const {
        return compare(m_data + pos, count, sv.begin(), sv.size());
    }

    constexpr int compare(size_type pos, size_type count1, const std::string_view& sv, size_type pos2, size_type count2) const {
        return compare(m_data + pos, count1, sv.begin() + pos2, count2);
    }

    constexpr bool operator==(const SIMDString& s) const {
        return (m_length == s.m_length) && ((m_data == s.m_data) || !memcmp(m_data, s.m_data, m_length));
    }

    constexpr bool operator==(const value_type* s) const {
        return (m_length == ::strlen(s)) && ((m_data == s) || !memcmp(m_data, s, m_length));
    }

    constexpr bool operator!=(const SIMDString& s) const {
        return !(*this == s);
    }

    constexpr bool operator!=(const value_type* s) const {
        return !(*this == s);
    }

    constexpr bool operator>(const SIMDString& s) const {
        return compare(s) > 0;
    }

    constexpr bool operator<(const SIMDString& s) const {
        return compare(s) < 0;
    }

    constexpr bool operator>=(const SIMDString& s) const {
        return compare(s) >= 0;
    }

    constexpr bool operator<=(const SIMDString& s) const {
        return compare(s) <= 0;
    }
}
#ifdef __APPLE__
__attribute__((__aligned__(SSO_ALIGNMENT)))
#endif
;

#undef m_allocated


TEMPLATE inline SIMDString<INTERNAL_SIZE, Allocator> operator+(const typename SIMDString<INTERNAL_SIZE, Allocator>::value_type* s1, const SIMDString<INTERNAL_SIZE, Allocator>& s2) {
    return SIMDString<INTERNAL_SIZE, Allocator>(s1) + s2;
}

TEMPLATE inline SIMDString<INTERNAL_SIZE, Allocator> operator+(const typename SIMDString<INTERNAL_SIZE, Allocator>::value_type s1, const SIMDString<INTERNAL_SIZE, Allocator>& s2) {
    return SIMDString<INTERNAL_SIZE, Allocator>(s1) + s2;
}

TEMPLATE inline SIMDString<INTERNAL_SIZE, Allocator> operator+(const SIMDString<INTERNAL_SIZE, Allocator>&& s1,
    SIMDString<INTERNAL_SIZE, Allocator>&& s2) {
    return std::move(s1.append(s2));
}

TEMPLATE inline SIMDString<INTERNAL_SIZE, Allocator> operator+(const SIMDString<INTERNAL_SIZE, Allocator>&& s1,
    const SIMDString<INTERNAL_SIZE, Allocator>& s2) {
    return std::move(s1.append(s2));
}

TEMPLATE inline SIMDString<INTERNAL_SIZE, Allocator> operator+(const SIMDString<INTERNAL_SIZE, Allocator>&& s1,
    const typename SIMDString<INTERNAL_SIZE, Allocator>::value_type* rhs) {
    return std::move(s1.append(rhs));
}

TEMPLATE inline SIMDString<INTERNAL_SIZE, Allocator> operator+(const SIMDString<INTERNAL_SIZE, Allocator>&& s1,
    const typename SIMDString<INTERNAL_SIZE, Allocator>::value_type rhs) {
    return std::move(s1.append(rhs));
}

TEMPLATE std::ostream& operator<<(std::ostream& os, const SIMDString<INTERNAL_SIZE, Allocator>& str) {
    return os << str.c_str();
}

TEMPLATE std::istream& operator>>(std::istream& is, SIMDString<INTERNAL_SIZE, Allocator>& str)
{
    typename SIMDString<INTERNAL_SIZE, Allocator>::size_type numExtracted = 0;
    std::istream::ios_base::iostate err = std::istream::ios_base::goodbit;
    std::istream::sentry sen(is);

    if (sen) {
        try
        {
            str.erase();
            const typename SIMDString<INTERNAL_SIZE, Allocator>::size_type n =
                is.width() > 0
                    ? static_cast<typename SIMDString<INTERNAL_SIZE,Allocator>::size_type>(is.width())
                    : str.max_size();
            typename SIMDString<INTERNAL_SIZE, Allocator>::value_type c = is.rdbuf()->sgetc();

            while (numExtracted < n && c != EOF && !std::isspace(c, is.getloc())) {
                str += c;
                ++numExtracted;
                c = is.rdbuf()->snextc();
            }

            if (numExtracted < n && c == EOF) {
                err |= std::istream::ios_base::eofbit;
                is.width(0);
            }
        }
        catch (...) {
            is.setstate(std::istream::ios_base::badbit);
            throw;
        }
    }

    if (!numExtracted) {
        err |= std::istream::ios_base::failbit;
    }
    if (err) {
        is.setstate(err);
    }

    return is;
}

TEMPLATE std::istream& getline(std::istream& is, SIMDString<INTERNAL_SIZE, Allocator>& str, typename SIMDString<INTERNAL_SIZE, Allocator>::value_type delim = '\n')
{
    typename SIMDString<INTERNAL_SIZE, Allocator>::size_type numExtracted = 0;
    std::istream::ios_base::iostate  err = std::istream::ios_base::goodbit;
    std::istream::sentry sen(is, true);

    if (sen) {
        try
        {
            str.erase();
            const typename SIMDString<INTERNAL_SIZE, Allocator>::size_type n = str.max_size();
            typename SIMDString<INTERNAL_SIZE, Allocator>::value_type c = is.rdbuf()->sgetc();

            while (numExtracted < n && c != EOF && c != delim) {
                str += c;
                ++numExtracted;
                c = is.rdbuf()->snextc();
            }

            if (numExtracted < n && c == EOF) {
                err |= std::istream::ios_base::eofbit;
                is.width(0);
            }
        }
        catch (...) {
            is.setstate(std::istream::ios_base::badbit);
            throw;
        }
    }

    if (!numExtracted) {
        err |= std::istream::ios_base::failbit;
    }
    if (err) {
        is.setstate(err);
    }

    return is;
}

TEMPLATE inline int stoi(const SIMDString<INTERNAL_SIZE, Allocator> str, typename SIMDString<INTERNAL_SIZE, Allocator>::size_type* pos = nullptr, int base = 10) {
    if (pos) {
        typename SIMDString<INTERNAL_SIZE, Allocator>::value_type* end;
        int tmp = ::strtol(str.c_str(), &end, base);
        *pos = end - str.c_str();
        return tmp;
    }
    else {
        return ::strtol(str.c_str(), nullptr, base);
    }
}

TEMPLATE inline long stol(const SIMDString<INTERNAL_SIZE, Allocator> str, typename SIMDString<INTERNAL_SIZE, Allocator>::size_type* pos = nullptr, int base = 10) {
    if (pos) {
        typename SIMDString<INTERNAL_SIZE, Allocator>::value_type* end;
        long tmp = ::strtol(str.c_str(), &end, base);
        *pos = end - str.c_str();
        return tmp;
    }
    else {
        return ::strtol(str.c_str(), nullptr, base);
    }
}

TEMPLATE inline long long stoll(const SIMDString<INTERNAL_SIZE, Allocator> str, typename SIMDString<INTERNAL_SIZE, Allocator>::size_type* pos = nullptr, int base = 10) {
    if (pos) {
        typename SIMDString<INTERNAL_SIZE, Allocator>::value_type* end;
        long long tmp = ::strtoll(str.c_str(), &end, base);
        *pos = end - str.c_str();
        return tmp;
    }
    else {
        return ::strtoll(str.c_str(), nullptr, base);
    }
}

TEMPLATE inline unsigned long stoul(const SIMDString<INTERNAL_SIZE, Allocator> str, typename SIMDString<INTERNAL_SIZE, Allocator>::size_type* pos = nullptr, int base = 10) {
    if (pos) {
        typename SIMDString<INTERNAL_SIZE, Allocator>::value_type* end;
        unsigned long tmp = ::strtoul(str.c_str(), &end, base);
        *pos = end - str.c_str();
        return tmp;
    }
    else {
        return ::strtoul(str.c_str(), nullptr, base);
    }
}

TEMPLATE inline unsigned long long stoull(const SIMDString<INTERNAL_SIZE, Allocator> str, typename SIMDString<INTERNAL_SIZE, Allocator>::size_type* pos = nullptr, int base = 10) {
    if (pos) {
        typename SIMDString<INTERNAL_SIZE, Allocator>::value_type* end;
        unsigned long long tmp = ::strtoull(str.c_str(), &end, base);
        *pos = end - str.c_str();
        return tmp;
    }
    else {
        return ::strtoull(str.c_str(), nullptr, base);
    }
}

TEMPLATE inline float stof(const SIMDString<INTERNAL_SIZE, Allocator> str, typename SIMDString<INTERNAL_SIZE, Allocator>::size_type* pos = nullptr) {
    if (pos) {
        typename SIMDString<INTERNAL_SIZE, Allocator>::value_type* end;
        float tmp = ::strtof(str.c_str(), &end);
        *pos = end - str.c_str();
        return tmp;
    }
    else {
        return ::strtof(str.c_str(), nullptr);
    }
}

TEMPLATE inline double stod(const SIMDString<INTERNAL_SIZE, Allocator> str, typename SIMDString<INTERNAL_SIZE, Allocator>::size_type* pos = nullptr) {
    if (pos) {
        typename SIMDString<INTERNAL_SIZE, Allocator>::value_type* end;
        double tmp = ::strtod(str.c_str(), &end);
        *pos = end - str.c_str();
        return tmp;
    }
    else {
        return ::strtod(str.c_str(), nullptr);
    }
}

TEMPLATE inline long double stold(const SIMDString<INTERNAL_SIZE, Allocator> str, typename SIMDString<INTERNAL_SIZE, Allocator>::size_type* pos = nullptr) {
    if (pos) {
        typename SIMDString<INTERNAL_SIZE, Allocator>::value_type* end;
        long double tmp = ::strtold(str.c_str(), &end);
        *pos = end - str.c_str();
        return tmp;
    }
    else {
        return ::strtold(str.c_str(), nullptr);
    }
}

TEMPLATE SIMDString<INTERNAL_SIZE, Allocator> to_string(int value)
{
    // digits10 returns floor value, so add 1 for remainder, 1 for - and 1 for null terminator
    typename SIMDString<INTERNAL_SIZE, Allocator>::value_type str[std::numeric_limits<unsigned int>::digits10 + 3];
    std::sprintf(str, "%d", value);
    return SIMDString<INTERNAL_SIZE, Allocator>(str);
}

TEMPLATE SIMDString<INTERNAL_SIZE, Allocator> to_string(long value)
{
    typename SIMDString<INTERNAL_SIZE, Allocator>::value_type str[std::numeric_limits<unsigned long>::digits10 + 3];
    sprintf(str, "%ld", value);
    return SIMDString<INTERNAL_SIZE, Allocator>(str);
}

TEMPLATE SIMDString<INTERNAL_SIZE, Allocator> to_string(long long value)
{
    typename SIMDString<INTERNAL_SIZE, Allocator>::value_type str[std::numeric_limits<unsigned long long>::digits10 + 3];
    sprintf(str, "%lld", value);
    return SIMDString<INTERNAL_SIZE, Allocator>(str);
}

TEMPLATE SIMDString<INTERNAL_SIZE, Allocator> to_string(unsigned int value)
{
    typename SIMDString<INTERNAL_SIZE, Allocator>::value_type str[std::numeric_limits<unsigned int>::digits10 + 3];
    sprintf(str, "%u", value);
    return SIMDString<INTERNAL_SIZE, Allocator>(str);
}

TEMPLATE SIMDString<INTERNAL_SIZE, Allocator> to_string(unsigned long value)
{
    typename SIMDString<INTERNAL_SIZE, Allocator>::value_type str[std::numeric_limits<unsigned long>::digits10 + 3];
    sprintf(str, "%lu", value);
    return SIMDString<INTERNAL_SIZE, Allocator>(str);
}

TEMPLATE SIMDString<INTERNAL_SIZE, Allocator> to_string(unsigned long long value)
{
    typename SIMDString<INTERNAL_SIZE, Allocator>::value_type str[std::numeric_limits<unsigned long long>::digits10 + 3];
    sprintf(str, "%llu", value);
    return SIMDString<INTERNAL_SIZE, Allocator>(str);
}

TEMPLATE SIMDString<INTERNAL_SIZE, Allocator> to_string(float value)
{
    typename SIMDString<INTERNAL_SIZE, Allocator>::value_type str[std::numeric_limits<float>::max_exponent + std::numeric_limits<float>::max_digits10 + 6];
    sprintf(str, "%f", value);
    return SIMDString<INTERNAL_SIZE, Allocator>(str);
}

TEMPLATE SIMDString<INTERNAL_SIZE, Allocator> to_string(double value)
{
    typename SIMDString<INTERNAL_SIZE, Allocator>::value_type str[std::numeric_limits<double>::max_exponent + std::numeric_limits<double>::max_digits10 + 6];
    sprintf(str, "%f", value);
    return SIMDString<INTERNAL_SIZE, Allocator>(str);
}

TEMPLATE SIMDString<INTERNAL_SIZE, Allocator> to_string(long double value)
{
    typename SIMDString<INTERNAL_SIZE, Allocator>::value_type str[std::numeric_limits<long double>::max_exponent + std::numeric_limits<long double>::max_digits10 + 6];
    sprintf(str, "%Lf", value);
    return SIMDString<INTERNAL_SIZE, Allocator>(str);
}

#undef TEMPLATE