#ifndef EXPECTED_DATA_INT_CHAR_STRUCT_H
#define EXPECTED_DATA_INT_CHAR_STRUCT_H

#include "interface_expected_data.h"
#include "tested_data_models.h"

namespace test_data {
namespace struct_int_char {

// NOLINTBEGIN (google-runtime-int)
struct IntCharStruct {
  int bar;
  char foo;
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
  using SerializedTypeImpl = IntCharStruct;

  static constexpr std::size_t kExpectedPaddingLocationsCountImpl = 1;
  static constexpr std::array<std::size_t, 1>
      kExpectedPaddingLocationsPaddingByteCountImpl{3};
  static constexpr std::array<std::size_t, 3> kExpectedPaddingByteIndexesImpl{
      5, 6, 7};
};

// This tests trailing padding for a non-self-aligned primitive type. The int
// has an alignment of 2, but size of 4. The char (align 1, size 1) should be
// placed on the 5th byte. Since the largest alignment of the struct is 2, there
// should be 1 byte of trailing padding.
template <>
class ExpectedData<
    test_data::data_models::Simple32BitButIntsNotSelfAlignedDataModel>
    : public IExpectedData<ExpectedData<
          test_data::data_models::Simple32BitButIntsNotSelfAlignedDataModel>> {
 public:
  using DataModelTypeImpl =
      test_data::data_models::Simple32BitButIntsNotSelfAlignedDataModel;
  using SerializedTypeImpl = IntCharStruct;

  static constexpr std::size_t kExpectedPaddingLocationsCountImpl = 1;
  static constexpr std::array<std::size_t, 1>
      kExpectedPaddingLocationsPaddingByteCountImpl{1};
  static constexpr std::array<std::size_t, 1> kExpectedPaddingByteIndexesImpl{
      5};
};

}  // namespace struct_int_char
}  // namespace test_data

#endif  // EXPECTED_DATA_INT_CHAR_STRUCT_H
