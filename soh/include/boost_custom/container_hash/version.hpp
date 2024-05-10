
#ifndef BOOST_CONTAINER_HASH_VERSION_HPP
#define BOOST_CONTAINER_HASH_VERSION_HPP

#include <boost/version.hpp>

#define BOOST_VERSION_HAS_HASH_RANGE ((BOOST_VERSION / 100 % 1000) >= 81)

#define BOOST_USE_STD_TYPES ((BOOST_VERSION / 100 % 1000) >= 84)

#endif // #ifndef BOOST_CONTAINER_HASH_VERSION_HPP
