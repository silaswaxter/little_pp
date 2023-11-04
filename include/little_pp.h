#ifndef LITTLE_PP_H
#define LITTLE_PP_H

#include <array>
#include <boost/pfr/core.hpp>
#include <climits>
#include <iostream>

#include "data_model.h"

// Only supporting 8-bit bytes for now to avoid scope creep.
// Within the embedded space (motivation for LittlePP in the first place),
// non-8-bit byte architectures do still exist and are not just relics of the
// past. See this active TI DSP and this discussion:
// - https://www.ti.com/product/TMS320F2809
// - https://groups.google.com/g/comp.lang.c++/c/14QZUvWE_Xc
const std::size_t kRequiredCharBit = 8;
static_assert(CHAR_BIT == kRequiredCharBit,
              "LittlePP requires 8-bit bytes at this time. "
              "(Rationale explained in comment above.)");

//-------------------------------------------------
// std::array constexpr equality overloads
//-------------------------------------------------
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
  return !OperatorDetails<N>::template constexpr_for<0, N, 1>(lhs, rhs);
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

template <typename T>
struct IsSupportedType {
  // violate google style guide in favor of std library convention.
  // NOLINTNEXTLINE(readability-identifier-naming)
  static const bool value = std::is_arithmetic<T>::value ||
                            std::is_enum<T>::value || std::is_class<T>::value;
};

template <typename SerializableClassType, typename DataModelType>
struct SerialiazableClassAlignment {
  template <std::size_t start, std::size_t end, std::size_t inc>
  static constexpr auto constexpr_for(std::size_t current_alignment = 0) ->
      typename std::enable_if<(start < end), std::size_t>::type {
    // start is iterated; (the template recursion performs iteration)
    using FieldType =
        typename boost::pfr::tuple_element_t<start, SerializableClassType>;
    static_assert(IsSupportedType<FieldType>::value,
                  "Field type not supported.");

    std::size_t field_alignment =
        DataModelType::template get_alignment<FieldType>();
    current_alignment = (field_alignment > current_alignment)
                            ? field_alignment
                            : current_alignment;

    return constexpr_for<start + inc, end, inc>(current_alignment);
  }

  template <std::size_t start, std::size_t end, std::size_t inc>
  static constexpr auto constexpr_for(std::size_t current_alignment = 0) ->
      typename std::enable_if<!(start < end), std::size_t>::type {
    return current_alignment;
  }

  // violate google style guide in favor of std library convention.
  // NOLINTNEXTLINE(readability-identifier-naming)
  static constexpr std::size_t value =
      constexpr_for<0, boost::pfr::tuple_size_v<SerializableClassType>, 1>();
};

template <typename SerializableClassType, typename DataModelType>
struct SerializableClassPaddingIndexes {
  // Helpers (must be defined before these functions)
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

  // To create the compile-time array of padding indexes, this struct is
  // traversed multiple times returning an array describing the padding in
  // greater detail each time. Note, AFAIK, there isn't a way to combine these
  // traversals since the array's type is bound to its size. In summary:
  //   1. Count the number of locations padding occurs; this information is used
  //   in the next step (because the size of a array is bound the array type)
  //   2. For each location where padding occurs, count the number padding
  //   bytes.
  //   3. For each location where padding occurs, write the index of each byte
  //   to an array (using iteration with the previously found constexpr padding
  //   byte count).

  // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv STEP 1 vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
  // Prevent easily swappable parameters
  struct PaddingLocationsCountT {
    std::size_t value;
  };

  template <std::size_t start, std::size_t end, std::size_t inc>
  static constexpr auto padding_locations_count(
      PaddingLocationsCountT return_value = {0},
      std::size_t current_alignment_filled = 0) ->
      typename std::enable_if<(start < end), std::size_t>::type {
    // start is iterated; (the template recursion performs iteration)
    using FieldType =
        typename boost::pfr::tuple_element_t<start, SerializableClassType>;
    static_assert(IsSupportedType<FieldType>::value,
                  "Field type not supported.");

    constexpr std::size_t kFieldSize =
        DataModelType::template get_size<FieldType>();
    constexpr std::size_t kFieldAlignment =
        DataModelType::template get_alignment<FieldType>();

    // detect when padding should be inserted (including trailing padding)
    if (current_alignment_filled % kFieldAlignment != 0) {
      return_value.value++;
      std::size_t padding_bytes =
          (current_alignment_filled > kFieldAlignment)
              ? current_alignment_filled % kFieldAlignment
              : kFieldAlignment - current_alignment_filled;
      current_alignment_filled += padding_bytes;
    }

    // account for the field
    current_alignment_filled += kFieldSize;

    return padding_locations_count<start + inc, end, inc>(
        return_value, current_alignment_filled);
  }

  template <std::size_t start, std::size_t end, std::size_t inc>
  static constexpr auto padding_locations_count(
      PaddingLocationsCountT return_value = {0},
      std::size_t current_alignment_filled = 0) ->
      typename std::enable_if<!(start < end), std::size_t>::type {
    constexpr std::size_t kStructAlignment =
        SerialiazableClassAlignment<SerializableClassType,
                                    DataModelType>::value;

    // account for trailing padding
    if (kStructAlignment >
        0) {  // when processing empty struct, its alignment will be 0 and will
              // cause a divide-by-zero error.
      if (current_alignment_filled % kStructAlignment != 0) {
        return_value.value++;
      }
    }

    return return_value.value;
  }

  static constexpr std::size_t kPaddingLocationsCount = padding_locations_count<
      0, boost::pfr::tuple_size_v<SerializableClassType>, 1>();
  // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ STEP 1 ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv STEP 2 vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
  using PaddingLocationsPaddingByteCountType =
      std::array<std::size_t, kPaddingLocationsCount>;

  template <std::size_t start, std::size_t end, std::size_t inc,
            std::size_t append_index = 0>
  static constexpr auto padding_locations_padding_byte_count(
      PaddingLocationsPaddingByteCountType locations_byte_count =
          PaddingLocationsPaddingByteCountType{},
      std::size_t current_alignment_filled = 0) ->
      typename std::enable_if<(start < end),
                              PaddingLocationsPaddingByteCountType>::type {
    // start is iterated; (the template recursion performs iteration)
    using FieldType =
        typename boost::pfr::tuple_element_t<start, SerializableClassType>;
    static_assert(IsSupportedType<FieldType>::value,
                  "Field type not supported.");

    constexpr std::size_t kFieldSize =
        DataModelType::template get_size<FieldType>();
    constexpr std::size_t kFieldAlignment =
        DataModelType::template get_alignment<FieldType>();

    bool is_locations_byte_count_written = false;

    // detect when padding should be inserted
    if (current_alignment_filled % kFieldAlignment != 0) {
      std::size_t padding_bytes =
          (current_alignment_filled > kFieldAlignment)
              ? current_alignment_filled % kFieldAlignment
              : kFieldAlignment - current_alignment_filled;
      current_alignment_filled += padding_bytes;
      sfinae_set<append_index>(locations_byte_count, padding_bytes);
      is_locations_byte_count_written = true;
    }

    // account for the field
    current_alignment_filled += kFieldSize;

    if (is_locations_byte_count_written) {
      return padding_locations_padding_byte_count<start + inc, end, inc,
                                                  append_index + 1>(
          locations_byte_count, current_alignment_filled);
    }
    return padding_locations_padding_byte_count<start + inc, end, inc,
                                                append_index>(
        locations_byte_count, current_alignment_filled);
  }

  template <std::size_t start, std::size_t end, std::size_t inc,
            std::size_t append_index = 0>
  static constexpr auto padding_locations_padding_byte_count(
      PaddingLocationsPaddingByteCountType locations_byte_count =
          PaddingLocationsPaddingByteCountType{},
      std::size_t current_alignment_filled = 0) ->
      typename std::enable_if<!(start < end),
                              PaddingLocationsPaddingByteCountType>::type {
    constexpr std::size_t kStructAlignment =
        SerialiazableClassAlignment<SerializableClassType,
                                    DataModelType>::value;

    // account for trailing padding
    if (kStructAlignment >
        0) {  // when processing empty struct, its alignment will be 0 and will
              // cause a divide-by-zero error.
      if (current_alignment_filled % kStructAlignment != 0) {
        std::size_t padding_bytes =
            kStructAlignment - (current_alignment_filled % kStructAlignment);
        sfinae_set<append_index>(locations_byte_count, padding_bytes);
      }
    }
    return locations_byte_count;
  }

  static constexpr PaddingLocationsPaddingByteCountType
      kPaddingLocationsPaddingByteCount = padding_locations_padding_byte_count<
          0, boost::pfr::tuple_size_v<SerializableClassType>, 1>();
  // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ STEP 2 ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv STEP 2.5 vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
  template <std::size_t start, std::size_t end, std::size_t inc>
  static constexpr auto padding_bytes_count(std::size_t return_value = 0) ->
      typename std::enable_if<(start < end), std::size_t>::type {
    return_value += sfinae_get<start>(kPaddingLocationsPaddingByteCount);
    return padding_bytes_count<start + inc, end, inc>(return_value);
  }

  template <std::size_t start, std::size_t end, std::size_t inc>
  static constexpr auto padding_bytes_count(std::size_t return_value = 0) ->
      typename std::enable_if<!(start < end), std::size_t>::type {
    return return_value;
  }

  static constexpr std::size_t kPaddingBytesCount =
      padding_bytes_count<0, kPaddingLocationsCount, 1>();
  // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ STEP 2.5 ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv STEP 3 vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
  using PaddingByteIndexesType = std::array<std::size_t, kPaddingBytesCount>;

  template <std::size_t start, std::size_t end, std::size_t inc,
            std::size_t append_index = 0>
  static constexpr auto append_helper_padding_byte_indexes(
      PaddingByteIndexesType padding_byte_indexes,
      std::size_t first_padding_byte_index) ->
      typename std::enable_if<(start < end), PaddingByteIndexesType>::type {
    sfinae_set<append_index>(padding_byte_indexes, first_padding_byte_index);

    return append_helper_padding_byte_indexes<start + inc, end, inc,
                                              append_index + 1>(
        padding_byte_indexes, first_padding_byte_index + 1);
  }

  template <std::size_t start, std::size_t end, std::size_t inc,
            std::size_t append_index = 0>
  static constexpr auto append_helper_padding_byte_indexes(
      PaddingByteIndexesType padding_byte_indexes,
      std::size_t first_padding_byte_index) ->
      typename std::enable_if<!(start < end), PaddingByteIndexesType>::type {
    return padding_byte_indexes;
  }

  template <std::size_t start, std::size_t end, std::size_t inc,
            std::size_t padding_locations_padding_byte_count_index = 0,
            std::size_t padding_byte_indexes_append_index = 0>
  static constexpr auto padding_byte_indexes(
      PaddingByteIndexesType return_value = PaddingByteIndexesType{},
      std::size_t current_alignment_filled = 0) ->
      typename std::enable_if<(start < end), PaddingByteIndexesType>::type {
    // start is iterated; (the template recursion performs iteration)
    using FieldType =
        typename boost::pfr::tuple_element_t<start, SerializableClassType>;
    static_assert(IsSupportedType<FieldType>::value,
                  "Field type not supported.");

    constexpr std::size_t kStructAlignment =
        SerialiazableClassAlignment<SerializableClassType,
                                    DataModelType>::value;
    constexpr std::size_t kFieldSize =
        DataModelType::template get_size<FieldType>();
    constexpr std::size_t kFieldAlignment =
        DataModelType::template get_alignment<FieldType>();

    bool is_locations_byte_count_written = false;
    constexpr std::size_t kPaddingBytes =
        sfinae_get<padding_locations_padding_byte_count_index>(
            kPaddingLocationsPaddingByteCount);
    // detect when padding should be inserted (including trailing padding)
    if (current_alignment_filled % kFieldAlignment != 0) {
      return_value =
          append_helper_padding_byte_indexes<0, kPaddingBytes, 1,
                                             padding_byte_indexes_append_index>(
              return_value, current_alignment_filled);
      is_locations_byte_count_written = true;

      current_alignment_filled += kPaddingBytes;
    }

    // account for the field
    current_alignment_filled += kFieldSize;

    if (is_locations_byte_count_written) {
      return padding_byte_indexes<
          start + inc, end, inc, padding_locations_padding_byte_count_index + 1,
          padding_byte_indexes_append_index + kPaddingBytes>(
          return_value, current_alignment_filled);
    }
    return padding_byte_indexes<start + inc, end, inc,
                                padding_locations_padding_byte_count_index,
                                padding_byte_indexes_append_index>(
        return_value, current_alignment_filled);
  }

  template <std::size_t start, std::size_t end, std::size_t inc,
            std::size_t padding_locations_padding_byte_count_index = 0,
            std::size_t padding_byte_indexes_append_index = 0>
  static constexpr auto padding_byte_indexes(
      PaddingByteIndexesType return_value = PaddingByteIndexesType{},
      std::size_t current_alignment_filled = 0) ->
      typename std::enable_if<!(start < end), PaddingByteIndexesType>::type {
    constexpr std::size_t kStructAlignment =
        SerialiazableClassAlignment<SerializableClassType,
                                    DataModelType>::value;
    constexpr std::size_t kPaddingBytes =
        sfinae_get<padding_locations_padding_byte_count_index>(
            kPaddingLocationsPaddingByteCount);

    // account for trailing padding
    if (kStructAlignment >
        0) {  // when processing empty struct, its alignment will be 0 and will
              // cause a divide-by-zero error.
      if (current_alignment_filled % kStructAlignment != 0) {
        return_value = append_helper_padding_byte_indexes<
            0, kPaddingBytes, 1, padding_byte_indexes_append_index>(
            return_value, current_alignment_filled);
      }
    }
    return return_value;
  }

  static constexpr PaddingByteIndexesType kPaddingByteIndexes =
      padding_byte_indexes<0, boost::pfr::tuple_size_v<SerializableClassType>,
                           1>();
  //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^STEP
  // 3^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
};

#endif  // LITTLE_PP_H
