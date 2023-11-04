// ABOUT:
//
// Why `static_assert` instead of gtest asserts?
// - static_assert enforces compile-time evaluation and correctness
// - Cons:
//   - breaking tests are really annoying to debug since its not clear
//     why the test failed; gtest's asserts prints the actual and expected
//     value
//
// It makes sense to encapsulate the static asserts within gtest; they are tests
// after all, but they happen to be run at compile-time. Futhermore, they should
// only be run iff the build target is testing.

#include "include/little_pp.h"

#include <gtest/gtest.h>

#include "little_pp_test_data.hpp"

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
    test_data::struct_empty::ExpectedData<test_data::Simple32BitDataModel>,
    test_data::struct_empty::ExpectedData<
        test_data::Simple32BitButIntsNotSelfAlignedDataModel>,
    test_data::struct_no_padding_32bit::ExpectedData<
        test_data::Simple32BitDataModel>,
    test_data::struct_no_padding_32bit::ExpectedData<
        test_data::Simple32BitButIntsNotSelfAlignedDataModel>,
    test_data::struct_internal_padding_3_32bit::ExpectedData<
        test_data::Simple32BitDataModel>,
    test_data::struct_internal_padding_3_32bit::ExpectedData<
        test_data::Simple32BitButIntsNotSelfAlignedDataModel> 
>;

TYPED_TEST_SUITE(PaddingReflectionTest, DataModelImplementations);

TYPED_TEST(PaddingReflectionTest, ReturnsExpectedPaddingLocationsCount) {
  static_assert(
      TestFixture::LittlePPPadding::kPaddingLocationsCount ==
          TestFixture::ExpectedData::get_expected_padding_locations_count(),
      "Padding locations count did not match expected.");
}

TYPED_TEST(PaddingReflectionTest,
           ReturnsExpectedPaddingLocationsPaddingByteCount) {
  static_assert(
      TestFixture::LittlePPPadding::kPaddingLocationsPaddingByteCount ==
          TestFixture::ExpectedData::
              get_expected_padding_locations_padding_byte_count(),
      "Padding locations padding-byte count did not match expected.");
}

// TYPED_TEST(PaddingReflectionTest, ReturnsExpectedPaddingByteIndexes) {
//   static_assert(
//       TestFixture::LittlePPPadding::kPaddingByteIndexes ==
//           TestFixture::ExpectedData::get_expected_padding_byte_indexes(),
//       "Padding indexes did not match expected.");
// }

//   struct OnlyInternalPadding3_DifferentStructAlignment {
//     unsigned char foo;
//     // unsigned char padding[3];
//     unsigned int bar;
//     unsigned long buzz;
//   };
//   static constexpr std::array<std::size_t, 1>
//       expected_padding_locations_padding_byte_count_OnlyInternalPadding3_DifferentStructAlignment{
//           3};
//   static constexpr std::array<std::size_t, 3>
//       expected_padding_indexes_OnlyInternalPadding3_DifferentStructAlignment{
//           1, 2, 3};
//
//   struct OnlyInternalPadding1_Simple {
//     unsigned char foo;
//     // unsigned char padding[1];
//     unsigned short bar;
//   };
//   static constexpr std::array<std::size_t, 1>
//       expected_padding_locations_padding_byte_count_OnlyInternalPadding1_Simple{
//           1};
//   static constexpr std::array<std::size_t, 1>
//       expected_padding_indexes_OnlyInternalPadding1_Simple{1};
//
//   struct OnlyInternalPadding1_DifferentStructAlignment {
//     unsigned char foo;
//     // unsigned char padding[1];
//     unsigned short bar;
//     unsigned int buzz;
//   };
//   static constexpr std::array<std::size_t, 1>
//       expected_padding_locations_padding_byte_count_OnlyInternalPadding1_DifferentStructAlignment{
//           1};
//   static constexpr std::array<std::size_t, 1>
//       expected_padding_indexes_OnlyInternalPadding1_DifferentStructAlignment{1};
//
//   struct OnlyTrailingPadding3_Simple {
//     unsigned int bar;
//     unsigned char foo;
//     // unsigned char padding[3];
//   };
//   static constexpr std::array<std::size_t, 1>
//       expected_padding_locations_padding_byte_count_OnlyTrailingPadding3_Simple{
//           3};
//   static constexpr std::array<std::size_t, 3>
//       expected_padding_indexes_OnlyTrailingPadding3_Simple{5, 6, 7};
//
//   struct OnlyTrailingPadding3_DifferentStructAlignment {
//     unsigned long buzz;
//     unsigned int bar;
//     unsigned char foo;
//     // unsigned char padding[3];
//   };
//   static constexpr std::array<std::size_t, 1>
//       expected_padding_locations_padding_byte_count_OnlyTrailingPadding3_DifferentStructAlignment{
//           3};
//   static constexpr std::array<std::size_t, 3>
//       expected_padding_indexes_OnlyTrailingPadding3_DifferentStructAlignment{
//           13, 14, 15};
//
//   struct ArbitraryInternalMultiPadding1Then2 {
//     char foo;
//     // unsigned char padding[1];
//     short bar;
//     float a;
//     short b;
//     // unsigned char padding[2];
//     int c;
//   };
//   static constexpr std::array<std::size_t, 2>
//       expected_padding_locations_padding_byte_count_ArbitraryInternalMultiPadding1Then2{
//           1, 2};
//   static constexpr std::array<std::size_t, 3>
//       expected_padding_indexes_ArbitraryInternalMultiPadding1Then2{1, 10,
//       11};
//
//   struct ArbitraryMixedPaddings1Then2Then3 {
//     char foo;
//     // unsigned char padding[1];
//     short bar;
//     float a;
//     short b;
//     // unsigned char padding[2];
//     int c;
//     unsigned char d;
//     // unsigned char padding[3];
//   };
//   static constexpr std::array<std::size_t, 3>
//       expected_padding_locations_padding_byte_count_ArbitraryMixedPaddings1Then2Then3{
//           1, 2, 3};
//   static constexpr std::array<std::size_t, 6>
//       expected_padding_indexes_ArbitraryMixedPaddings1Then2Then3{1,  10,
//       11,
//                                                                  17, 18,
//                                                                  19};
//
//   struct ArbitraryMixedPaddingOneDataMemberBetween {
//     long bar;
//     char baz;
//     // unsigned char padding[1]
//     short foo;
//     // unsigned char padding[4]
//   };
//   static constexpr std::array<std::size_t, 2>
//       expected_padding_locations_padding_byte_count_ArbitraryMixedPaddingOneDataMemberBetween{
//           1, 4};
//   static constexpr std::array<std::size_t, 5>
//       expected_padding_indexes_ArbitraryMixedPaddingOneDataMemberBetween{
//           9, 12, 13, 14, 15};
//
//   // A data model in which 32-bit ints can be aligned to 16-bit words
//   using Simple32BitDatModelButIntsNotSelfAligned =
//       little_pp::DataModel<1, 1, 1, 1, 1, 1, 2, 2,
//
//                            2, 2, 2, 2,
//
//                            4, 2, 4, 2,
//
//                            8, 8, 8, 8,
//
//                            8, 8, 8, 8,
//
//                            4, 4, 8, 8, 8, 8,
//
//                            1, 1>;
//   struct ForNotSelfAligned_ArbitraryMixedPaddings1Then3 {
//     char foo;
//     // unsigned char padding[1];
//     short bar;
//     float a;
//     short b;
//     int c;
//     unsigned char d;  // 15 bytes from beginning to here
//                       // unsigned char padding[1];
//   };
//   static constexpr std::array<std::size_t, 2>
//       expected_padding_locations_padding_byte_count_ForNotSelfAligned_ArbitraryMixedPaddings1Then3{
//           1, 1};
//   static constexpr std::array<std::size_t, 2>
//       expected_padding_indexes_ForNotSelfAligned_ArbitraryMixedPaddings1Then3{
//           1, 15};
//
//   // TODO:
//   // - test array data members
//   // - test nested struct/class
//
//   // Test padding locations count
//   static_assert(SerializableClassPaddingIndexes<
//                     NoPadding,
//                     Simple32BitDataModel>::kPaddingLocationsCount
//                     == 0,
//                 "Struct should have specified padding.");
//   static_assert(
//       SerializableClassPaddingIndexes<
//           NoPadding,
//           Simple32BitDatModelButIntsNotSelfAligned>::kPaddingLocationsCount
//           == 0,
//       "Struct should have specified padding.");
//
//   static_assert(SerializableClassPaddingIndexes<
//                     OnlyInternalPadding3_Simple,
//                     Simple32BitDataModel>::kPaddingLocationsCount == 1,
//                 "Struct should have specified padding.");
//   static_assert(SerializableClassPaddingIndexes<
//                     OnlyInternalPadding3_DifferentStructAlignment,
//                     Simple32BitDataModel>::kPaddingLocationsCount == 1,
//                 "Struct should have specified padding.");
//
//   static_assert(SerializableClassPaddingIndexes<
//                     OnlyInternalPadding1_Simple,
//                     Simple32BitDataModel>::kPaddingLocationsCount == 1,
//                 "Struct should have specified padding.");
//   static_assert(SerializableClassPaddingIndexes<
//                     OnlyInternalPadding1_DifferentStructAlignment,
//                     Simple32BitDataModel>::kPaddingLocationsCount == 1,
//                 "Struct should have specified padding.");
//
//   static_assert(SerializableClassPaddingIndexes<
//                     OnlyTrailingPadding3_Simple,
//                     Simple32BitDataModel>::kPaddingLocationsCount == 1,
//                 "Struct should have specified padding.");
//   static_assert(SerializableClassPaddingIndexes<
//                     OnlyTrailingPadding3_DifferentStructAlignment,
//                     Simple32BitDataModel>::kPaddingLocationsCount == 1,
//                 "Struct should have specified padding.");
//
//   static_assert(SerializableClassPaddingIndexes<
//                     ArbitraryInternalMultiPadding1Then2,
//                     Simple32BitDataModel>::kPaddingLocationsCount == 2,
//                 "Struct should have specified padding.");
//   static_assert(SerializableClassPaddingIndexes<
//                     ArbitraryMixedPaddings1Then2Then3,
//                     Simple32BitDataModel>::kPaddingLocationsCount == 3,
//                 "Struct should have specified padding.");
//   static_assert(SerializableClassPaddingIndexes<
//                     ArbitraryMixedPaddingOneDataMemberBetween,
//                     Simple32BitDataModel>::kPaddingLocationsCount == 2,
//                 "Struct should have specified padding.");
//
//   static_assert(
//       SerializableClassPaddingIndexes<
//           ForNotSelfAligned_ArbitraryMixedPaddings1Then3,
//           Simple32BitDatModelButIntsNotSelfAligned>::kPaddingLocationsCount
//           == 2,
//       "Struct should have specified padding.");
//
//   // Test padding locations padding bytes count
//   static_assert(
//       SerializableClassPaddingIndexes<
//           NoPadding,
//           Simple32BitDataModel>::kPaddingLocationsPaddingByteCount
//           == expected_padding_locations_padding_byte_count_NoPadding,
//       "Struct should have specified padding.");
//
//   static_assert(
//       SerializableClassPaddingIndexes<
//           OnlyInternalPadding3_Simple,
//           Simple32BitDataModel>::kPaddingLocationsPaddingByteCount ==
//           expected_padding_locations_padding_byte_count_OnlyInternalPadding3_Simple,
//       "Struct should have specified padding.");
//   static_assert(
//       SerializableClassPaddingIndexes<
//           OnlyInternalPadding3_DifferentStructAlignment,
//           Simple32BitDataModel>::kPaddingLocationsPaddingByteCount ==
//           expected_padding_locations_padding_byte_count_OnlyInternalPadding3_DifferentStructAlignment,
//       "Struct should have specified padding.");
//
//   static_assert(
//       SerializableClassPaddingIndexes<
//           OnlyInternalPadding1_Simple,
//           Simple32BitDataModel>::kPaddingLocationsPaddingByteCount ==
//           expected_padding_locations_padding_byte_count_OnlyInternalPadding1_Simple,
//       "Struct should have specified padding.");
//   static_assert(
//       SerializableClassPaddingIndexes<
//           OnlyInternalPadding1_DifferentStructAlignment,
//           Simple32BitDataModel>::kPaddingLocationsPaddingByteCount ==
//           expected_padding_locations_padding_byte_count_OnlyInternalPadding1_DifferentStructAlignment,
//       "Struct should have specified padding.");
//
//   static_assert(
//       SerializableClassPaddingIndexes<
//           OnlyTrailingPadding3_Simple,
//           Simple32BitDataModel>::kPaddingLocationsPaddingByteCount ==
//           expected_padding_locations_padding_byte_count_OnlyTrailingPadding3_Simple,
//       "Struct should have specified padding.");
//   static_assert(
//       SerializableClassPaddingIndexes<
//           OnlyTrailingPadding3_DifferentStructAlignment,
//           Simple32BitDataModel>::kPaddingLocationsPaddingByteCount ==
//           expected_padding_locations_padding_byte_count_OnlyTrailingPadding3_DifferentStructAlignment,
//       "Struct should have specified padding.");
//
//   static_assert(
//       SerializableClassPaddingIndexes<
//           ArbitraryInternalMultiPadding1Then2,
//           Simple32BitDataModel>::kPaddingLocationsPaddingByteCount ==
//           expected_padding_locations_padding_byte_count_ArbitraryInternalMultiPadding1Then2,
//       "Struct should have specified padding.");
//   static_assert(
//       SerializableClassPaddingIndexes<
//           ArbitraryMixedPaddings1Then2Then3,
//           Simple32BitDataModel>::kPaddingLocationsPaddingByteCount ==
//           expected_padding_locations_padding_byte_count_ArbitraryMixedPaddings1Then2Then3,
//       "Struct should have specified padding.");
//   static_assert(
//       SerializableClassPaddingIndexes<
//           ArbitraryMixedPaddingOneDataMemberBetween,
//           Simple32BitDataModel>::kPaddingLocationsPaddingByteCount ==
//           expected_padding_locations_padding_byte_count_ArbitraryMixedPaddingOneDataMemberBetween,
//       "Struct should have specified padding.");
//
//   static_assert(
//       SerializableClassPaddingIndexes<
//           ForNotSelfAligned_ArbitraryMixedPaddings1Then3,
//           Simple32BitDatModelButIntsNotSelfAligned>::
//               kPaddingLocationsPaddingByteCount ==
//           expected_padding_locations_padding_byte_count_ForNotSelfAligned_ArbitraryMixedPaddings1Then3,
//       "Struct should have specified padding.");
//
//   // Test padding byte count
//   static_assert(SerializableClassPaddingIndexes<
//                     NoPadding, Simple32BitDataModel>::kPaddingBytesCount
//                     == 0,
//                 "Struct should have specified padding.");
//
//   static_assert(SerializableClassPaddingIndexes<
//                     OnlyInternalPadding3_Simple,
//                     Simple32BitDataModel>::kPaddingBytesCount == 3,
//                 "Struct should have specified padding.");
//   static_assert(SerializableClassPaddingIndexes<
//                     OnlyInternalPadding3_DifferentStructAlignment,
//                     Simple32BitDataModel>::kPaddingBytesCount == 3,
//                 "Struct should have specified padding.");
//
//   static_assert(SerializableClassPaddingIndexes<
//                     OnlyInternalPadding1_Simple,
//                     Simple32BitDataModel>::kPaddingBytesCount == 1,
//                 "Struct should have specified padding.");
//   static_assert(SerializableClassPaddingIndexes<
//                     OnlyInternalPadding1_DifferentStructAlignment,
//                     Simple32BitDataModel>::kPaddingBytesCount == 1,
//                 "Struct should have specified padding.");
//
//   static_assert(SerializableClassPaddingIndexes<
//                     OnlyTrailingPadding3_Simple,
//                     Simple32BitDataModel>::kPaddingBytesCount == 3,
//                 "Struct should have specified padding.");
//   static_assert(SerializableClassPaddingIndexes<
//                     OnlyTrailingPadding3_DifferentStructAlignment,
//                     Simple32BitDataModel>::kPaddingBytesCount == 3,
//                 "Struct should have specified padding.");
//
//   static_assert(SerializableClassPaddingIndexes<
//                     ArbitraryInternalMultiPadding1Then2,
//                     Simple32BitDataModel>::kPaddingBytesCount == 3,
//                 "Struct should have specified padding.");
//   static_assert(SerializableClassPaddingIndexes<
//                     ArbitraryMixedPaddings1Then2Then3,
//                     Simple32BitDataModel>::kPaddingBytesCount == 6,
//                 "Struct should have specified padding.");
//   static_assert(SerializableClassPaddingIndexes<
//                     ArbitraryMixedPaddingOneDataMemberBetween,
//                     Simple32BitDataModel>::kPaddingBytesCount == 5,
//                 "Struct should have specified padding.");
//
//   static_assert(
//       SerializableClassPaddingIndexes<
//           ForNotSelfAligned_ArbitraryMixedPaddings1Then3,
//           Simple32BitDatModelButIntsNotSelfAligned>::kPaddingBytesCount
//           == 2,
//       "Struct should have specified padding.");
//
//   // Test padding indexes method
//   static_assert(SerializableClassPaddingIndexes<
//                     NoPadding, Simple32BitDataModel>::kPaddingByteIndexes
//                     == expected_padding_indexes_NoPadding,
//                 "Struct should have specified padding.");
//
//   static_assert(SerializableClassPaddingIndexes<
//                     OnlyInternalPadding3_Simple,
//                     Simple32BitDataModel>::kPaddingByteIndexes ==
//                     expected_padding_indexes_OnlyInternalPadding3_Simple,
//                 "Struct should have specified padding.");
//   static_assert(
//       SerializableClassPaddingIndexes<
//           OnlyInternalPadding3_DifferentStructAlignment,
//           Simple32BitDataModel>::kPaddingByteIndexes ==
//           expected_padding_indexes_OnlyInternalPadding3_DifferentStructAlignment,
//       "Struct should have specified padding.");
//
//   static_assert(SerializableClassPaddingIndexes<
//                     OnlyInternalPadding1_Simple,
//                     Simple32BitDataModel>::kPaddingByteIndexes ==
//                     expected_padding_indexes_OnlyInternalPadding1_Simple,
//                 "Struct should have specified padding.");
//   static_assert(
//       SerializableClassPaddingIndexes<
//           OnlyInternalPadding1_DifferentStructAlignment,
//           Simple32BitDataModel>::kPaddingByteIndexes ==
//           expected_padding_indexes_OnlyInternalPadding1_DifferentStructAlignment,
//       "Struct should have specified padding.");
//
//   static_assert(SerializableClassPaddingIndexes<
//                     OnlyTrailingPadding3_Simple,
//                     Simple32BitDataModel>::kPaddingByteIndexes ==
//                     expected_padding_indexes_OnlyTrailingPadding3_Simple,
//                 "Struct should have specified padding.");
//   static_assert(
//       SerializableClassPaddingIndexes<
//           OnlyTrailingPadding3_DifferentStructAlignment,
//           Simple32BitDataModel>::kPaddingByteIndexes ==
//           expected_padding_indexes_OnlyTrailingPadding3_DifferentStructAlignment,
//       "Struct should have specified padding.");
//
//   static_assert(
//       SerializableClassPaddingIndexes<
//           ArbitraryInternalMultiPadding1Then2,
//           Simple32BitDataModel>::kPaddingByteIndexes ==
//           expected_padding_indexes_ArbitraryInternalMultiPadding1Then2,
//       "Struct should have specified padding.");
//   static_assert(SerializableClassPaddingIndexes<
//                     ArbitraryMixedPaddings1Then2Then3,
//                     Simple32BitDataModel>::kPaddingByteIndexes ==
//                     expected_padding_indexes_ArbitraryMixedPaddings1Then2Then3,
//                 "Struct should have specified padding.");
//   static_assert(
//       SerializableClassPaddingIndexes<
//           ArbitraryMixedPaddingOneDataMemberBetween,
//           Simple32BitDataModel>::kPaddingByteIndexes ==
//           expected_padding_indexes_ArbitraryMixedPaddingOneDataMemberBetween,
//       "Struct should have specified padding.");
//
//   static_assert(
//       SerializableClassPaddingIndexes<
//           ForNotSelfAligned_ArbitraryMixedPaddings1Then3,
//           Simple32BitDatModelButIntsNotSelfAligned>::kPaddingByteIndexes
//           ==
//           expected_padding_indexes_ForNotSelfAligned_ArbitraryMixedPaddings1Then3,
//       "Struct should have specified padding.");
// };
