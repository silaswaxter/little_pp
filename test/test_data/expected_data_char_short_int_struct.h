#ifndef EXPECTED_DATA_CHAR_INT_LONG_STRUCT_H
#define EXPECTED_DATA_CHAR_INT_LONG_STRUCT_H

#include "interface_expected_data.h"
#include "tested_data_models.h"

namespace test_data {
namespace struct_char_int_long {

// NOLINTBEGIN (google-runtime-int)
struct CharIntLongStruct {
  char foo;
  int bar;
  long buzz;
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
  using SerializedTypeImpl = CharIntLongStruct;

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
  using SerializedTypeImpl = CharIntLongStruct;

  static constexpr std::size_t kExpectedPaddingLocationsCountImpl = 2;
  static constexpr std::array<std::size_t, 2>
      kExpectedPaddingLocationsPaddingByteCountImpl{1, 2};
  static constexpr std::array<std::size_t, 3> kExpectedPaddingByteIndexesImpl{
      1, 6, 7};
};

}  // namespace struct_char_int_long
}  // namespace test_data

#endif  //  EXPECTED_DATA_CHAR_INT_LONG_STRUCT_H
