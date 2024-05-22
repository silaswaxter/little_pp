#include <gtest/gtest.h>

#include "include/little_pp.h"
#include "test_data/tested_data_models.h"

// alias gtest macro so that its clear why there is commented-out code
#define UNCOMMENT_TO_FAIL_AND_PRINT ADD_FAILURE

TEST(UniquePadding, Std32BitDataModelToNoPaddingDataModel) {
  struct TestedStructType {
    char a;
    int b;
    char c;
    char d;
  };
  using TestedFirstDataModel = test_data::data_models::Simple32BitDataModel;
  using TestedSecondDataModel = test_data::data_models::NoPaddingDataModel;

  constexpr std::array<little_pp::unique_padding_element::UniquePaddingElement,
                       2>
      kExpected = {
          little_pp::unique_padding_element::UniquePaddingElement{1, 3, true},
          little_pp::unique_padding_element::UniquePaddingElement{10, 2, true},
      };

  auto constexpr kGot = little_pp::padding_reflection::unique_padding_v<
      TestedStructType, TestedFirstDataModel, TestedSecondDataModel>;

  EXPECT_EQ(kExpected.size(), kGot.size());
  for (std::size_t i = 0; i < kExpected.size(); i++) {
    EXPECT_EQ(kExpected[i].padding_first_index, kGot[i].padding_first_index)
        << "i = " << i;
    EXPECT_EQ(kExpected[i].padding_span_length, kGot[i].padding_span_length)
        << "i = " << i;
    EXPECT_EQ(kExpected[i].is_padding_for_first_passed_data_model,
              kGot[i].is_padding_for_first_passed_data_model)
        << "i = " << i;
  }
}

TEST(UniquePadding, NoPaddingDataModelToStd32BitDataModel) {
  struct TestedStructType {
    char a;
    int b;
    char c;
    char d;
  };
  using TestedFirstDataModel = test_data::data_models::NoPaddingDataModel;
  using TestedSecondDataModel = test_data::data_models::Simple32BitDataModel;

  constexpr std::array<little_pp::unique_padding_element::UniquePaddingElement,
                       2>
      kExpected = {
          little_pp::unique_padding_element::UniquePaddingElement{1, 3, false},
          little_pp::unique_padding_element::UniquePaddingElement{10, 2, false},
      };

  auto constexpr kGot = little_pp::padding_reflection::unique_padding_v<
      TestedStructType, TestedFirstDataModel, TestedSecondDataModel>;

  EXPECT_EQ(kExpected.size(), kGot.size());
  for (std::size_t i = 0; i < kExpected.size(); i++) {
    EXPECT_EQ(kExpected[i].padding_first_index, kGot[i].padding_first_index)
        << "i = " << i;
    EXPECT_EQ(kExpected[i].padding_span_length, kGot[i].padding_span_length)
        << "i = " << i;
    EXPECT_EQ(kExpected[i].is_padding_for_first_passed_data_model,
              kGot[i].is_padding_for_first_passed_data_model)
        << "i = " << i;
  }
}

TEST(UniquePadding, Std16BitDataModelToStd32BitDataModel) {
  struct TestedStructType {
    uint8_t a;
    int32_t b;
    int8_t c;
    uint8_t d;
  };
  using TestedFirstDataModel = test_data::data_models::Simple16BitDataModel;
  using TestedSecondDataModel = test_data::data_models::Simple32BitDataModel;

  constexpr std::array<little_pp::unique_padding_element::UniquePaddingElement,
                       2>
      kExpected = {
          little_pp::unique_padding_element::UniquePaddingElement{2, 2, false},
          little_pp::unique_padding_element::UniquePaddingElement{10, 2, false},
      };

  auto constexpr kGot = little_pp::padding_reflection::unique_padding_v<
      TestedStructType, TestedFirstDataModel, TestedSecondDataModel>;

  EXPECT_EQ(kExpected.size(), kGot.size());
  for (std::size_t i = 0; i < kExpected.size(); i++) {
    EXPECT_EQ(kExpected[i].padding_first_index, kGot[i].padding_first_index)
        << "i = " << i;
    EXPECT_EQ(kExpected[i].padding_span_length, kGot[i].padding_span_length)
        << "i = " << i;
    EXPECT_EQ(kExpected[i].is_padding_for_first_passed_data_model,
              kGot[i].is_padding_for_first_passed_data_model)
        << "i = " << i;
  }
}
