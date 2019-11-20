#pragma once

#include <type_traits>

// clang-format off
#if __cplusplus >= 201703L // C++17
#ifndef JSON_UTILS_NORETURN
#define JSON_UTILS_NORETURN [[noreturn]]
#endif

#ifndef JSON_UTILS_NODISCARD
#define JSON_UTILS_NODISCARD [[nodiscard]]
#endif

#ifndef JSON_UTILS_MAYBEUNUSED
#define JSON_UTILS_MAYBE_UNUSED [[maybe_unused]]
#endif
#else
#ifndef JSON_UTILS_NORETURN
#define JSON_UTILS_NORETURN
#endif

#ifndef JSON_UTILS_NODISCARD
#define JSON_UTILS_NODISCARD
#endif

#ifndef JSON_UTILS_MAYBEUNUSED
#define JSON_UTILS_MAYBEUNUSED
#endif
#endif
// clang-format on

/**
 * The functionality provided in this namespace emulates similar functionality found in C++17.
 */
namespace future_std
{
template <typename...> using void_t = void;
} // namespace future_std
