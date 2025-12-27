#ifndef SECUREVECTOR_H
#define SECUREVECTOR_H

#include <vector>
#include <memory>
#include <limits>

namespace Secure
{
    template <class T>
    struct SecureAllocator
    {
        typedef T value_type;

        SecureAllocator() noexcept {} 

        template <class U> SecureAllocator(const SecureAllocator<U>&) noexcept {}

        T* allocate(std::size_t n) {
            if (n > std::numeric_limits<std::size_t>::max() / sizeof(T))
                throw std::bad_alloc();
            return static_cast<T*>(::operator new(n * sizeof(T)));
        }

        void deallocate(T* p, std::size_t n) noexcept
        {
            if (p) {
                volatile unsigned char* v_p = reinterpret_cast<volatile unsigned char*>(p);
                size_t total_bytes = n * sizeof(T);
                for (size_t i = 0; i < total_bytes; ++i) {
                    v_p[i] = 0;
                }
            }::operator delete(p);
        }
    };
}

namespace Secure {
    using SecureVector = std::vector<unsigned char, SecureAllocator<unsigned char>>;
}

#endif /* SECUREVECTOR_H */