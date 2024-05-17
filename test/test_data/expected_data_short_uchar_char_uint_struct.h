#ifndef EXPECTED_DATA_SHORT_UCHAR_CHAR_UINT_STRUCT_H
#define EXPECTED_DATA_SHORT_UCHAR_CHAR_UINT_STRUCT_H

#include "interface_expected_data.h"
#include "tested_data_models.h"

namespace test_data {
namespace struct_short_uchar_char_uint {

// NOLINTBEGIN (google-runtime-int)
struct ShortUCharCharIntStruct {
  short a;
  unsigned char b;
  char c;
  int d;
};
// NOLINTEND (google-runtime-int)

template <typename DataModelT>
class ExpectedData : public IExpectedData<ExpectedData<DataModelT>> {};

template <>
class ExpectedData<test_data::data_models::Simple32BitDataModel>
    : public IExpectedData<
          ExpectedData<test_data::data_models::Simple32BitDataModel>> {
 public:
  using DataModelTypeImpl = test_data::data_models::Simple32BitDataModel;
  using SerializedTypeImpl = ShortUCharCharIntStruct;

  static constexpr std::size_t kExpectedPaddingLocationsCountImpl = 0;
  static constexpr std::array<std::size_t, 0>
      kExpectedPaddingLocationsByteCountsImpl{};
  static constexpr std::array<std::size_t, 0> kExpectedPaddingByteIndexesImpl{};
};

template <>
class ExpectedData<test_data::data_models::NoPaddingDataModel>
    : public IExpectedData<
          ExpectedData<test_data::data_models::NoPaddingDataModel>> {
 public:
  using DataModelTypeImpl = test_data::data_models::NoPaddingDataModel;
  using SerializedTypeImpl = ShortUCharCharIntStruct;

  static constexpr std::size_t kExpectedPaddingLocationsCountImpl = 0;
  static constexpr std::array<std::size_t, 0>
      kExpectedPaddingLocationsByteCountsImpl{};
  static constexpr std::array<std::size_t, 0> kExpectedPaddingByteIndexesImpl{};
};

}  // namespace struct_short_uchar_char_uint
}  // namespace test_data

#endif  // EXPECTED_DATA_SHORT_UCHAR_CHAR_UINT_STRUCT_H
