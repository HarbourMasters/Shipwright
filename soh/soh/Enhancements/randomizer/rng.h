#pragma once

#include "soh/ShipUtils.h"
#include <array>
#include <cstddef>
#include <stdint.h>
#include <vector>
#include <set>

inline uint64_t rando_state = 0;

static inline void Random_Init(uint64_t seed) {
    ShipUtils::RandInit(seed, &rando_state);
}

// Returns a random integer in range [min, max-1]
static inline uint32_t Random(uint32_t min, uint32_t max) {
    return ShipUtils::Random(min, max, &rando_state);
}

// Returns a random floating point number in [0.0, 1.0)
static inline double RandomDouble() {
    return ShipUtils::RandomDouble(&rando_state);
}

// Get a random element from a vector or array
template <typename T> T RandomElement(std::vector<T>& vector, bool erase) {
    return ShipUtils::RandomElement(vector, erase, &rando_state);
}
template <typename Container> auto& RandomElement(Container& container) {
    return ShipUtils::RandomElement(container, &rando_state);
}
template <typename Container> const auto& RandomElement(const Container& container) {
    return ShipUtils::RandomElement(container, &rando_state);
}

template <typename T> const T RandomElementFromSet(const std::set<T>& set) {
    return ShipUtils::RandomElementFromSet(set, &rando_state);
}

// Shuffle items within a vector or array
template <typename T> void Shuffle(std::vector<T>& vector) {
    ShipUtils::Shuffle(vector, &rando_state);
}
template <typename T, size_t size> void Shuffle(std::array<T, size>& arr) {
    ShipUtils::Shuffle(arr, &rando_state);
}
