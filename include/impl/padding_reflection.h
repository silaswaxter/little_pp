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
#include <type_traits>

#include "../data_model.h"
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

}  // namespace impl

}  // namespace little_pp

#endif  // LITTLE_PP_IMPL_PADDING_REFLECTION_H
