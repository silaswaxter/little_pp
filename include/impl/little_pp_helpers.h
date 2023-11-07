#ifndef LITTLE_PP_HELPERS_H
#define LITTLE_PP_HELPERS_H

// *** THIS FILE MIGHT BE DELETE, BUT ITS PROGRESS FOR NOW ***

#include <array>

namespace little_pp {

struct Helpers {
  template <std::size_t I, class T, std::size_t N>
  static constexpr auto sfinae_get(const std::array<T, N>& array) noexcept ->
      typename std::enable_if<!(I < N), const T>::type {
    return T{};
  }

  template <std::size_t I, class T, std::size_t N>
  static constexpr auto sfinae_get(const std::array<T, N>& array) noexcept ->
      typename std::enable_if<(I < N), const T>::type {
    return std::get<I>(array);
  }

  template <std::size_t I, class T, std::size_t N>
  static constexpr auto sfinae_set(std::array<T, N>& array, T val) noexcept ->
      typename std::enable_if<!(I < N), void>::type {}

  template <std::size_t I, class T, std::size_t N>
  static constexpr auto sfinae_set(std::array<T, N>& array, T val) noexcept ->
      typename std::enable_if<(I < N), void>::type {
    std::get<I>(array) = val;
  }
};

}  // namespace little_pp

#endif  // LITTLE_PP_HELPERS_H
