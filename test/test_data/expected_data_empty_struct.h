#ifndef EXPECTED_DATA_EMPTY_STRUCT_H
#define EXPECTED_DATA_EMPTY_STRUCT_H

#include "interface_expected_data.h"
#include "tested_data_models.h"

namespace test_data {
namespace struct_empty {

struct Empty {};

template <typename DataModelT>
class ExpectedData : public IExpectedData<ExpectedData<DataModelT>> {};

template <>
class ExpectedData<test_data::data_models::Simple32BitDataModel>
    : public IExpectedData<
          ExpectedData<test_data::data_models::Simple32BitDataModel>> {
 public:
  using DataModelTypeImpl = test_data::data_models::Simple32BitDataModel;
  using SerializedTypeImpl = Empty;

  static constexpr std::size_t kExpectedPaddingLocationsCountImpl = 0;
  static constexpr std::array<std::size_t, 0>
      kExpectedPaddingLocationsByteCountsImpl{};
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
  using SerializedTypeImpl = Empty;

  static constexpr std::size_t kExpectedPaddingLocationsCountImpl = 0;
  static constexpr std::array<std::size_t, 0>
      kExpectedPaddingLocationsByteCountsImpl{};
  static constexpr std::array<std::size_t, 0> kExpectedPaddingByteIndexesImpl{};
};

}  // namespace struct_empty
}  // namespace test_data

#endif  // EXPECTED_DATA_EMPTY_STRUCT_H
