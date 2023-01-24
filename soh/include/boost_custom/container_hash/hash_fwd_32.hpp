// 32 bit implementation based off of Boost hash

#ifndef BOOST_FUNCTIONAL_HASH_FWD_32_HPP
#define BOOST_FUNCTIONAL_HASH_FWD_32_HPP

#include <boost/container_hash/hash_fwd.hpp>

namespace boost
{

namespace container_hash
{

} // namespace container_hash

template<class T> struct hash_32;

template<class T> void hash_combine_32( uint32_t& seed, T const& v );

template<class It> void hash_range_32( uint32_t&, It, It );
template<class It> uint32_t hash_range_32( It, It );

} // namespace boost

#endif // #ifndef BOOST_FUNCTIONAL_HASH_FWD_32_HPP
