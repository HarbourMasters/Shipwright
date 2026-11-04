#pragma once

#include <algorithm>
#include <vector>
#include <iterator>

template <typename T, typename Predicate> std::vector<T> FilterFromPool(std::vector<T>& vector, Predicate pred) {
    std::vector<T> filteredPool = {};
    std::copy_if(vector.begin(), vector.end(), std::back_inserter(filteredPool), pred);
    return filteredPool;
}

template <typename T, typename Predicate>
std::vector<T> FilterAndEraseFromPool(std::vector<T>& vector, Predicate pred) {
    auto filtered = FilterFromPool(vector, pred);
    std::erase_if(vector, pred);
    return filtered;
}
