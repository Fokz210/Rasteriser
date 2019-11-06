#ifndef ALIGNING_MALLOCATOR_HPP
#define ALIGNING_MALLOCATOR_HPP

#include <xmmintrin.h>

template<typename T, std::size_t align>
struct aligning_mallocator
{
    using value_type = T;
    template<typename U>
    struct rebind
    {
        using other = aligning_mallocator<U, align>;
    };

    T *allocate(std::size_t const n) noexcept
    {
        return static_cast<T *>(_mm_malloc(n * sizeof(T), align));
    }
    void deallocate(T * const ptr, std::size_t const) noexcept
    {
        _mm_free(ptr);
    }
};

#endif // ALIGNING_MALLOCATOR_HPP
