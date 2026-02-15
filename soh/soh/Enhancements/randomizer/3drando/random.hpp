#pragma once

#include "soh/ShipUtils.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>
#include <set>

extern uint64_t rando_state;

void Random_Init(uint64_t seed);
uint32_t Random(uint32_t min, uint32_t max);
double RandomDouble();

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
// RANDOTODO There's probably a more efficient way to do what this does.
template <typename T> void Shuffle(std::vector<T>& vector) {
    for (size_t i = 0; i + 1 < vector.size(); i++) {
        std::swap(vector[i], vector[Random(static_cast<uint32_t>(i), static_cast<uint32_t>(vector.size()))]);
    }
}
template <typename T, size_t size> void Shuffle(std::array<T, size>& arr) {
    for (size_t i = 0; i + 1 < arr.size(); i++) {
        std::swap(arr[i], arr[Random(static_cast<uint32_t>(i), static_cast<uint32_t>(arr.size()))]);
    }
}
