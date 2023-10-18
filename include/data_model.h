#ifndef DATA_MODEL_H
#define DATA_MODEL_H

#include <cstddef>

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
  static constexpr auto get_alignment() -> std::size_t = delete;

  template <>
  static constexpr auto get_alignment<char>() -> std::size_t {
    return static_cast<std::size_t>(kCharAlign);
  }

  template <>
  static constexpr auto get_alignment<unsigned char>() -> std::size_t {
    return static_cast<std::size_t>(kUnsignedCharAlign);
  }

  template <>
  static constexpr auto get_alignment<signed char>() -> std::size_t {
    return static_cast<std::size_t>(kSignedCharAlign);
  }

  template <>
  static constexpr auto get_alignment<wchar_t>() -> std::size_t {
    return static_cast<std::size_t>(kWCharAlign);
  }

  template <>
  // NOLINTNEXTLINE(google-runtime-int)
  static constexpr auto get_alignment<short>() -> std::size_t {
    return static_cast<std::size_t>(kShortAlign);
  }

  template <>
  // NOLINTNEXTLINE(google-runtime-int)
  static constexpr auto get_alignment<unsigned short>() -> std::size_t {
    return static_cast<std::size_t>(kUnsignedShortAlign);
  }

  template <>
  static constexpr auto get_alignment<int>() -> std::size_t {
    return static_cast<std::size_t>(kIntAlign);
  }

  template <>
  static constexpr auto get_alignment<unsigned int>() -> std::size_t {
    return static_cast<std::size_t>(kUnsignedIntAlign);
  }

  template <>
  // NOLINTNEXTLINE(google-runtime-int)
  static constexpr auto get_alignment<long>() -> std::size_t {
    return static_cast<std::size_t>(kLongAlign);
  }

  template <>
  // NOLINTNEXTLINE(google-runtime-int)
  static constexpr auto get_alignment<unsigned long>() -> std::size_t {
    return static_cast<std::size_t>(kUnsignedLongAlign);
  }

  template <>
  // NOLINTNEXTLINE(google-runtime-int)
  static constexpr auto get_alignment<long long>() -> std::size_t {
    return static_cast<std::size_t>(kLongLongAlign);
  }

  template <>
  // NOLINTNEXTLINE(google-runtime-int)
  static constexpr auto get_alignment<unsigned long long>() -> std::size_t {
    return static_cast<std::size_t>(kUnsignedLongLongAlign);
  }

  template <>
  static constexpr auto get_alignment<float>() -> std::size_t {
    return static_cast<std::size_t>(kFloatAlign);
  }

  template <>
  static constexpr auto get_alignment<double>() -> std::size_t {
    return static_cast<std::size_t>(kDoubleAlign);
  }

  template <>
  static constexpr auto get_alignment<long double>() -> std::size_t {
    return static_cast<std::size_t>(kLongDoubleAlign);
  }

  template <>
  static constexpr auto get_alignment<bool>() -> std::size_t {
    return static_cast<std::size_t>(kBoolAlign);
  }

  template <class T>
  static constexpr auto get_size() -> std::size_t = delete;

  template <>
  static constexpr auto get_size<char>() -> std::size_t {
    return static_cast<std::size_t>(kCharSize);
  }

  template <>
  static constexpr auto get_size<unsigned char>() -> std::size_t {
    return static_cast<std::size_t>(kUnsignedCharSize);
  }

  template <>
  static constexpr auto get_size<signed char>() -> std::size_t {
    return static_cast<std::size_t>(kSignedCharSize);
  }

  template <>
  static constexpr auto get_size<wchar_t>() -> std::size_t {
    return static_cast<std::size_t>(kWCharSize);
  }

  template <>
  // NOLINTNEXTLINE(google-runtime-int)
  static constexpr auto get_size<short>() -> std::size_t {
    return static_cast<std::size_t>(kShortSize);
  }

  template <>
  // NOLINTNEXTLINE(google-runtime-int)
  static constexpr auto get_size<unsigned short>() -> std::size_t {
    return static_cast<std::size_t>(kUnsignedShortSize);
  }

  template <>
  static constexpr auto get_size<int>() -> std::size_t {
    return static_cast<std::size_t>(kIntSize);
  }

  template <>
  static constexpr auto get_size<unsigned int>() -> std::size_t {
    return static_cast<std::size_t>(kUnsignedIntSize);
  }

  template <>
  // NOLINTNEXTLINE(google-runtime-int)
  static constexpr auto get_size<long>() -> std::size_t {
    return static_cast<std::size_t>(kLongSize);
  }

  template <>
  // NOLINTNEXTLINE(google-runtime-int)
  static constexpr auto get_size<unsigned long>() -> std::size_t {
    return static_cast<std::size_t>(kUnsignedLongSize);
  }

  template <>
  // NOLINTNEXTLINE(google-runtime-int)
  static constexpr auto get_size<long long>() -> std::size_t {
    return static_cast<std::size_t>(kLongLongSize);
  }

  template <>
  // NOLINTNEXTLINE(google-runtime-int)
  static constexpr auto get_size<unsigned long long>() -> std::size_t {
    return static_cast<std::size_t>(kUnsignedLongLongSize);
  }

  template <>
  static constexpr auto get_size<float>() -> std::size_t {
    return static_cast<std::size_t>(kFloatSize);
  }

  template <>
  static constexpr auto get_size<double>() -> std::size_t {
    return static_cast<std::size_t>(kDoubleSize);
  }

  template <>
  static constexpr auto get_size<long double>() -> std::size_t {
    return static_cast<std::size_t>(kLongDoubleSize);
  }

  template <>
  static constexpr auto get_size<bool>() -> std::size_t {
    return static_cast<std::size_t>(kBoolSize);
  }
};

}  // namespace little_pp

#endif  // DATA_MODEL_H
