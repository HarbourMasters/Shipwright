// 32 bit implementation based off of Boost hash

#ifndef BOOST_HASH_DETAIL_HASH_MIX_32_HPP
#define BOOST_HASH_DETAIL_HASH_MIX_32_HPP

#include <boost/cstdint.hpp>
#include <cstddef>
#include <climits>

namespace boost
{
namespace hash_detail
{

// clang-format off
// not sure how to get this to play nice, it wants to change it so
// template<> is on it's own line all the time
// https://reviews.llvm.org/D42684
template<uint32_t Bits> struct hash_mix_impl_32;

// hash_mix for 32 bit
//
// We use the "best xmxmx" implementation from
// https://github.com/skeeto/hash-prospector/issues/19

template<> struct hash_mix_impl_32<32>
{
    inline static boost::uint32_t fn( boost::uint32_t x )
    {
        boost::uint32_t const m1 = 0x21f0aaad;
        boost::uint32_t const m2 = 0x735a2d97;

        x ^= x >> 16;
        x *= m1;
        x ^= x >> 15;
        x *= m2;
        x ^= x >> 15;

        return x;
    }
};
// clang-format on

inline uint32_t hash_mix_32( uint32_t v )
{
    return hash_mix_impl_32<32>::fn( v );
}

} // namespace hash_detail
} // namespace boost

#endif // #ifndef BOOST_HASH_DETAIL_HASH_MIX_32_HPP
