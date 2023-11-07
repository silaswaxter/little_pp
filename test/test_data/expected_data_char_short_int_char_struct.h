#ifndef EXPECTED_DATA_CHAR_SHORT_INT_CHAR_STRUCT_H
#define EXPECTED_DATA_CHAR_SHORT_INT_CHAR_STRUCT_H

#include "interface_expected_data.h"
#include "tested_data_models.h"

namespace test_data {
namespace struct_char_short_int_char {

// NOLINTBEGIN (google-runtime-int)
struct CharShortIntCharStruct {
  char bar;
  short foo;
  int baz;
  char buzz;
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
  using SerializedTypeImpl = CharShortIntCharStruct;

  static constexpr std::size_t kExpectedPaddingLocationsCountImpl = 2;
  static constexpr std::array<std::size_t, 2>
      kExpectedPaddingLocationsByteCountsImpl{1, 3};
  static constexpr std::array<std::size_t, 4> kExpectedPaddingByteIndexesImpl{
      1, 9, 10, 11};
};

template <>
class ExpectedData<
    test_data::data_models::Simple32BitButIntsNotSelfAlignedDataModel>
    : public IExpectedData<ExpectedData<
          test_data::data_models::Simple32BitButIntsNotSelfAlignedDataModel>> {
 public:
  using DataModelTypeImpl =
      test_data::data_models::Simple32BitButIntsNotSelfAlignedDataModel;
  using SerializedTypeImpl = CharShortIntCharStruct;

  static constexpr std::size_t kExpectedPaddingLocationsCountImpl = 2;
  static constexpr std::array<std::size_t, 2>
      kExpectedPaddingLocationsByteCountsImpl{1, 1};
  static constexpr std::array<std::size_t, 2> kExpectedPaddingByteIndexesImpl{
      1, 9};
};

}  // namespace struct_char_short_int_char
}  // namespace test_data

#endif  //  EXPECTED_DATA_CHAR_SHORT_INT_CHAR_STRUCT_H
