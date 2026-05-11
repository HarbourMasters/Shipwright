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

template <typename T, typename FromPool> void AddElementsToPool(std::vector<T>& toPool, const FromPool& fromPool) {
    toPool.insert(toPool.end(), std::cbegin(fromPool), std::cend(fromPool));
}

template <typename T, typename Container> bool ElementInContainer(T& element, const Container& container) {
    return std::find(container.begin(), container.end(), element) != container.end();
}
