#include <cstdint>

#include "include/little_pp.h"

struct TEST_SerializableClassPaddingIndexes {
  // IMPORTANT: When defining test structures make sure to define them using the
  // exact primitive types. Do not use fixed-width integers because the aliasing
  //            to the underlying types is platform dependent and can break
  //            these tests.
  using Simple32BitDataModel = little_pp::DataModel<1, 1, 1, 1, 1, 1, 2, 2,

                                                    2, 2, 2, 2,

                                                    4, 4, 4, 4,

                                                    8, 8, 8, 8,

                                                    8, 8, 8, 8,

                                                    4, 4, 8, 8, 8, 8,

                                                    1, 1>;

  struct Empty {};
  static constexpr std::array<std::size_t, 0>
      expected_padding_locations_padding_byte_count_Empty{};
  static constexpr std::array<std::size_t, 0> expected_padding_indexes_Empty{};

  struct NoPadding {
    unsigned short a;
    unsigned char b;
    unsigned char c;
    int d;
  };
  static constexpr std::array<std::size_t, 0>
      expected_padding_locations_padding_byte_count_NoPadding{};
  static constexpr std::array<std::size_t, 0>
      expected_padding_indexes_NoPadding{};

  struct OnlyInternalPadding3_Simple {
    unsigned char foo;
    // unsigned char padding[3];
    unsigned int bar;
  };
  static constexpr std::array<std::size_t, 1>
      expected_padding_locations_padding_byte_count_OnlyInternalPadding3_Simple{
          3};
  static constexpr std::array<std::size_t, 3>
      expected_padding_indexes_OnlyInternalPadding3_Simple{1, 2, 3};

  struct OnlyInternalPadding3_DifferentStructAlignment {
    unsigned char foo;
    // unsigned char padding[3];
    unsigned int bar;
    unsigned long buzz;
  };
  static constexpr std::array<std::size_t, 1>
      expected_padding_locations_padding_byte_count_OnlyInternalPadding3_DifferentStructAlignment{
          3};
  static constexpr std::array<std::size_t, 3>
      expected_padding_indexes_OnlyInternalPadding3_DifferentStructAlignment{
          1, 2, 3};

  struct OnlyInternalPadding1_Simple {
    unsigned char foo;
    // unsigned char padding[1];
    unsigned short bar;
  };
  static constexpr std::array<std::size_t, 1>
      expected_padding_locations_padding_byte_count_OnlyInternalPadding1_Simple{
          1};
  static constexpr std::array<std::size_t, 1>
      expected_padding_indexes_OnlyInternalPadding1_Simple{1};

  struct OnlyInternalPadding1_DifferentStructAlignment {
    unsigned char foo;
    // unsigned char padding[1];
    unsigned short bar;
    unsigned int buzz;
  };
  static constexpr std::array<std::size_t, 1>
      expected_padding_locations_padding_byte_count_OnlyInternalPadding1_DifferentStructAlignment{
          1};
  static constexpr std::array<std::size_t, 1>
      expected_padding_indexes_OnlyInternalPadding1_DifferentStructAlignment{1};

  struct OnlyTrailingPadding3_Simple {
    unsigned int bar;
    unsigned char foo;
    // unsigned char padding[3];
  };
  static constexpr std::array<std::size_t, 1>
      expected_padding_locations_padding_byte_count_OnlyTrailingPadding3_Simple{
          3};
  static constexpr std::array<std::size_t, 3>
      expected_padding_indexes_OnlyTrailingPadding3_Simple{5, 6, 7};

  struct OnlyTrailingPadding3_DifferentStructAlignment {
    unsigned long buzz;
    unsigned int bar;
    unsigned char foo;
    // unsigned char padding[3];
  };
  static constexpr std::array<std::size_t, 1>
      expected_padding_locations_padding_byte_count_OnlyTrailingPadding3_DifferentStructAlignment{
          3};
  static constexpr std::array<std::size_t, 3>
      expected_padding_indexes_OnlyTrailingPadding3_DifferentStructAlignment{
          13, 14, 15};

  struct ArbitraryInternalMultiPadding1Then2 {
    char foo;
    // unsigned char padding[1];
    short bar;
    float a;
    short b;
    // unsigned char padding[2];
    int c;
  };
  static constexpr std::array<std::size_t, 2>
      expected_padding_locations_padding_byte_count_ArbitraryInternalMultiPadding1Then2{
          1, 2};
  static constexpr std::array<std::size_t, 3>
      expected_padding_indexes_ArbitraryInternalMultiPadding1Then2{1, 10, 11};

  struct ArbitraryMixedPaddings1Then2Then3 {
    char foo;
    // unsigned char padding[1];
    short bar;
    float a;
    short b;
    // unsigned char padding[2];
    int c;
    unsigned char d;
    // unsigned char padding[3];
  };
  static constexpr std::array<std::size_t, 3>
      expected_padding_locations_padding_byte_count_ArbitraryMixedPaddings1Then2Then3{
          1, 2, 3};
  static constexpr std::array<std::size_t, 6>
      expected_padding_indexes_ArbitraryMixedPaddings1Then2Then3{1,  10, 11,
                                                                 17, 18, 19};

  struct ArbitraryMixedPaddingOneDataMemberBetween {
    long bar;
    char baz;
    // unsigned char padding[1]
    short foo;
    // unsigned char padding[4]
  };
  static constexpr std::array<std::size_t, 2>
      expected_padding_locations_padding_byte_count_ArbitraryMixedPaddingOneDataMemberBetween{
          1, 4};
  static constexpr std::array<std::size_t, 5>
      expected_padding_indexes_ArbitraryMixedPaddingOneDataMemberBetween{
          9, 12, 13, 14, 15};

  // A data model in which 32-bit ints can be aligned to 16-bit words
  using Simple32BitDatModelButIntsNotSelfAligned =
      little_pp::DataModel<1, 1, 1, 1, 1, 1, 2, 2,

                           2, 2, 2, 2,

                           4, 2, 4, 2,

                           8, 8, 8, 8,

                           8, 8, 8, 8,

                           4, 4, 8, 8, 8, 8,

                           1, 1>;
  struct ForNotSelfAligned_ArbitraryMixedPaddings1Then3 {
    char foo;
    // unsigned char padding[1];
    short bar;
    float a;
    short b;
    int c;
    unsigned char d;  // 15 bytes from beginning to here
                      // unsigned char padding[1];
  };
  static constexpr std::array<std::size_t, 2>
      expected_padding_locations_padding_byte_count_ForNotSelfAligned_ArbitraryMixedPaddings1Then3{
          1, 1};
  static constexpr std::array<std::size_t, 2>
      expected_padding_indexes_ForNotSelfAligned_ArbitraryMixedPaddings1Then3{
          1, 15};

  // TODO:
  // - test array data members
  // - test nested struct/class

  // Test padding locations count
  static_assert(SerializableClassPaddingIndexes<
                    Empty, Simple32BitDataModel>::kPaddingLocationsCount == 0,
                "Empty struct should have no padding.");
  static_assert(
      SerializableClassPaddingIndexes<
          Empty,
          Simple32BitDatModelButIntsNotSelfAligned>::kPaddingLocationsCount ==
          0,
      "Empty struct should have no padding.");
  static_assert(SerializableClassPaddingIndexes<
                    NoPadding, Simple32BitDataModel>::kPaddingLocationsCount ==
                    0,
                "Struct should have specified padding.");
  static_assert(
      SerializableClassPaddingIndexes<
          NoPadding,
          Simple32BitDatModelButIntsNotSelfAligned>::kPaddingLocationsCount ==
          0,
      "Struct should have specified padding.");

  static_assert(SerializableClassPaddingIndexes<
                    OnlyInternalPadding3_Simple,
                    Simple32BitDataModel>::kPaddingLocationsCount == 1,
                "Struct should have specified padding.");
  static_assert(SerializableClassPaddingIndexes<
                    OnlyInternalPadding3_DifferentStructAlignment,
                    Simple32BitDataModel>::kPaddingLocationsCount == 1,
                "Struct should have specified padding.");

  static_assert(SerializableClassPaddingIndexes<
                    OnlyInternalPadding1_Simple,
                    Simple32BitDataModel>::kPaddingLocationsCount == 1,
                "Struct should have specified padding.");
  static_assert(SerializableClassPaddingIndexes<
                    OnlyInternalPadding1_DifferentStructAlignment,
                    Simple32BitDataModel>::kPaddingLocationsCount == 1,
                "Struct should have specified padding.");

  static_assert(SerializableClassPaddingIndexes<
                    OnlyTrailingPadding3_Simple,
                    Simple32BitDataModel>::kPaddingLocationsCount == 1,
                "Struct should have specified padding.");
  static_assert(SerializableClassPaddingIndexes<
                    OnlyTrailingPadding3_DifferentStructAlignment,
                    Simple32BitDataModel>::kPaddingLocationsCount == 1,
                "Struct should have specified padding.");

  static_assert(SerializableClassPaddingIndexes<
                    ArbitraryInternalMultiPadding1Then2,
                    Simple32BitDataModel>::kPaddingLocationsCount == 2,
                "Struct should have specified padding.");
  static_assert(SerializableClassPaddingIndexes<
                    ArbitraryMixedPaddings1Then2Then3,
                    Simple32BitDataModel>::kPaddingLocationsCount == 3,
                "Struct should have specified padding.");
  static_assert(SerializableClassPaddingIndexes<
                    ArbitraryMixedPaddingOneDataMemberBetween,
                    Simple32BitDataModel>::kPaddingLocationsCount == 2,
                "Struct should have specified padding.");

  static_assert(
      SerializableClassPaddingIndexes<
          ForNotSelfAligned_ArbitraryMixedPaddings1Then3,
          Simple32BitDatModelButIntsNotSelfAligned>::kPaddingLocationsCount ==
          2,
      "Struct should have specified padding.");

  // Test padding locations padding bytes count
  static_assert(SerializableClassPaddingIndexes<Empty, Simple32BitDataModel>::
                        kPaddingLocationsPaddingByteCount ==
                    expected_padding_locations_padding_byte_count_Empty,
                "Empty struct should have no padding.");
  static_assert(
      SerializableClassPaddingIndexes<
          NoPadding, Simple32BitDataModel>::kPaddingLocationsPaddingByteCount ==
          expected_padding_locations_padding_byte_count_NoPadding,
      "Struct should have specified padding.");

  static_assert(
      SerializableClassPaddingIndexes<
          OnlyInternalPadding3_Simple,
          Simple32BitDataModel>::kPaddingLocationsPaddingByteCount ==
          expected_padding_locations_padding_byte_count_OnlyInternalPadding3_Simple,
      "Struct should have specified padding.");
  static_assert(
      SerializableClassPaddingIndexes<
          OnlyInternalPadding3_DifferentStructAlignment,
          Simple32BitDataModel>::kPaddingLocationsPaddingByteCount ==
          expected_padding_locations_padding_byte_count_OnlyInternalPadding3_DifferentStructAlignment,
      "Struct should have specified padding.");

  static_assert(
      SerializableClassPaddingIndexes<
          OnlyInternalPadding1_Simple,
          Simple32BitDataModel>::kPaddingLocationsPaddingByteCount ==
          expected_padding_locations_padding_byte_count_OnlyInternalPadding1_Simple,
      "Struct should have specified padding.");
  static_assert(
      SerializableClassPaddingIndexes<
          OnlyInternalPadding1_DifferentStructAlignment,
          Simple32BitDataModel>::kPaddingLocationsPaddingByteCount ==
          expected_padding_locations_padding_byte_count_OnlyInternalPadding1_DifferentStructAlignment,
      "Struct should have specified padding.");

  static_assert(
      SerializableClassPaddingIndexes<
          OnlyTrailingPadding3_Simple,
          Simple32BitDataModel>::kPaddingLocationsPaddingByteCount ==
          expected_padding_locations_padding_byte_count_OnlyTrailingPadding3_Simple,
      "Struct should have specified padding.");
  static_assert(
      SerializableClassPaddingIndexes<
          OnlyTrailingPadding3_DifferentStructAlignment,
          Simple32BitDataModel>::kPaddingLocationsPaddingByteCount ==
          expected_padding_locations_padding_byte_count_OnlyTrailingPadding3_DifferentStructAlignment,
      "Struct should have specified padding.");

  static_assert(
      SerializableClassPaddingIndexes<
          ArbitraryInternalMultiPadding1Then2,
          Simple32BitDataModel>::kPaddingLocationsPaddingByteCount ==
          expected_padding_locations_padding_byte_count_ArbitraryInternalMultiPadding1Then2,
      "Struct should have specified padding.");
  static_assert(
      SerializableClassPaddingIndexes<
          ArbitraryMixedPaddings1Then2Then3,
          Simple32BitDataModel>::kPaddingLocationsPaddingByteCount ==
          expected_padding_locations_padding_byte_count_ArbitraryMixedPaddings1Then2Then3,
      "Struct should have specified padding.");
  static_assert(
      SerializableClassPaddingIndexes<
          ArbitraryMixedPaddingOneDataMemberBetween,
          Simple32BitDataModel>::kPaddingLocationsPaddingByteCount ==
          expected_padding_locations_padding_byte_count_ArbitraryMixedPaddingOneDataMemberBetween,
      "Struct should have specified padding.");

  static_assert(
      SerializableClassPaddingIndexes<
          ForNotSelfAligned_ArbitraryMixedPaddings1Then3,
          Simple32BitDatModelButIntsNotSelfAligned>::
              kPaddingLocationsPaddingByteCount ==
          expected_padding_locations_padding_byte_count_ForNotSelfAligned_ArbitraryMixedPaddings1Then3,
      "Struct should have specified padding.");

  // Test padding byte count
  static_assert(SerializableClassPaddingIndexes<
                    Empty, Simple32BitDataModel>::kPaddingBytesCount == 0,
                "Empty struct should have no padding.");
  static_assert(SerializableClassPaddingIndexes<
                    NoPadding, Simple32BitDataModel>::kPaddingBytesCount == 0,
                "Struct should have specified padding.");

  static_assert(SerializableClassPaddingIndexes<
                    OnlyInternalPadding3_Simple,
                    Simple32BitDataModel>::kPaddingBytesCount == 3,
                "Struct should have specified padding.");
  static_assert(SerializableClassPaddingIndexes<
                    OnlyInternalPadding3_DifferentStructAlignment,
                    Simple32BitDataModel>::kPaddingBytesCount == 3,
                "Struct should have specified padding.");

  static_assert(SerializableClassPaddingIndexes<
                    OnlyInternalPadding1_Simple,
                    Simple32BitDataModel>::kPaddingBytesCount == 1,
                "Struct should have specified padding.");
  static_assert(SerializableClassPaddingIndexes<
                    OnlyInternalPadding1_DifferentStructAlignment,
                    Simple32BitDataModel>::kPaddingBytesCount == 1,
                "Struct should have specified padding.");

  static_assert(SerializableClassPaddingIndexes<
                    OnlyTrailingPadding3_Simple,
                    Simple32BitDataModel>::kPaddingBytesCount == 3,
                "Struct should have specified padding.");
  static_assert(SerializableClassPaddingIndexes<
                    OnlyTrailingPadding3_DifferentStructAlignment,
                    Simple32BitDataModel>::kPaddingBytesCount == 3,
                "Struct should have specified padding.");

  static_assert(SerializableClassPaddingIndexes<
                    ArbitraryInternalMultiPadding1Then2,
                    Simple32BitDataModel>::kPaddingBytesCount == 3,
                "Struct should have specified padding.");
  static_assert(SerializableClassPaddingIndexes<
                    ArbitraryMixedPaddings1Then2Then3,
                    Simple32BitDataModel>::kPaddingBytesCount == 6,
                "Struct should have specified padding.");
  static_assert(SerializableClassPaddingIndexes<
                    ArbitraryMixedPaddingOneDataMemberBetween,
                    Simple32BitDataModel>::kPaddingBytesCount == 5,
                "Struct should have specified padding.");

  static_assert(
      SerializableClassPaddingIndexes<
          ForNotSelfAligned_ArbitraryMixedPaddings1Then3,
          Simple32BitDatModelButIntsNotSelfAligned>::kPaddingBytesCount == 2,
      "Struct should have specified padding.");

  // Test padding indexes method
  static_assert(SerializableClassPaddingIndexes<
                    Empty, Simple32BitDataModel>::kPaddingByteIndexes ==
                    expected_padding_indexes_Empty,
                "Empty struct should have no padding.");
  static_assert(SerializableClassPaddingIndexes<
                    NoPadding, Simple32BitDataModel>::kPaddingByteIndexes ==
                    expected_padding_indexes_NoPadding,
                "Struct should have specified padding.");

  static_assert(SerializableClassPaddingIndexes<
                    OnlyInternalPadding3_Simple,
                    Simple32BitDataModel>::kPaddingByteIndexes ==
                    expected_padding_indexes_OnlyInternalPadding3_Simple,
                "Struct should have specified padding.");
  static_assert(
      SerializableClassPaddingIndexes<
          OnlyInternalPadding3_DifferentStructAlignment,
          Simple32BitDataModel>::kPaddingByteIndexes ==
          expected_padding_indexes_OnlyInternalPadding3_DifferentStructAlignment,
      "Struct should have specified padding.");

  static_assert(SerializableClassPaddingIndexes<
                    OnlyInternalPadding1_Simple,
                    Simple32BitDataModel>::kPaddingByteIndexes ==
                    expected_padding_indexes_OnlyInternalPadding1_Simple,
                "Struct should have specified padding.");
  static_assert(
      SerializableClassPaddingIndexes<
          OnlyInternalPadding1_DifferentStructAlignment,
          Simple32BitDataModel>::kPaddingByteIndexes ==
          expected_padding_indexes_OnlyInternalPadding1_DifferentStructAlignment,
      "Struct should have specified padding.");

  static_assert(SerializableClassPaddingIndexes<
                    OnlyTrailingPadding3_Simple,
                    Simple32BitDataModel>::kPaddingByteIndexes ==
                    expected_padding_indexes_OnlyTrailingPadding3_Simple,
                "Struct should have specified padding.");
  static_assert(
      SerializableClassPaddingIndexes<
          OnlyTrailingPadding3_DifferentStructAlignment,
          Simple32BitDataModel>::kPaddingByteIndexes ==
          expected_padding_indexes_OnlyTrailingPadding3_DifferentStructAlignment,
      "Struct should have specified padding.");

  static_assert(
      SerializableClassPaddingIndexes<
          ArbitraryInternalMultiPadding1Then2,
          Simple32BitDataModel>::kPaddingByteIndexes ==
          expected_padding_indexes_ArbitraryInternalMultiPadding1Then2,
      "Struct should have specified padding.");
  static_assert(SerializableClassPaddingIndexes<
                    ArbitraryMixedPaddings1Then2Then3,
                    Simple32BitDataModel>::kPaddingByteIndexes ==
                    expected_padding_indexes_ArbitraryMixedPaddings1Then2Then3,
                "Struct should have specified padding.");
  static_assert(
      SerializableClassPaddingIndexes<
          ArbitraryMixedPaddingOneDataMemberBetween,
          Simple32BitDataModel>::kPaddingByteIndexes ==
          expected_padding_indexes_ArbitraryMixedPaddingOneDataMemberBetween,
      "Struct should have specified padding.");

  static_assert(
      SerializableClassPaddingIndexes<
          ForNotSelfAligned_ArbitraryMixedPaddings1Then3,
          Simple32BitDatModelButIntsNotSelfAligned>::kPaddingByteIndexes ==
          expected_padding_indexes_ForNotSelfAligned_ArbitraryMixedPaddings1Then3,
      "Struct should have specified padding.");
};

auto main() -> int {
  auto thing_under_debug = SerializableClassPaddingIndexes<
      TEST_SerializableClassPaddingIndexes::
          ForNotSelfAligned_ArbitraryMixedPaddings1Then3,
      TEST_SerializableClassPaddingIndexes::
          Simple32BitDatModelButIntsNotSelfAligned>::kPaddingByteIndexes;
  std::cout << "# of elements = " << thing_under_debug.size() << std::endl;

  for (int i = 0; i < thing_under_debug.size(); i++) {
    std::cout << thing_under_debug[i] << std::endl;
  }

  return 0;
}
