#pragma once

#include <unordered_map>
#include <string_view>
#include <optional>

template <typename T> const std::unordered_map<std::string_view, T>& GetStringToEnumMap();

template <typename T> std::optional<T> StringToEnum(const std::string_view& str) {
    const auto& map = GetStringToEnumMap<T>();
    auto it = map.find(str);
    if (it != map.end()) {
        return it->second;
    } else {
        return std::nullopt;
    }
}

template <typename T> const std::unordered_map<T, std::string_view>& GetEnumToStringMap();

template <typename T> std::optional<std::string_view> EnumToString(const T& value) {
    const auto& map = GetEnumToStringMap<T>();
    auto it = map.find(value);
    if (it != map.end()) {
        return it->second;
    } else {
        return std::nullopt;
    }
}
