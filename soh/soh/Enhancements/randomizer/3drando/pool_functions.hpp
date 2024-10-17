#pragma once

#include <algorithm>
#include <vector>
#include <iostream>
#include <iterator>

template <typename T, typename Predicate>
static void erase_if(std::vector<T>& vector, Predicate pred) {
  vector.erase(std::remove_if(begin(vector), end(vector), pred), end(vector));
}

template <typename T, typename Predicate>
std::vector<T> FilterFromPool(std::vector<T>& vector, Predicate pred, bool eraseAfterFilter = false) {
  std::vector<T> filteredPool = {};
  std::copy_if(vector.begin(), vector.end(), std::back_inserter(filteredPool), pred);

  if (eraseAfterFilter) {
    erase_if(vector, pred);
  }

  return filteredPool;
}

template <typename T, typename Predicate>
std::vector<T> FilterAndEraseFromPool(std::vector<T>& vector, Predicate pred) {
  return FilterFromPool(vector, pred, true);
}

template <typename T, typename FromPool>
void AddElementsToPool(std::vector<T>& toPool, const FromPool& fromPool) {
  toPool.insert(toPool.end(), std::cbegin(fromPool), std::cend(fromPool));
}

template <typename T, typename Container>
bool ElementInContainer(T& element, const Container& container) {
  return std::find(container.begin(), container.end(), element) != container.end();
}

template <typename First, typename... T> bool IsAnyOf(First&& first, T&&... t) {
  return ((first == t) || ...);
}
