#ifndef EXPECTED_DATA_INTERNAL_PADDING_3_32BIT_H
#define EXPECTED_DATA_INTERNAL_PADDING_3_32BIT_H

#include "interface_expected_data.hpp"
#include "tested_data_models.h"

namespace test_data {
namespace struct_internal_padding_3_32bit {

// NOLINTBEGIN (google-runtime-int)
struct InternalPadding3 {
  unsigned char foo;
  unsigned int bar;
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
  using SerializedTypeImpl = InternalPadding3;

  static constexpr std::size_t kExpectedPaddingLocationsCountImpl = 1;
  static constexpr std::array<std::size_t, 1>
      kExpectedPaddingLocationsPaddingByteCountImpl{3};
  static constexpr std::array<std::size_t, 3> kExpectedPaddingByteIndexesImpl{
      1, 2, 3};
};

template <>
class ExpectedData<
    test_data::data_models::Simple32BitButIntsNotSelfAlignedDataModel>
    : public IExpectedData<ExpectedData<
          test_data::data_models::Simple32BitButIntsNotSelfAlignedDataModel>> {
 public:
  using DataModelTypeImpl =
      test_data::data_models::Simple32BitButIntsNotSelfAlignedDataModel;
  using SerializedTypeImpl = InternalPadding3;

  static constexpr std::size_t kExpectedPaddingLocationsCountImpl = 1;
  static constexpr std::array<std::size_t, 1>
      kExpectedPaddingLocationsPaddingByteCountImpl{1};
  static constexpr std::array<std::size_t, 1> kExpectedPaddingByteIndexesImpl{
      1};
};

}  // namespace struct_internal_padding_3_32bit
}  // namespace test_data

#endif  // EXPECTED_DATA_INTERNAL_PADDING_3_32BIT_H
