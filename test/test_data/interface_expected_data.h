#ifndef INTERFACE_EXPECTED_TEST_DATA_HPP
#define INTERFACE_EXPECTED_TEST_DATA_HPP

#include <type_traits>

namespace test_data {

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

  static constexpr auto get_expected_padding_locations_byte_counts() {
    return Derived::kExpectedPaddingLocationsByteCountsImpl;
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
            decltype(&Derived::kExpectedPaddingLocationsByteCountsImpl)>::value,
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

}  // namespace test_data

#endif  // INTERFACE_EXPECTED_TEST_DATA_HPP
