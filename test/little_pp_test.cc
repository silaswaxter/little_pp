// ABOUT: This set of test implementations is not optimal, but i tried to make
//        it okay. The following description should help you get your bearings
//        if you have to make changes.
//
//        Here, I'll briefly explain how everything is hooked up. First off, we
//        have a template class which inherits from gtest's test; its a testing
//        framework that is templated--i.e. parameterized--on whatever the type
//        under test is. Next, we declare a collection of types we want to test.
//        The type under test is the structure to "analyze/serialize"; its
//        templated on the data model, so in this list the type under test is
//        declared for each data model it has defined expectations for. Lastly,
//        typed test suites are declared in which the templated framework and
//        list of types to test are passed; then within, a static_assert checks
//        that the actual value matches the expected value. Note that a gtest
//        with no gtest assertions will pass by default.
//
// FAQ:
//
// Why `static_assert` instead of gtest asserts?
// - static_assert enforces compile-time evaluation and correctness
// - Cons:
//   - breaking tests are really annoying to debug since its not clear
//     why the test failed; gtest's asserts prints the actual and expected
//     value. However, tests include commented-out code which will print the
//     received and expected value, respectively.
//
// Okay, then why use gtest at all?
// - gtest is one of the most popular testing frameworks for c++. I'm not
//   familiar with a better alternative for a template-meta-programming c++
//   project like this except for boost's framework, but I wanted to stay away
//   from boost.
// - It makes sense to encapsulate the static asserts within a test
//   framework; they are tests after all--they just happen to be "run" at
//   compile-time. Futhermore, they should only be run iff the build target is
//   testing.
// - Gtest's parameterized typed tests are useful in succinctly declaring the
//   actual test; then we define the test data elsewhere and inject it.

#include "include/little_pp.h"

#include <gtest/gtest.h>

#include "std_array_comparison_operators.h"
#include "test_data/expected_data_char_short_int_char_struct.h"
#include "test_data/expected_data_char_short_int_struct.h"
#include "test_data/expected_data_empty_struct.h"
#include "test_data/expected_data_int_char.h"
#include "test_data/expected_data_short_uchar_char_uint_struct.h"
#include "test_data/interface_expected_data.h"

#define UNCOMMENT_TO_FAIL_AND_PRINT ADD_FAILURE

template <class T>
class PaddingReflectionTest : public testing::Test {
 protected:
  using ExpectedData = test_data::IExpectedData<T>;
  using DataModelType =
      typename test_data::IExpectedData<T>::Parameters::DataModelType;
  using SerializedType =
      typename test_data::IExpectedData<T>::Parameters::SerializedType;
  using LittlePPPadding =
      SerializableClassPaddingIndexes<SerializedType, DataModelType>;
};

// The list of types we want to test.
using DataModelImplementations = testing::Types<
    test_data::struct_empty::ExpectedData<
        test_data::data_models::Simple32BitDataModel>,
    test_data::struct_empty::ExpectedData<
        test_data::data_models::Simple32BitButIntsNotSelfAlignedDataModel>,
    test_data::struct_short_uchar_char_uint::ExpectedData<
        test_data::data_models::Simple32BitDataModel>,
    test_data::struct_short_uchar_char_uint::ExpectedData<
        test_data::data_models::Simple32BitButIntsNotSelfAlignedDataModel>,
    test_data::struct_char_int_long::ExpectedData<
        test_data::data_models::Simple32BitDataModel>,
    test_data::struct_char_int_long::ExpectedData<
        test_data::data_models::Simple32BitButIntsNotSelfAlignedDataModel>,
    test_data::struct_int_char::ExpectedData<
        test_data::data_models::Simple32BitDataModel>,
    test_data::struct_int_char::ExpectedData<
        test_data::data_models::Simple32BitButIntsNotSelfAlignedDataModel>,
    test_data::struct_char_short_int_char::ExpectedData<
        test_data::data_models::Simple32BitDataModel>,
    test_data::struct_char_short_int_char::ExpectedData<
        test_data::data_models::Simple32BitButIntsNotSelfAlignedDataModel>
    // clang-format off
>;
// clang-format on

TYPED_TEST_SUITE(PaddingReflectionTest, DataModelImplementations);

TYPED_TEST(PaddingReflectionTest, ReturnsExpectedPaddingLocationsCount) {
  constexpr auto kGot = TestFixture::LittlePPPadding::kPaddingLocationsCount;
  constexpr auto kExpected =
      TestFixture::ExpectedData::get_expected_padding_locations_count();

  // clang-format off
  // UNCOMMENT_TO_FAIL_AND_PRINT() << "Got:      " << testing::PrintToString(kGot);
  // UNCOMMENT_TO_FAIL_AND_PRINT() << "Expected: " << testing::PrintToString(kExpected);
  // clang-format on

  static_assert(kGot == kExpected,
                "Padding locations count did not match expected.");
}

TYPED_TEST(PaddingReflectionTest,
           ReturnsExpectedPaddingLocationsPaddingByteCount) {
  constexpr auto kGot =
      TestFixture::LittlePPPadding::kPaddingLocationsPaddingByteCount;
  constexpr auto kExpected = TestFixture::ExpectedData::
      get_expected_padding_locations_padding_byte_count();

  // clang-format off
  // UNCOMMENT_TO_FAIL_AND_PRINT() << "Got:      " << testing::PrintToString(kGot);
  // UNCOMMENT_TO_FAIL_AND_PRINT() << "Expected: " << testing::PrintToString(kExpected);
  // clang-format on

  static_assert(kGot == kExpected,
                "Padding locations padding-byte count did not match expected.");
}

TYPED_TEST(PaddingReflectionTest, ReturnsExpectedPaddingByteIndexes) {
  constexpr auto kGot = TestFixture::LittlePPPadding::kPaddingByteIndexes;
  constexpr auto kExpected =
      TestFixture::ExpectedData::get_expected_padding_byte_indexes();

  // clang-format off
  // UNCOMMENT_TO_FAIL_AND_PRINT() << "Got:      " << testing::PrintToString(kGot);
  // UNCOMMENT_TO_FAIL_AND_PRINT() << "Expected: " << testing::PrintToString(kExpected);
  // clang-format on

  static_assert(kGot == kExpected, "Padding indexes did not match expected.");
}

// TODO:
// - test array data members
// - test nested struct/class
