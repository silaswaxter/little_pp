// TODO: - Consider replacing constexpr_for with generic templated for utilizing
//         templates and visitor pattern on function that is declared as lambda;
//         see
//         https://stackoverflow.com/questions/37602057/why-isnt-a-for-loop-a-compile-time-expression
//       - FIX DOCS IN THIS FILLE:
//
// To create the compile-time array of padding indexes, this struct is
// traversed multiple times returning an array describing the padding in
// greater detail each time. It may be more efficient to start with a maximum
// sized array and truncate as needed; however, I already successfully
// implemented the multiple-traversal method--so to avoid premature
// optimization, hold off on refactoring until its actually needed.
//   1. Count the number of locations padding occurs; this information is used
//   in the next step (because the size of a array is bound the array type)
//   2. For each location where padding occurs, count the number padding
//   bytes.
//   3. For each location where padding occurs, write the index of each byte
//   to an array (using iteration with the previously found constexpr padding
//   byte count).

#ifndef LITTLE_PP_IMPL_PADDING_REFLECTION_H
#define LITTLE_PP_IMPL_PADDING_REFLECTION_H

#include <boost/pfr/core.hpp>
#include <iostream>
#include <type_traits>

#include "../data_model.h"
#include "../unique_padding_element.h"
#include "little_pp_helpers.h"

namespace little_pp {

namespace impl {

struct ArrayUtil {
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

template <typename T>
struct IsSerializableType {
  static constexpr bool kValue = std::is_arithmetic<T>::value ||
                                 std::is_enum<T>::value ||
                                 std::is_class<T>::value;
};

template <typename SerializableClassType, typename DataModelType>
struct SerializableClassAlignment {
  template <std::size_t start, std::size_t end, std::size_t inc>
  static constexpr auto constexpr_for(std::size_t current_alignment = 0) ->
      typename std::enable_if<(start < end), std::size_t>::type {
    // start is iterated; (the template recursion performs iteration)
    using FieldType =
        typename boost::pfr::tuple_element_t<start, SerializableClassType>;
    static_assert(IsSerializableType<FieldType>::kValue,
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

  static constexpr std::size_t kValue =
      constexpr_for<0, boost::pfr::tuple_size_v<SerializableClassType>, 1>();
};

template <typename SerializableClassType, typename DataModelType>
struct SerializableClassPaddingLocations {
  // strong-type prevents easily swappable parameters
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
    static_assert(IsSerializableType<FieldType>::kValue,
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
        SerializableClassAlignment<SerializableClassType,
                                   DataModelType>::kValue;

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

  static constexpr std::size_t kValue = padding_locations_count<
      0, boost::pfr::tuple_size_v<SerializableClassType>, 1>();
};

template <typename SerializableClassType, typename DataModelType>
struct SerializableClassPaddingLocationsByteCounts {
  using ReturnType =
      std::array<std::size_t,
                 SerializableClassPaddingLocations<SerializableClassType,
                                                   DataModelType>::kValue>;

  template <std::size_t start, std::size_t end, std::size_t inc,
            std::size_t append_index = 0>
  static constexpr auto padding_locations_byte_counts(
      ReturnType locations_byte_counts = ReturnType{},
      std::size_t current_alignment_filled = 0) ->
      typename std::enable_if<(start < end), ReturnType>::type {
    // start is iterated; (the template recursion performs iteration)
    using FieldType =
        typename boost::pfr::tuple_element_t<start, SerializableClassType>;
    static_assert(IsSerializableType<FieldType>::kValue,
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
      ArrayUtil::sfinae_set<append_index>(locations_byte_counts, padding_bytes);
      is_locations_byte_count_written = true;
    }

    // account for the field
    current_alignment_filled += kFieldSize;

    if (is_locations_byte_count_written) {
      return padding_locations_byte_counts<start + inc, end, inc,
                                           append_index + 1>(
          locations_byte_counts, current_alignment_filled);
    }
    return padding_locations_byte_counts<start + inc, end, inc, append_index>(
        locations_byte_counts, current_alignment_filled);
  }

  template <std::size_t start, std::size_t end, std::size_t inc,
            std::size_t append_index = 0>
  static constexpr auto padding_locations_byte_counts(
      ReturnType locations_byte_counts = ReturnType{},
      std::size_t current_alignment_filled = 0) ->
      typename std::enable_if<!(start < end), ReturnType>::type {
    constexpr std::size_t kStructAlignment =
        SerializableClassAlignment<SerializableClassType,
                                   DataModelType>::kValue;

    // account for trailing padding
    if (kStructAlignment > 0) {
      // when processing empty struct, its alignment will be 0 and will cause a
      // divide-by-zero error.
      if (current_alignment_filled % kStructAlignment != 0) {
        std::size_t padding_bytes =
            kStructAlignment - (current_alignment_filled % kStructAlignment);
        ArrayUtil::sfinae_set<append_index>(locations_byte_counts,
                                            padding_bytes);
      }
    }
    return locations_byte_counts;
  }

  static constexpr ReturnType kValue = padding_locations_byte_counts<
      0, boost::pfr::tuple_size_v<SerializableClassType>, 1>();
};

template <typename SerializableClassType, typename DataModelType>
struct SerializableClassPaddingByteCount {
  template <std::size_t start, std::size_t end, std::size_t inc>
  static constexpr auto padding_bytes_count(std::size_t return_value = 0) ->
      typename std::enable_if<(start < end), std::size_t>::type {
    return_value += ArrayUtil::sfinae_get<start>(
        SerializableClassPaddingLocationsByteCounts<SerializableClassType,
                                                    DataModelType>::kValue);
    return padding_bytes_count<start + inc, end, inc>(return_value);
  }

  template <std::size_t start, std::size_t end, std::size_t inc>
  static constexpr auto padding_bytes_count(std::size_t return_value = 0) ->
      typename std::enable_if<!(start < end), std::size_t>::type {
    return return_value;
  }

  static constexpr std::size_t kValue =
      padding_bytes_count<0,
                          SerializableClassPaddingLocations<
                              SerializableClassType, DataModelType>::kValue,
                          1>();
};

template <typename SerializableClassType, typename DataModelType>
struct SerializableClassPaddingIndexes {
  using ReturnType =
      std::array<std::size_t,
                 SerializableClassPaddingByteCount<SerializableClassType,
                                                   DataModelType>::kValue>;

  template <std::size_t start, std::size_t end, std::size_t inc,
            std::size_t append_index = 0>
  static constexpr auto append_helper_padding_byte_indexes(
      ReturnType padding_byte_indexes, std::size_t first_padding_byte_index) ->
      typename std::enable_if<(start < end), ReturnType>::type {
    ArrayUtil::sfinae_set<append_index>(padding_byte_indexes,
                                        first_padding_byte_index);

    return append_helper_padding_byte_indexes<start + inc, end, inc,
                                              append_index + 1>(
        padding_byte_indexes, first_padding_byte_index + 1);
  }

  template <std::size_t start, std::size_t end, std::size_t inc,
            std::size_t append_index = 0>
  static constexpr auto append_helper_padding_byte_indexes(
      ReturnType padding_byte_indexes, std::size_t first_padding_byte_index) ->
      typename std::enable_if<!(start < end), ReturnType>::type {
    return padding_byte_indexes;
  }

  template <std::size_t start, std::size_t end, std::size_t inc,
            std::size_t padding_locations_padding_byte_count_index = 0,
            std::size_t padding_byte_indexes_append_index = 0>
  static constexpr auto padding_byte_indexes(
      ReturnType return_value = ReturnType{},
      std::size_t current_alignment_filled = 0) ->
      typename std::enable_if<(start < end), ReturnType>::type {
    // start is iterated; (the template recursion performs iteration)
    using FieldType =
        typename boost::pfr::tuple_element_t<start, SerializableClassType>;
    static_assert(IsSerializableType<FieldType>::kValue,
                  "Field type not supported.");

    constexpr std::size_t kFieldSize =
        DataModelType::template get_size<FieldType>();
    constexpr std::size_t kFieldAlignment =
        DataModelType::template get_alignment<FieldType>();

    bool is_locations_byte_count_written = false;
    constexpr std::size_t kPaddingBytes =
        ArrayUtil::sfinae_get<padding_locations_padding_byte_count_index>(
            SerializableClassPaddingLocationsByteCounts<SerializableClassType,
                                                        DataModelType>::kValue);
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
      ReturnType return_value = ReturnType{},
      std::size_t current_alignment_filled = 0) ->
      typename std::enable_if<!(start < end), ReturnType>::type {
    constexpr std::size_t kStructAlignment =
        SerializableClassAlignment<SerializableClassType,
                                   DataModelType>::kValue;
    constexpr std::size_t kPaddingBytes =
        ArrayUtil::sfinae_get<padding_locations_padding_byte_count_index>(
            SerializableClassPaddingLocationsByteCounts<SerializableClassType,
                                                        DataModelType>::kValue);

    // account for trailing padding
    if (kStructAlignment > 0) {
      // when processing empty struct, its alignment will be 0 and will cause a
      // divide-by-zero error.
      if (current_alignment_filled % kStructAlignment != 0) {
        return_value = append_helper_padding_byte_indexes<
            0, kPaddingBytes, 1, padding_byte_indexes_append_index>(
            return_value, current_alignment_filled);
      }
    }
    return return_value;
  }

  static constexpr ReturnType kValue =
      padding_byte_indexes<0, boost::pfr::tuple_size_v<SerializableClassType>,
                           1>();
};

struct FillFromRemainingPaddingArray {
  template <std::size_t index, std::size_t NRet,
            typename ReturnArrayOffsetIntegralConstType, std::size_t NPadding,
            typename PaddingArrayOffsetIntegralConstType>
  static constexpr void func(
      std::array<unique_padding_element::UniquePaddingElement, NRet>&
          return_array,
      ReturnArrayOffsetIntegralConstType /*unused*/,
      std::array<std::size_t, NPadding>& padding_array,
      PaddingArrayOffsetIntegralConstType /*unused*/) {
    constexpr std::size_t kReturnArrayI =
        ReturnArrayOffsetIntegralConstType::value + index;
    constexpr std::size_t kPaddingArrayI =
        PaddingArrayOffsetIntegralConstType::value + index;
    std::get<kReturnArrayI>(return_array) =
        little_pp::unique_padding_element::UniquePaddingElement{
            std::get<kPaddingArrayI>(padding_array), 1, false};
  };
};

// struct RenameMe {
//   template <std::size_t first_index, std::size_t second_index = 0>
//   static constexpr void func(
//       std::integral_constant<std::size_t, second_index> /*unused*/ =
//           std::integral_constant<std::size_t, second_index>()) {
//     if (first_index == 1 && second_index == 0) {
//       func<first_index>(
//           std::integral_constant<std::size_t, second_index + 1>());
//     }
//
//     //   constexpr std::size_t first_unique_padding_bytes = 0;
//     //   constexpr std::size_t second_unique_padding_bytes = 0;
//     //
//     //   // when either the first or second padding has been fully iterated
//     //   through,
//     //   // add the rest of the other padding to the unique paddings.
//     //   if (first_i == first_padding_byte_indexes.size()) {
//     //     static_for<second_i, second_padding_byte_indexes.size(),
//     //                FillFromRemainingPaddingArray>(
//     //         return_array, i_return, second_padding_byte_indexes,
//     //         second_i);
//     //   }
//     //   if (second_i == second_padding_byte_indexes.size()) {
//     //     static_for<first_i, first_padding_byte_indexes.size(),
//     //                FillFromRemainingPaddingArray>(
//     //         return_array, i_return, first_padding_byte_indexes,
//     //         first_i);
//     //   }
//     //
//     //   // the padding index value can be compared to find uniqueness if it
//     is
//     //   // adjusted as if only non-unique padding is applied
//     //   // first_padding_byte_index = first_nonunique_padding_byte_index +
//     //   //                            first_accounted_unique_padding
//     //   std::size_t current_first_nonunique =
//     //       std::get<first_i>(first_padding_byte_indexes) +
//     //       first_unique_padding_bytes;
//     //   std::size_t current_second_nonunique =
//     //       std::get<first_i>(second_padding_byte_indexes) +
//     //       second_unique_padding_bytes;
//     //
//     //   if (current_first_nonunique == current_second_nonunique) {
//     //     // call loop with:
//     //     // first_i++;
//     //     // second_i++;
//     //     continue;
//     //   }
//     //   if (current_first_nonunique < current_second_nonunique) {
//     //     // do
//     //     return_array[i_return] =
//     //         little_pp::unique_padding_element::UniquePaddingElement{
//     //             first_padding_byte_indexes[first_i], 1, true};
//     //     i_return++;
//     //     // then call loop with:
//     //     first_i++;
//     //   }
//     //   if (current_second_nonunique < current_first_nonunique) {
//     //     // do
//     //     return_array[i_return] =
//     //         little_pp::unique_padding_element::UniquePaddingElement{
//     //             second_padding_byte_indexes[second_i], 1, false};
//     //     i_return++;
//     //     // then call this function with
//     //     second_i++;
//     //   }
//   }
// };

// template <std::size_t i1_max, std::size_t i2_max>
// struct UniquePaddingInternal {
//   template <std::size_t i1 = 0, std::size_t i2 = 0, std::size_t return_i = 0,
//             std::size_t N1, std::size_t N2, std::size_t NRet>
//   static constexpr inline auto func(
//       std::array<std::size_t, N1>& first_padding_byte_indexes,
//       std::array<std::size_t, N2>& second_padding_byte_indexes,
//       std::array<unique_padding_element::UniquePaddingElement, NRet>&
//           return_array) ->
//       typename std::enable_if<(i1 < i1_max && i2 < i2_max)>::type {
//     // normal; use index1=i1 and index2=i2
//
//     func_impl<i1, i2, return_i>(first_padding_byte_indexes,
//                                 second_padding_byte_indexes, return_array);
//     // instantiates more recursion
//   }
//
//   template <std::size_t i1 = 0, std::size_t i2 = 0, std::size_t return_i = 0,
//             std::size_t N1, std::size_t N2, std::size_t NRet>
//   static constexpr inline auto func(
//       std::array<std::size_t, N1>& first_padding_byte_indexes,
//       std::array<std::size_t, N2>& second_padding_byte_indexes,
//       std::array<unique_padding_element::UniquePaddingElement, NRet>&
//           return_array) ->
//       typename std::enable_if<(i1 < i1_max && i2 == i2_max)>::type {
//     // use index1=i1 and index2=(i2_max-1)
//
//     func_impl<i1, i2_max - 1, return_i>(
//         first_padding_byte_indexes, second_padding_byte_indexes,
//         return_array);
//     // instantiates more recursion
//   }
//
//   template <std::size_t i1 = 0, std::size_t i2 = 0, std::size_t return_i = 0,
//             std::size_t N1, std::size_t N2, std::size_t NRet>
//   static constexpr inline auto func(
//       std::array<std::size_t, N1>& first_padding_byte_indexes,
//       std::array<std::size_t, N2>& second_padding_byte_indexes,
//       std::array<unique_padding_element::UniquePaddingElement, NRet>&
//           return_array) ->
//       typename std::enable_if<(i1 == i1_max && i2 < i2_max)>::type {
//     // use index1=(i1_max-1) and index2=i2
//
//     func_impl<i1_max - 1, i1, return_i>(
//         first_padding_byte_indexes, second_padding_byte_indexes,
//         return_array);
//     // instantiates more recursion
//   }
//
//   template <std::size_t i1 = 0, std::size_t i2 = 0, std::size_t return_i = 0,
//             std::size_t N1, std::size_t N2, std::size_t NRet>
//   static constexpr inline auto func(
//       std::array<std::size_t, N1>& first_padding_byte_indexes,
//       std::array<std::size_t, N2>& second_padding_byte_indexes,
//       std::array<unique_padding_element::UniquePaddingElement, NRet>&
//           return_array) ->
//       typename std::enable_if<(i1 == i1_max && i2 == i2_max)>::type {
//     // returns from recursion
//   }
//
//   template <std::size_t i1, std::size_t i2, std::size_t return_i,
//             std::size_t first_unique_padding_bytes = 0,
//             std::size_t second_unique_padding_bytes = 0, std::size_t N1,
//             std::size_t N2, std::size_t NRet>
//   static constexpr void func_impl(
//       std::array<std::size_t, N1>& first_padding_byte_indexes,
//       std::array<std::size_t, N2>& second_padding_byte_indexes,
//       std::array<unique_padding_element::UniquePaddingElement, NRet>&
//           return_array) {
//     //   constexpr std::size_t first_unique_padding_bytes = 0;
//     //   constexpr std::size_t second_unique_padding_bytes = 0;
//     //
//     // when either the first or second padding has been fully iterated
//     through,
//     // add the rest of the other padding to the unique paddings.
//     if (i1 == i1_max) {
//       static_for<i2, i2_max, FillFromRemainingPaddingArray>(
//           return_array, std::integral_constant<std::size_t, return_i>(),
//           second_padding_byte_indexes,
//           std::integral_constant<std::size_t, i2>());
//       return;
//     }
//     if (i2 == i2_max) {
//       static_for<i1, i1_max, FillFromRemainingPaddingArray>(
//           return_array, std::integral_constant<std::size_t, return_i>(),
//           first_padding_byte_indexes,
//           std::integral_constant<std::size_t, i1>());
//       return;
//     }
//
//     // the padding index value can be compared to find uniqueness if it is
//     // adjusted as if only non-unique padding is applied
//     // first_padding_byte_index = first_nonunique_padding_byte_index +
//     //                            first_accounted_unique_padding
//     // constexpr kIndex1 = std::get<i1>(first_padding_byte_indexes);
//
//     // constexpr std::size_t kCurrentFirstNonunique =
//     //     std::get<i1>(first_padding_byte_indexes) -
//     first_unique_padding_bytes;
//     // constexpr std::size_t kCurrentSecondNonunique =
//     //     std::get<i2>(second_padding_byte_indexes) -
//     second_unique_padding_bytes;
//
//     // if (kCurrentFirstNonunique == kCurrentSecondNonunique) {
//     //   func<i1 + 1, i2 + 1, return_i>(first_padding_byte_indexes,
//     //                                  second_padding_byte_indexes,
//     //                                  return_array);
//     // }
//     // if (kCurrentFirstNonunique < kCurrentSecondNonunique) {
//     //   // do
//     //   std::get<return_i>(return_array) =
//     //       little_pp::unique_padding_element::UniquePaddingElement{
//     //           std::get<i1>(first_padding_byte_indexes), 1, true};
//     //   func<i1 + 1, i2, return_i + 1>(first_padding_byte_indexes,
//     //                                  second_padding_byte_indexes,
//     //                                  return_array);
//     // }
//     // if (kCurrentSecondNonunique < kCurrentFirstNonunique) {
//     //   // do
//     //   return_array[i_return] =
//     //       little_pp::unique_padding_element::UniquePaddingElement{
//     //           second_padding_byte_indexes[second_i], 1, false};
//     //   i_return++;
//     //   // then call this function with
//     //   second_i++;
//     // }
//   }
// };

template <typename SerializableClassType, typename FirstDataModelType,
          typename SecondDataModelType>
struct UniquePaddingImpl {
  static constexpr auto kFirstArray =
      SerializableClassPaddingIndexes<SerializableClassType,
                                      FirstDataModelType>::kValue;
  static constexpr auto kSecondArray =
      SerializableClassPaddingIndexes<SerializableClassType,
                                      SecondDataModelType>::kValue;

  using FirstArrayType = decltype(kFirstArray);
  using SecondArrayType = decltype(kSecondArray);

  // A single "entry-point" function allows refactoring the implementation
  // while maintaining a clean minimal interface; will likely need to refactor
  // into a divide-and-conquer then combine approach to circumvent recursion
  // limits which will be hit for very large structs.
  static constexpr auto value() {
    constexpr auto kUntrimmedArray = func_impl();
    return trimmer<std::get<1>(kUntrimmedArray)>(std::get<0>(kUntrimmedArray));
  }

  // IMPLEMENTATION OF ALGORITHM
  // ---------------------------
  // Use SFINAE enable_if to create an array of unique padding indexes from an
  // array of the first data model padding indexes and an array of the second
  // data model padding indexes.
  //
  // We have 4 functions:
  //  1. A function for when we are NOT at the end of EITHER padding index
  //     arrays
  //  2. A function for when we are at the end of the first padding index array
  //  3. A function for when we are at the end of the second padding index array
  //  4. A function for when we are at the end of BOTH padding index arrays
  // ---------------------------
  using UntrimmedReturnType =
      std::array<little_pp::unique_padding_element::UniquePaddingElement,
                 kFirstArray.size() + kSecondArray.size()>;
  // this type holds the untrimmed array and the last appended index so that
  // the trimmer can know what std::array size to return
  using TrimPreparedReturnType = std::tuple<UntrimmedReturnType, std::size_t>;

  // Since the returned array is compressed (i.e. consecutive indexes are
  // captured with first index and length), we need conditional appending. This
  // set of functions takes care of this.
  // ---------------------------
  // returns true if element was appended by adding true to the previous
  // element's span, returns false if a new element was added.
  template <std::size_t i_return, std::size_t padding_index_value,
            bool is_padding_for_first_passed_data_model>
  static constexpr auto append_unique_padding_element(
      UntrimmedReturnType& return_array) ->
      typename std::enable_if<(i_return == 0), bool>::type {
    // append new non-spanning padding index value
    std::get<i_return>(return_array) =
        little_pp::unique_padding_element::UniquePaddingElement{
            padding_index_value, 1, is_padding_for_first_passed_data_model};
    return false;
  }

  // returns true if element was appended by adding true to the previous
  // element's span, returns false if a new element was added.
  template <std::size_t i_return, std::size_t padding_index_value,
            bool is_padding_for_first_passed_data_model>
  static constexpr auto append_unique_padding_element(
      UntrimmedReturnType& return_array) ->
      typename std::enable_if<(i_return > 0), bool>::type {
    auto previous_returned_padding_index_value =
        std::get<i_return - 1>(return_array).padding_first_index;
    auto previous_returned_padding_length =
        std::get<i_return - 1>(return_array).padding_span_length;
    // check if this padding index is part of a span and can be added by
    // increasing length of padding indexes of previous returned padding
    // index
    if (previous_returned_padding_index_value ==
        (padding_index_value - previous_returned_padding_length)) {
      std::get<i_return - 1>(return_array).padding_span_length++;
      return true;
    }
    // append new non-spanning padding index value
    std::get<i_return>(return_array) =
        little_pp::unique_padding_element::UniquePaddingElement{
            padding_index_value, 1, is_padding_for_first_passed_data_model};
    return false;
  }

  template <std::size_t i1 = 0, std::size_t i2 = 0, std::size_t i_return = 0,
            std::size_t unique_padding_bytes_1 = 0,
            std::size_t unique_padding_bytes_2 = 0>
  static constexpr auto func_impl(UntrimmedReturnType return_array = {}) ->
      typename std::enable_if<i1 != kFirstArray.size() &&
                                  i2 != kSecondArray.size(),
                              TrimPreparedReturnType>::type {
    std::size_t normalized_padding_index_1 =
        std::get<i1>(kFirstArray) - unique_padding_bytes_1;
    std::size_t normalized_padding_index_2 =
        std::get<i2>(kSecondArray) - unique_padding_bytes_2;

    if (normalized_padding_index_1 == normalized_padding_index_2) {
      return func_impl<i1 + 1, i2 + 1, i_return, unique_padding_bytes_1,
                       unique_padding_bytes_2>(return_array);
    }
    if (normalized_padding_index_1 < normalized_padding_index_2) {
      if (append_unique_padding_element<i_return, std::get<i1>(kFirstArray),
                                        true>(return_array)) {
        return func_impl<i1 + 1, i2, i_return, unique_padding_bytes_1 + 1,
                         unique_padding_bytes_2>(return_array);
      }
      return func_impl<i1 + 1, i2, i_return + 1, unique_padding_bytes_1 + 1,
                       unique_padding_bytes_2>(return_array);
    }
    if (normalized_padding_index_1 > normalized_padding_index_2) {
      if (append_unique_padding_element<i_return, std::get<i2>(kSecondArray),
                                        false>(return_array)) {
        return func_impl<i1, i2 + 1, i_return, unique_padding_bytes_1,
                         unique_padding_bytes_2 + 1>(return_array);
      }
      return func_impl<i1, i2 + 1, i_return + 1, unique_padding_bytes_1,
                       unique_padding_bytes_2 + 1>(return_array);
    }
  }

  template <std::size_t i1 = 0, std::size_t i2 = 0, std::size_t i_return = 0,
            std::size_t unique_padding_bytes_1 = 0,
            std::size_t unique_padding_bytes_2 = 0>
  static constexpr auto func_impl(UntrimmedReturnType return_array = {}) ->
      typename std::enable_if<i1 == kFirstArray.size() &&
                                  i2 != kSecondArray.size(),
                              TrimPreparedReturnType>::type {
    // Since we are at the end of the first array, the rest of the second array
    // has unique indexes
    if (append_unique_padding_element<i_return, std::get<i2>(kSecondArray),
                                      false>(return_array)) {
      return func_impl<i1, i2 + 1, i_return, unique_padding_bytes_1,
                       unique_padding_bytes_2 + 1>(return_array);
    }
    return func_impl<i1, i2 + 1, i_return + 1, unique_padding_bytes_1,
                     unique_padding_bytes_2 + 1>(return_array);
  }

  template <std::size_t i1 = 0, std::size_t i2 = 0, std::size_t i_return = 0,
            std::size_t unique_padding_bytes_1 = 0,
            std::size_t unique_padding_bytes_2 = 0>
  static constexpr auto func_impl(UntrimmedReturnType return_array = {}) ->
      typename std::enable_if<i1 != kFirstArray.size() &&
                                  i2 == kSecondArray.size(),
                              TrimPreparedReturnType>::type {
    // Since we are at the end of the second array, the rest of the first array
    // has unique indexes
    if (append_unique_padding_element<i_return, std::get<i1>(kFirstArray),
                                      true>(return_array)) {
      return func_impl<i1 + 1, i2, i_return, unique_padding_bytes_1 + 1,
                       unique_padding_bytes_2>(return_array);
    }
    return func_impl<i1 + 1, i2, i_return + 1, unique_padding_bytes_1 + 1,
                     unique_padding_bytes_2>(return_array);
  }

  template <std::size_t i1 = 0, std::size_t i2 = 0, std::size_t i_return = 0,
            std::size_t unique_padding_bytes_1 = 0,
            std::size_t unique_padding_bytes_2 = 0>
  static constexpr auto func_impl(UntrimmedReturnType return_array = {}) ->
      typename std::enable_if<i1 == kFirstArray.size() &&
                                  i2 == kSecondArray.size(),
                              TrimPreparedReturnType>::type {
    // Since we are at the end of both arrays, return from recursion, but first
    // trim empty elements from the end.
    return {return_array, i_return};
  }

  template <std::size_t last_appdended_index, std::size_t i = 0>
  static constexpr auto trimmer(
      UntrimmedReturnType untrimmed_array,
      std::array<unique_padding_element::UniquePaddingElement,
                 last_appdended_index>
          trimmed_array = {}) ->
      typename std::enable_if<
          (i < last_appdended_index),
          std::array<unique_padding_element::UniquePaddingElement,
                     last_appdended_index>>::type {
    std::get<i>(trimmed_array) = std::get<i>(untrimmed_array);
    return trimmer<last_appdended_index, i + 1>(untrimmed_array, trimmed_array);

  }

  template <std::size_t last_appdended_index, std::size_t i = 0>
  static constexpr auto trimmer(
      UntrimmedReturnType untrimmed_array,
      std::array<unique_padding_element::UniquePaddingElement,
                 last_appdended_index>
          trimmed_array = {}) ->
      typename std::enable_if<
          (i == last_appdended_index),
          std::array<unique_padding_element::UniquePaddingElement,
                     last_appdended_index>>::type {
    return trimmed_array;
  }
};

}  // namespace impl

}  // namespace little_pp

#endif  // LITTLE_PP_IMPL_PADDING_REFLECTION_H
