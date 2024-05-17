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
      kExpectedPaddingLocationsByteCountsImpl{3};
  static constexpr std::array<std::size_t, 3> kExpectedPaddingByteIndexesImpl{
      5, 6, 7};
};

template <>
class ExpectedData<test_data::data_models::NoPaddingDataModel>
    : public IExpectedData<
          ExpectedData<test_data::data_models::NoPaddingDataModel>> {
 public:
  using DataModelTypeImpl = test_data::data_models::NoPaddingDataModel;
  using SerializedTypeImpl = IntCharStruct;

  static constexpr std::size_t kExpectedPaddingLocationsCountImpl = 0;
  static constexpr std::array<std::size_t, 0>
      kExpectedPaddingLocationsByteCountsImpl{};
  static constexpr std::array<std::size_t, 0> kExpectedPaddingByteIndexesImpl{};
};

}  // namespace struct_int_char
}  // namespace test_data

#endif  // EXPECTED_DATA_INT_CHAR_STRUCT_H
