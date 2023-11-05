#ifndef EXPECTED_DATA_NO_PADDING_32BIT_STRUCT_H
#define EXPECTED_DATA_NO_PADDING_32BIT_STRUCT_H

#include "interface_expected_data.hpp"
#include "tested_data_models.h"

namespace test_data {
namespace struct_no_padding_32bit {

// NOLINTBEGIN (google-runtime-int)
struct NoPadding32Bit {
  unsigned short a;
  unsigned char b;
  unsigned char c;
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
  using SerializedTypeImpl = NoPadding32Bit;

  static constexpr std::size_t kExpectedPaddingLocationsCountImpl = 0;
  static constexpr std::array<std::size_t, 0>
      kExpectedPaddingLocationsPaddingByteCountImpl{};
  static constexpr std::array<std::size_t, 0> kExpectedPaddingByteIndexesImpl{};
};

template <>
class ExpectedData<
    test_data::data_models::Simple32BitButIntsNotSelfAlignedDataModel>
    : public IExpectedData<ExpectedData<
          test_data::data_models::Simple32BitButIntsNotSelfAlignedDataModel>> {
 public:
  using DataModelTypeImpl =
      test_data::data_models::Simple32BitButIntsNotSelfAlignedDataModel;
  using SerializedTypeImpl = NoPadding32Bit;

  static constexpr std::size_t kExpectedPaddingLocationsCountImpl = 0;
  static constexpr std::array<std::size_t, 0>
      kExpectedPaddingLocationsPaddingByteCountImpl{};
  static constexpr std::array<std::size_t, 0> kExpectedPaddingByteIndexesImpl{};
};

}  // namespace struct_no_padding_32bit
}  // namespace test_data

#endif  // EXPECTED_DATA_NO_PADDING_32BIT_STRUCT_H
