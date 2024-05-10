// 32 bit implementation based off of Boost hash
// Only implementing 32 bit versions integral and string based hashes

#ifndef BOOST_FUNCTIONAL_HASH_HASH_32_HPP
#define BOOST_FUNCTIONAL_HASH_HASH_32_HPP

#include <boost/container_hash/hash.hpp>
#include <boost_custom/container_hash/hash_fwd_32.hpp>
#include <boost_custom/container_hash/detail/hash_mix_32.hpp>
#include <boost_custom/container_hash/detail/hash_range_32.hpp>
#include <boost_custom/container_hash/version.hpp>

#if !BOOST_VERSION_HAS_HASH_RANGE
#include <boost/type_traits/is_unsigned.hpp>
#include <boost/type_traits/make_unsigned.hpp>

#if BOOST_WORKAROUND(__GNUC__, < 3) \
    && !defined(__SGI_STL_PORT) && !defined(_STLPORT_VERSION)
#define BOOST_HASH_CHAR_TRAITS string_char_traits
#else
#define BOOST_HASH_CHAR_TRAITS char_traits
#endif

#endif // #if !BOOST_VERSION_HAS_HASH_RANGE

#if BOOST_USE_STD_TYPES
#define BOOST_ENABLE_IF std::enable_if
#define BOOST_IS_INTEGRAL hash_detail::is_integral
#define BOOST_IS_UNSIGNED is_unsigned
#define BOOST_MAKE_UNSIGNED make_unsigned
#else
#define BOOST_ENABLE_IF boost::enable_if_
#define BOOST_IS_INTEGRAL boost::is_integral
#define BOOST_IS_UNSIGNED boost::is_unsigned
#define BOOST_MAKE_UNSIGNED boost::make_unsigned
#endif

namespace boost
{

    //
    // boost::hash_value
    //

    // integral types

    namespace hash_detail
    {
        template<class T,
            bool bigger_than_size_t = (sizeof(T) > sizeof(uint32_t)),
            bool is_unsigned = BOOST_IS_UNSIGNED<T>::value,
            std::size_t size_t_bits = sizeof(uint32_t) * CHAR_BIT,
            std::size_t type_bits = sizeof(T) * CHAR_BIT>
        struct hash_integral_impl_32;

        template<class T, bool is_unsigned, std::size_t size_t_bits, std::size_t type_bits> struct hash_integral_impl_32<T, false, is_unsigned, size_t_bits, type_bits>
        {
            static uint32_t fn( T v )
            {
                return static_cast<uint32_t>( v );
            }
        };

        template<class T, std::size_t size_t_bits, std::size_t type_bits> struct hash_integral_impl_32<T, true, false, size_t_bits, type_bits>
        {
            static uint32_t fn( T v )
            {
                typedef typename BOOST_MAKE_UNSIGNED<T>::type U;

                if( v >= 0 )
                {
                    return hash_integral_impl_32<U>::fn( static_cast<U>( v ) );
                }
                else
                {
                    return ~hash_integral_impl_32<U>::fn( static_cast<U>( ~static_cast<U>( v ) ) );
                }
            }
        };

        template<class T> struct hash_integral_impl_32<T, true, true, 32, 64>
        {
            static uint32_t fn( T v )
            {
                uint32_t seed = 0;

                seed = static_cast<uint32_t>( v >> 32 ) + hash_detail::hash_mix_32( seed );
                seed = static_cast<uint32_t>( v ) + hash_detail::hash_mix_32( seed );

                return seed;
            }
        };

        template<class T> struct hash_integral_impl_32<T, true, true, 32, 128>
        {
            static uint32_t fn( T v )
            {
                uint32_t seed = 0;

                seed = static_cast<uint32_t>( v >> 96 ) + hash_detail::hash_mix_32( seed );
                seed = static_cast<uint32_t>( v >> 64 ) + hash_detail::hash_mix_32( seed );
                seed = static_cast<uint32_t>( v >> 32 ) + hash_detail::hash_mix_32( seed );
                seed = static_cast<uint32_t>( v ) + hash_detail::hash_mix_32( seed );

                return seed;
            }
        };

    } // namespace hash_detail

    template <typename T>
    typename BOOST_ENABLE_IF<BOOST_IS_INTEGRAL<T>::value, uint32_t>::type
        hash_value_32( T v )
    {
        return hash_detail::hash_integral_impl_32<T>::fn( v );
    }

    // contiguous ranges (string, vector, array)
#if BOOST_VERSION_HAS_HASH_RANGE
    template <typename T>
    typename BOOST_ENABLE_IF<container_hash::is_contiguous_range<T>::value, uint32_t>::type
        hash_value_32( T const& v )
    {
        return boost::hash_range_32( v.data(), v.data() + v.size() );
    }
#else
    template <class Ch, class A>
    inline uint32_t hash_value_32(
        std::basic_string<Ch, std::BOOST_HASH_CHAR_TRAITS<Ch>, A> const& v)
    {
        return boost::hash_range_32( v.data(), v.data() + v.size() );
    }
#endif

    //
    // boost::hash_combine
    //

    template <class T>
    inline void hash_combine_32( uint32_t& seed, T const& v )
    {
        seed = boost::hash_detail::hash_mix_32( seed + 0x9e3779b9 + boost::hash_32<T>()( v ) );
    }

    //
    // boost::hash_range
    //

    template <class It>
    inline void hash_range_32( uint32_t& seed, It first, It last )
    {
        seed = hash_detail::hash_range_32( seed, first, last );
    }

    template <class It>
    inline uint32_t hash_range_32( It first, It last )
    {
        uint32_t seed = 0;

        hash_range_32( seed, first, last );

        return seed;
    }

    //
    // boost::hash
    //

    template <class T> struct hash_32
    {
        typedef T argument_type;
        typedef uint32_t result_type;

        uint32_t operator()( T const& val ) const
        {
            return hash_value_32( val );
        }
    };

} // namespace boost

#undef BOOST_HASH_CHAR_TRAITS
#undef BOOST_ENABLE_IF
#undef BOOST_IS_INTEGRAL
#undef BOOST_IS_UNSIGNED
#undef BOOST_MAKE_UNSIGNED

#endif // #ifndef BOOST_FUNCTIONAL_HASH_HASH_32_HPP
