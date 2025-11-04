#pragma once

#include <functional>
#include <string_view>

namespace bookdb {

struct TransparentStringLess {
    using is_transparent = void;

    template <typename T1, typename T2>
    constexpr bool operator()(T1 &&lhs, T2 &&rhs) const
        requires std::convertible_to<T1, std::string_view> && std::convertible_to<T2, std::string_view>
    {
        return std::string_view(lhs) < std::string_view(rhs);
    }
};

struct TransparentStringEqual {
    using is_transparent = void;

    template <typename T1, typename T2>
    constexpr bool operator()(T1 &&lhs, T2 &&rhs) const
        requires std::convertible_to<T1, std::string_view> && std::convertible_to<T2, std::string_view>
    {
        return std::string_view(lhs) == std::string_view(rhs);
    }
};

struct TransparentStringHash {
    using is_transparent = void;
    using hash_type = std::hash<std::string_view>;

    template <typename T>
    constexpr std::size_t operator()(T &&t) const
        requires std::convertible_to<T, std::string_view>
    {
        return hash_type{}(std::string_view(t));
    }
};

}  // namespace bookdb