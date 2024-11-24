#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>
#include <set>

void Random_Init(uint32_t seed);
uint32_t Random(int min, int max);
double RandomDouble();

//Get a random element from a vector or array
template <typename T>
T RandomElement(std::vector<T>& vector, bool erase) {
    const auto idx = Random(0, vector.size());
    const T selected = vector[idx];
    if (erase) {
        vector.erase(vector.begin() + idx);
    }
    return selected;
}
template <typename Container>
auto& RandomElement(Container& container) {
    return container[Random(0, std::size(container))];
}
template <typename Container>
const auto& RandomElement(const Container& container) {
    return container[Random(0, std::size(container))];
}

template <typename T>
const T RandomElementFromSet(const std::set<T>& set) {
    if (set.size() == 1) {
        return *set.begin();
    }
    uint32_t rand = Random(0, set.size());
    auto it = set.begin();
    for (uint32_t i = 0; i < rand; i++) {
        it++;
    }
    auto test = *it;
    return *it;
}

//Shuffle items within a vector or array
//RANDOTODO There's probably a more efficient way to do what this does.
template <typename T>
void Shuffle(std::vector<T>& vector) {
    for (std::size_t i = 0; i + 1 < vector.size(); i++)
    {
        std::swap(vector[i], vector[Random(i, vector.size())]);
    }
}
template <typename T, std::size_t size>
void Shuffle(std::array<T, size>& arr) {
    for (std::size_t i = 0; i + 1 < arr.size(); i++)
    {
        std::swap(arr[i], arr[Random(i, arr.size())]);
    }
}
