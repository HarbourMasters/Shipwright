// 32 bit implementation based off of Boost hash
// Only implementing 32 bit version of char based ranges

#ifndef BOOST_HASH_DETAIL_HASH_RANGE_32_HPP
#define BOOST_HASH_DETAIL_HASH_RANGE_32_HPP

#include <boost_custom/container_hash/hash_fwd_32.hpp>
#include <boost_custom/container_hash/version.hpp>

#if BOOST_VERSION_HAS_HASH_RANGE
#include <boost/container_hash/detail/hash_range.hpp>
#else
#include <boost/type_traits/integral_constant.hpp>
#include <boost/type_traits/enable_if.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/cstdint.hpp>
#include <cstddef>
#include <climits>
#include <iterator>
#endif // #if BOOST_VERSION_HAS_HASH_RANGE

namespace boost
{
namespace hash_detail
{

#if !BOOST_VERSION_HAS_HASH_RANGE

template<class T> struct is_char_type: public boost::false_type {};

#if CHAR_BIT == 8

template<> struct is_char_type<char>: public boost::true_type {};
template<> struct is_char_type<signed char>: public boost::true_type {};
template<> struct is_char_type<unsigned char>: public boost::true_type {};

#if defined(__cpp_char8_t) && __cpp_char8_t >= 201811L
template<> struct is_char_type<char8_t>: public boost::true_type {};
#endif

#if defined(__cpp_lib_byte) && __cpp_lib_byte >= 201603L
template<> struct is_char_type<std::byte>: public boost::true_type {};
#endif

#endif

#endif // #if !BOOST_VERSION_HAS_HASH_RANGE

#if BOOST_USE_STD_TYPES
#define BOOST_ENABLE_IF std::enable_if
#define BOOST_IS_SAME std::is_same
#else
#define BOOST_ENABLE_IF boost::enable_if_
#define BOOST_IS_SAME is_same
#endif

template<class It>
inline typename BOOST_ENABLE_IF<
    is_char_type<typename std::iterator_traits<It>::value_type>::value &&
    BOOST_IS_SAME<typename std::iterator_traits<It>::iterator_category, std::random_access_iterator_tag>::value,
std::size_t>::type
    hash_range_32( uint32_t seed, It first, It last )
{
    std::size_t n = static_cast<std::size_t>( last - first );

    for( ; n >= 4; first += 4, n -= 4 )
    {
        // clang 5+, gcc 5+ figure out this pattern and use a single mov on x86
        // gcc on s390x and power BE even knows how to use load-reverse

        boost::uint32_t w =
            static_cast<boost::uint32_t>( static_cast<unsigned char>( first[0] ) ) |
            static_cast<boost::uint32_t>( static_cast<unsigned char>( first[1] ) ) <<  8 |
            static_cast<boost::uint32_t>( static_cast<unsigned char>( first[2] ) ) << 16 |
            static_cast<boost::uint32_t>( static_cast<unsigned char>( first[3] ) ) << 24;

        hash_combine_32( seed, w );
    }

    {
        // add a trailing suffix byte of 0x01 because otherwise sequences of
        // trailing zeroes are indistinguishable from end of string

        boost::uint32_t w = 0x01u;

        switch( n )
        {
        case 1:

            w =
                static_cast<boost::uint32_t>( static_cast<unsigned char>( first[0] ) ) |
                0x0100u;

            break;

        case 2:

            w =
                static_cast<boost::uint32_t>( static_cast<unsigned char>( first[0] ) ) |
                static_cast<boost::uint32_t>( static_cast<unsigned char>( first[1] ) ) <<  8 |
                0x010000u;

            break;

        case 3:

            w =
                static_cast<boost::uint32_t>( static_cast<unsigned char>( first[0] ) ) |
                static_cast<boost::uint32_t>( static_cast<unsigned char>( first[1] ) ) <<  8 |
                static_cast<boost::uint32_t>( static_cast<unsigned char>( first[2] ) ) << 16 |
                0x01000000u;

            break;
        }

        hash_combine_32( seed, w );
    }

    return seed;
}

} // namespace hash_detail
} // namespace boost

#undef BOOST_ENABLE_IF
#undef BOOST_IS_SAME

#endif // #ifndef BOOST_HASH_DETAIL_HASH_RANGE_32_HPP
