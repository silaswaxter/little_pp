#ifndef STD_ARRAY_COMPARISON_OPERATORS_H
#define STD_ARRAY_COMPARISON_OPERATORS_H

#include <array>
#include <cstddef>

template <std::size_t N>
struct OperatorDetails {
  template <std::size_t start, std::size_t end, std::size_t inc>
  static constexpr auto constexpr_for(std::array<std::size_t, N> lhs,
                                      std::array<std::size_t, N> rhs) ->
      typename std::enable_if<(start < end), bool>::type {
    if (std::get<start>(lhs) != std::get<start>(rhs)) {
      return false;
    }
    return constexpr_for<start + inc, end, inc>(lhs, rhs);
  }

  template <std::size_t start, std::size_t end, std::size_t inc>
  static constexpr auto constexpr_for(std::array<std::size_t, N> lhs,
                                      std::array<std::size_t, N> rhs) ->
      typename std::enable_if<!(start < end), bool>::type {
    return true;
  }
};

template <std::size_t N>
static constexpr auto operator==(std::array<std::size_t, N> lhs,
                                 std::array<std::size_t, N> rhs) -> bool {
  return OperatorDetails<N>::template constexpr_for<0, N, 1>(lhs, rhs);
}

template <std::size_t N1, std::size_t N2>
static constexpr auto operator==(std::array<std::size_t, N1> lhs,
                                 std::array<std::size_t, N2> rhs) -> bool {
  return false;
}

template <std::size_t N>
static constexpr auto operator!=(std::array<std::size_t, N> lhs,
                                 std::array<std::size_t, N> rhs) -> bool {
  return !(lhs == rhs);
}

template <std::size_t N1, std::size_t N2>
static constexpr auto operator!=(std::array<std::size_t, N1> lhs,
                                 std::array<std::size_t, N2> rhs) -> bool {
  return true;
}

constexpr std::array<std::size_t, 0> kEmptyArray{};
constexpr std::array<std::size_t, 2> kSomeArray{1, 2};
constexpr std::array<std::size_t, 2> kOtherArray{2, 1};
static_assert(kEmptyArray == kEmptyArray,
              "These are the same array and should be equal.");
static_assert(kSomeArray == kSomeArray,
              "These are the same array and should be equal.");
static_assert(kSomeArray != kEmptyArray,
              "These are not the same array and should NOT be equal.");
static_assert(kSomeArray != kOtherArray,
              "These are not the same array and should NOT be equal.");
//-------------------------------------------------

#endif  // STD_ARRAY_COMPARISON_OPERATORS_H
