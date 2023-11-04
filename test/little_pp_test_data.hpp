#ifndef LITTLE_PP_TEST_DATA_HPP
#define LITTLE_PP_TEST_DATA_HPP

#include "include/little_pp.h"

namespace test_data {

// clang-format off
// NOLINTBEGIN(*-magic-numbers)
using Simple32BitDataModel =
    little_pp::DataModel<1, 1, 1, 1, 1, 1, 2, 2,

                         2, 2, 2, 2,

                         4, 4, 4, 4,

                         8, 8, 8, 8,

                         8, 8, 8, 8,

                         4, 4, 8, 8, 8, 8,

                         1, 1>;
using Simple32BitButIntsNotSelfAlignedDataModel =
    little_pp::DataModel<1, 1, 1, 1, 1, 1, 2, 2,

                         2, 2, 2, 2,

                         4, 2, 4, 2,

                         8, 8, 8, 8,

                         8, 8, 8, 8,

                         4, 4, 8, 8, 8, 8,

                         1, 1>;
// NOLINTEND(*-magic-numbers)
// clang-format on

template <typename Derived>
class IExpectedData {
 public:
  struct Parameters {
    using DataModelType = typename Derived::DataModelTypeImpl;
    using SerializedType = typename Derived::SerializedTypeImpl;
  };

  static constexpr auto get_expected_padding_locations_count() -> std::size_t {
    return Derived::kExpectedPaddingLocationsCountImpl;
  }

  static constexpr auto get_expected_padding_locations_padding_byte_count() {
    return Derived::kExpectedPaddingLocationsPaddingByteCountImpl;
  }
  static constexpr auto get_expected_padding_byte_indexes() {
    return Derived::kExpectedPaddingByteIndexesImpl;
  }

 private:
  // This is an imperfect solution to enforcing the CRTPBase interface on
  // Derived; without it, the interface is only enforced on Derived when there
  // exists code which calls Derived implementations via the interface. So, by
  // default the interface is enforced (statically) via usage. This solution
  // improves by enforcing the interface on construction of derived. Note that
  // this enforcement is dependent on synchronization between the static
  // assertion and the usage. In general there are two forms of checks:
  // 1. A strong check that tests for existence of member function signature
  // 2. A weak check that tests only for existence of member function name
  // See the CRTP Boilerplate C++11 Verbose Example reference (top of document)
  // for more info.
  //
  // This interface will use the strong check for interface enforcement.
  static constexpr auto enforce_interface() -> bool {
    // These static_asserts will not fail if `&Derived::__` is not implemented,
    // but `decltype(&Derived::__)` will fail. Note that since `&Derived::__`
    // should never be void, `std::is_void` is a suitable mechanism for
    // enforcement. See the CRTP Boilerplate C++11 Verbose Example reference
    // (top of document) for more info.
    static_assert(
        !std::is_void<
            decltype(&Derived::kExpectedPaddingLocationsCountImpl)>::value,
        "Derived class must implement member function.");
    static_assert(
        !std::is_void<
            decltype(&Derived::kExpectedPaddingLocationsPaddingByteCountImpl)>::
            value,
        "Derived class must implement member function.");
    static_assert(
        !std::is_void<
            decltype(&Derived::kExpectedPaddingByteIndexesImpl)>::value,
        "Derived class must implement member function.");
    // if "static execution" reaches this point, the interface was successfully
    // enforced.
    return true;
  }

  // Enforce correct CRTP usage (ie derived classes passing themselves).
  // Constructors and destructors must be accessible to derived classes and
  // we hide these from "everyone" except the friend class which must be the
  // Derived class otherwise we'll have a compilation error. See the CRTP Blog
  // Series reference (top of document) for more info.
  IExpectedData() { enforce_interface(); }
  ~IExpectedData() = default;
  friend Derived;
};

namespace struct_empty {

struct Empty {};

template <typename DataModelT>
class ExpectedData : public IExpectedData<ExpectedData<DataModelT>> {};

template <>
class ExpectedData<Simple32BitDataModel>
    : public IExpectedData<ExpectedData<Simple32BitDataModel>> {
 public:
  using DataModelTypeImpl = Simple32BitDataModel;
  using SerializedTypeImpl = Empty;

  static constexpr std::size_t kExpectedPaddingLocationsCountImpl = 0;
  static constexpr std::array<std::size_t, 0>
      kExpectedPaddingLocationsPaddingByteCountImpl{};
  static constexpr std::array<std::size_t, 0> kExpectedPaddingByteIndexesImpl{};
};

template <>
class ExpectedData<Simple32BitButIntsNotSelfAlignedDataModel>
    : public IExpectedData<
          ExpectedData<Simple32BitButIntsNotSelfAlignedDataModel>> {
 public:
  using DataModelTypeImpl = Simple32BitButIntsNotSelfAlignedDataModel;
  using SerializedTypeImpl = Empty;

  static constexpr std::size_t kExpectedPaddingLocationsCountImpl = 0;
  static constexpr std::array<std::size_t, 0>
      kExpectedPaddingLocationsPaddingByteCountImpl{};
  static constexpr std::array<std::size_t, 0> kExpectedPaddingByteIndexesImpl{};
};

}  // namespace struct_empty

namespace struct_no_padding_32bit {

// NOLINTBEGIN (google-runtime-int)
struct NoPadding32Bit {
  unsigned short a;
  unsigned char b;
  unsigned char c;
  int d;
};
// NOLINTEND (google-runtime-int)

template <typename DataModelT>
class ExpectedData : public IExpectedData<ExpectedData<DataModelT>> {};

template <>
class ExpectedData<Simple32BitDataModel>
    : public IExpectedData<ExpectedData<Simple32BitDataModel>> {
 public:
  using DataModelTypeImpl = Simple32BitDataModel;
  using SerializedTypeImpl = NoPadding32Bit;

  static constexpr std::size_t kExpectedPaddingLocationsCountImpl = 0;
  static constexpr std::array<std::size_t, 0>
      kExpectedPaddingLocationsPaddingByteCountImpl{};
  static constexpr std::array<std::size_t, 0> kExpectedPaddingByteIndexesImpl{};
};

template <>
class ExpectedData<Simple32BitButIntsNotSelfAlignedDataModel>
    : public IExpectedData<
          ExpectedData<Simple32BitButIntsNotSelfAlignedDataModel>> {
 public:
  using DataModelTypeImpl = Simple32BitButIntsNotSelfAlignedDataModel;
  using SerializedTypeImpl = NoPadding32Bit;

  static constexpr std::size_t kExpectedPaddingLocationsCountImpl = 0;
  static constexpr std::array<std::size_t, 0>
      kExpectedPaddingLocationsPaddingByteCountImpl{};
  static constexpr std::array<std::size_t, 0> kExpectedPaddingByteIndexesImpl{};
};

}  // namespace struct_no_padding_32bit

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
class ExpectedData<Simple32BitDataModel>
    : public IExpectedData<ExpectedData<Simple32BitDataModel>> {
 public:
  using DataModelTypeImpl = Simple32BitDataModel;
  using SerializedTypeImpl = InternalPadding3;

  static constexpr std::size_t kExpectedPaddingLocationsCountImpl = 1;
  static constexpr std::array<std::size_t, 1>
      kExpectedPaddingLocationsPaddingByteCountImpl{3};
  static constexpr std::array<std::size_t, 3> kExpectedPaddingByteIndexesImpl{
      1, 2, 3};
};

template <>
class ExpectedData<Simple32BitButIntsNotSelfAlignedDataModel>
    : public IExpectedData<
          ExpectedData<Simple32BitButIntsNotSelfAlignedDataModel>> {
 public:
  using DataModelTypeImpl = Simple32BitButIntsNotSelfAlignedDataModel;
  using SerializedTypeImpl = InternalPadding3;

  static constexpr std::size_t kExpectedPaddingLocationsCountImpl = 1;
  static constexpr std::array<std::size_t, 1>
      kExpectedPaddingLocationsPaddingByteCountImpl{1};
  static constexpr std::array<std::size_t, 1> kExpectedPaddingByteIndexesImpl{
      1};
};

}  // namespace struct_internal_padding_3_32bit

}  // namespace test_data

#endif  // LITTLE_PP_TEST_DATA_HPP
