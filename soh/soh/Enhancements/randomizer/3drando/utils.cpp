#include "utils.hpp"

// Removes any line breaks from s.
std::string RemoveLineBreaks(std::string s) {
  s.erase(std::remove(s.begin(), s.end(), '\n'), s.end());
  return s;
}