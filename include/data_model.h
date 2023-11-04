#ifndef DATA_MODEL_H
#define DATA_MODEL_H

#include <cstddef>
#include <type_traits>

namespace little_pp {

enum class Endianess {
  kBigEndian,
  kLittleEndian,
};

// NOTE: this code is compiler-dependent. Currently, Clang and GCC are
//       implemented.
#ifdef __BYTE_ORDER__
constexpr auto get_this_architecture_endianess() -> Endianess {
// Tested w/ Big-endian architectures: M68K gcc 13.2.0 and SPARC LEON gcc 13.2.0
// Tested w/ Little-endian architecture: x86-64 clang (trunk)
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  return Endianess::kBigEndian;
#endif
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  return Endianess::kLittleEndian;
#endif
}
#endif  //__BYTE_ORDER__

// Encode sizeof and alignof of supported POD type into a DataModel type.
// See https://en.cppreference.com/w/cpp/language/types
template <std::size_t kCharSize, std::size_t kCharAlign,
          std::size_t kUnsignedCharSize, std::size_t kUnsignedCharAlign,
          std::size_t kSignedCharSize, std::size_t kSignedCharAlign,
          std::size_t kWCharSize, std::size_t kWCharAlign,

          std::size_t kShortSize, std::size_t kShortAlign,
          std::size_t kUnsignedShortSize, std::size_t kUnsignedShortAlign,

          std::size_t kIntSize, std::size_t kIntAlign,
          std::size_t kUnsignedIntSize, std::size_t kUnsignedIntAlign,

          std::size_t kLongSize, std::size_t kLongAlign,
          std::size_t kUnsignedLongSize, std::size_t kUnsignedLongAlign,

          std::size_t kLongLongSize, std::size_t kLongLongAlign,
          std::size_t kUnsignedLongLongSize, std::size_t kUnsignedLongLongAlign,

          std::size_t kFloatSize, std::size_t kFloatAlign,
          std::size_t kDoubleSize, std::size_t kDoubleAlign,
          std::size_t kLongDoubleSize, std::size_t kLongDoubleAlign,

          std::size_t kBoolSize, std::size_t kBoolAlign>
struct DataModel {
  template <class T>
  static constexpr auto get_size() -> std::size_t {
    // NOTE: this method cannot be implemented using template full
    //       specialization:
    //       ```
    //       An explicit specialization of a member function, member class or
    //       static data member of a class template shall be declared in the
    //       namespace of which the class template is a member.
    //       ```C++03, §14.7.3/2:
    //
    //       Still an issue in C+14. See this stack overflow page
    //       page:https://stackoverflow.com/questions/3052579/explicit-specialization-in-non-namespace-scope
    //       note that a user comments this is no longer true for C++17.
    if (std::is_same<T, char>::value) {
      return static_cast<std::size_t>(kCharSize);
    }
    if (std::is_same<T, char>::value) {
      return static_cast<std::size_t>(kCharSize);
    }

    if (std::is_same<T, unsigned char>::value) {
      return static_cast<std::size_t>(kUnsignedCharSize);
    }

    if (std::is_same<T, signed char>::value) {
      return static_cast<std::size_t>(kSignedCharSize);
    }

    if (std::is_same<T, wchar_t>::value) {
      return static_cast<std::size_t>(kWCharSize);
    }

    // NOLINTNEXTLINE(google-runtime-int)
    if (std::is_same<T, short>::value) {
      return static_cast<std::size_t>(kShortSize);
    }

    // NOLINTNEXTLINE(google-runtime-int)
    if (std::is_same<T, unsigned short>::value) {
      return static_cast<std::size_t>(kUnsignedShortSize);
    }

    if (std::is_same<T, int>::value) {
      return static_cast<std::size_t>(kIntSize);
    }

    if (std::is_same<T, unsigned int>::value) {
      return static_cast<std::size_t>(kUnsignedIntSize);
    }

    // NOLINTNEXTLINE(google-runtime-int)
    if (std::is_same<T, long>::value) {
      return static_cast<std::size_t>(kLongSize);
    }

    // NOLINTNEXTLINE(google-runtime-int)
    if (std::is_same<T, unsigned long>::value) {
      return static_cast<std::size_t>(kUnsignedLongSize);
    }

    // NOLINTNEXTLINE(google-runtime-int)
    if (std::is_same<T, long long>::value) {
      return static_cast<std::size_t>(kLongLongSize);
    }

    // NOLINTNEXTLINE(google-runtime-int)
    if (std::is_same<T, unsigned long long>::value) {
      return static_cast<std::size_t>(kUnsignedLongLongSize);
    }

    if (std::is_same<T, float>::value) {
      return static_cast<std::size_t>(kFloatSize);
    }

    if (std::is_same<T, double>::value) {
      return static_cast<std::size_t>(kDoubleSize);
    }

    if (std::is_same<T, long double>::value) {
      return static_cast<std::size_t>(kLongDoubleSize);
    }

    if (std::is_same<T, bool>::value) {
      return static_cast<std::size_t>(kBoolSize);
    }
  }

  template <class T>
  static constexpr auto get_alignment() -> std::size_t {
    // NOTE: this method cannot be implemented using template full
    //       specialization:
    //       ```
    //       An explicit specialization of a member function, member class or
    //       static data member of a class template shall be declared in the
    //       namespace of which the class template is a member.
    //       ```C++03, §14.7.3/2:
    //
    //       Still an issue in C+14. See this stack overflow page
    //       page:https://stackoverflow.com/questions/3052579/explicit-specialization-in-non-namespace-scope
    //       note that a user comments this is no longer true for C++17.
    if (std::is_same<T, char>::value) {
      return static_cast<std::size_t>(kCharAlign);
    }
    if (std::is_same<T, char>::value) {
      return static_cast<std::size_t>(kCharAlign);
    }

    if (std::is_same<T, unsigned char>::value) {
      return static_cast<std::size_t>(kUnsignedCharAlign);
    }

    if (std::is_same<T, signed char>::value) {
      return static_cast<std::size_t>(kSignedCharAlign);
    }

    if (std::is_same<T, wchar_t>::value) {
      return static_cast<std::size_t>(kWCharAlign);
    }

    // NOLINTNEXTLINE(google-runtime-int)
    if (std::is_same<T, short>::value) {
      return static_cast<std::size_t>(kShortAlign);
    }

    // NOLINTNEXTLINE(google-runtime-int)
    if (std::is_same<T, unsigned short>::value) {
      return static_cast<std::size_t>(kUnsignedShortAlign);
    }

    if (std::is_same<T, int>::value) {
      return static_cast<std::size_t>(kIntAlign);
    }

    if (std::is_same<T, unsigned int>::value) {
      return static_cast<std::size_t>(kUnsignedIntAlign);
    }

    // NOLINTNEXTLINE(google-runtime-int)
    if (std::is_same<T, long>::value) {
      return static_cast<std::size_t>(kLongAlign);
    }

    // NOLINTNEXTLINE(google-runtime-int)
    if (std::is_same<T, unsigned long>::value) {
      return static_cast<std::size_t>(kUnsignedLongAlign);
    }

    // NOLINTNEXTLINE(google-runtime-int)
    if (std::is_same<T, long long>::value) {
      return static_cast<std::size_t>(kLongLongAlign);
    }

    // NOLINTNEXTLINE(google-runtime-int)
    if (std::is_same<T, unsigned long long>::value) {
      return static_cast<std::size_t>(kUnsignedLongLongAlign);
    }

    if (std::is_same<T, float>::value) {
      return static_cast<std::size_t>(kFloatAlign);
    }

    if (std::is_same<T, double>::value) {
      return static_cast<std::size_t>(kDoubleAlign);
    }

    if (std::is_same<T, long double>::value) {
      return static_cast<std::size_t>(kLongDoubleAlign);
    }

    if (std::is_same<T, bool>::value) {
      return static_cast<std::size_t>(kBoolAlign);
    }
  }
};

}  // namespace little_pp

#endif  // DATA_MODEL_H
