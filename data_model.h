#ifndef DATA_MODEL_H
#define DATA_MODEL_H

#include <cstddef>

// Encode sizeof and alignof of supported POD type into a DataModel type.
// See https://en.cppreference.com/w/cpp/language/types
template<
    std::size_t kCharSize,             std::size_t kCharAlign,
    std::size_t kUnsignedCharSize,     std::size_t kUnsignedCharAlign,
    std::size_t kSignedCharSize,       std::size_t kSignedCharAlign,
    std::size_t kWCharSize,            std::size_t kWCharAlign,

    std::size_t kShortSize,            std::size_t kShortAlign,
    std::size_t kUnsignedShortSize,    std::size_t kUnsignedShortAlign,

    std::size_t kIntSize,              std::size_t kIntAlign,
    std::size_t kUnsignedIntSize,      std::size_t kUnsignedIntAlign,

    std::size_t kLongSize,             std::size_t kLongAlign,
    std::size_t kUnsignedLongSize,     std::size_t kUnsignedLongAlign,

    std::size_t kLongLongSize,         std::size_t kLongLongAlign,
    std::size_t kUnsignedLongLongSize, std::size_t kUnsignedLongLongAlign,

    std::size_t kFloatSize,            std::size_t kFloatAlign,
    std::size_t kDoubleSize,           std::size_t kDoubleAlign,
    std::size_t kLongDoubleSize,       std::size_t kLongDoubleAlign,

    std::size_t kBoolSize,             std::size_t kBoolAlign
>
struct DataModel {
    template<class T>
    static constexpr std::size_t GetAlignment(){
        static_assert(false, "Unrecognized type");
        return 0;
    }

    template<>
    static constexpr std::size_t GetAlignment<char>() {
        return static_cast<std::size_t>(kCharAlign);
    }

    template<>
    static constexpr std::size_t GetAlignment<unsigned char>() {
        return static_cast<std::size_t>(kUnsignedCharAlign);
    }

    template<>
    static constexpr std::size_t GetAlignment<signed char>() {
        return static_cast<std::size_t>(kSignedCharAlign);
    }

    template<>
    static constexpr std::size_t GetAlignment<wchar_t>() {
        return static_cast<std::size_t>(kWCharAlign);
    }

    template<>
    static constexpr std::size_t GetAlignment<short>() {
        return static_cast<std::size_t>(kShortAlign);
    }

    template<>
    static constexpr std::size_t GetAlignment<unsigned short>() {
        return static_cast<std::size_t>(kUnsignedShortAlign);
    }

    template<>
    static constexpr std::size_t GetAlignment<int>() {
        return static_cast<std::size_t>(kIntAlign);
    }

    template<>
    static constexpr std::size_t GetAlignment<unsigned int>() {
        return static_cast<std::size_t>(kUnsignedIntAlign);
    }

    template<>
    static constexpr std::size_t GetAlignment<long>() {
        return static_cast<std::size_t>(kLongAlign);
    }

    template<>
    static constexpr std::size_t GetAlignment<unsigned long>() {
        return static_cast<std::size_t>(kUnsignedLongAlign);
    }

    template<>
    static constexpr std::size_t GetAlignment<long long>() {
        return static_cast<std::size_t>(kLongLongAlign);
    }

    template<>
    static constexpr std::size_t GetAlignment<unsigned long long>() {
        return static_cast<std::size_t>(kUnsignedLongLongAlign);
    }
    
    template<>
    static constexpr std::size_t GetAlignment<float>() {
        return static_cast<std::size_t>(kFloatAlign);
    }
    
    template<>
    static constexpr std::size_t GetAlignment<double>() {
        return static_cast<std::size_t>(kDoubleAlign);
    }

    template<>
    static constexpr std::size_t GetAlignment<long double>() {
        return static_cast<std::size_t>(kLongDoubleAlign);
    }
    
    template<>
    static constexpr std::size_t GetAlignment<bool>() {
        return static_cast<std::size_t>(kBoolAlign);

    }

    template<class T>
    static constexpr std::size_t GetSize(){
        static_assert(false, "Unrecognized type");
        return 0;
    }

    template<>
    static constexpr std::size_t GetSize<char>() {
        return static_cast<std::size_t>(kCharSize);
    }

    template<>
    static constexpr std::size_t GetSize<unsigned char>() {
        return static_cast<std::size_t>(kUnsignedCharSize);
    }

    template<>
    static constexpr std::size_t GetSize<signed char>() {
        return static_cast<std::size_t>(kSignedCharSize);
    }

    template<>
    static constexpr std::size_t GetSize<wchar_t>() {
        return static_cast<std::size_t>(kWCharSize);
    }

    template<>
    static constexpr std::size_t GetSize<short>() {
        return static_cast<std::size_t>(kShortSize);
    }

    template<>
    static constexpr std::size_t GetSize<unsigned short>() {
        return static_cast<std::size_t>(kUnsignedShortSize);
    }

    template<>
    static constexpr std::size_t GetSize<int>() {
        return static_cast<std::size_t>(kIntSize);
    }

    template<>
    static constexpr std::size_t GetSize<unsigned int>() {
        return static_cast<std::size_t>(kUnsignedIntSize);
    }

    template<>
    static constexpr std::size_t GetSize<long>() {
        return static_cast<std::size_t>(kLongSize);
    }

    template<>
    static constexpr std::size_t GetSize<unsigned long>() {
        return static_cast<std::size_t>(kUnsignedLongSize);
    }

    template<>
    static constexpr std::size_t GetSize<long long>() {
        return static_cast<std::size_t>(kLongLongSize);
    }

    template<>
    static constexpr std::size_t GetSize<unsigned long long>() {
        return static_cast<std::size_t>(kUnsignedLongLongSize);
    }
    
    template<>
    static constexpr std::size_t GetSize<float>() {
        return static_cast<std::size_t>(kFloatSize);
    }
    
    template<>
    static constexpr std::size_t GetSize<double>() {
        return static_cast<std::size_t>(kDoubleSize);
    }

    template<>
    static constexpr std::size_t GetSize<long double>() {
        return static_cast<std::size_t>(kLongDoubleSize);
    }
    
    template<>
    static constexpr std::size_t GetSize<bool>() {
        return static_cast<std::size_t>(kBoolSize);

    }
};

#endif // DATA_MODEL_H
