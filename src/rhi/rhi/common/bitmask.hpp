#pragma once

#include <type_traits>
#include <utility>

template<typename T>
requires std::is_enum_v<T>
constexpr static bool RHI_ENABLE_BIT_OPERATORS = false;

template<typename T>
requires RHI_ENABLE_BIT_OPERATORS<T>
constexpr auto operator&(T left, T right) noexcept
{
    return T(std::to_underlying(left) & std::to_underlying(right));
}

template<typename T>
requires RHI_ENABLE_BIT_OPERATORS<T>
constexpr auto operator|(T left, T right) noexcept
{
    return T(std::to_underlying(left) | std::to_underlying(right));
}

template<typename T>
requires RHI_ENABLE_BIT_OPERATORS<T>
constexpr auto operator^(T left, T right) noexcept
{
    return T(std::to_underlying(left) ^ std::to_underlying(right));
}

template<typename T>
requires RHI_ENABLE_BIT_OPERATORS<T>
constexpr auto operator~(T value) noexcept
{
    return T(~std::to_underlying(value));
}
