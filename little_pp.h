#include <iostream>
#include <cstring>
#include <assert.h>
#include <array>

#include "boost/pfr.hpp"
#include "boost/pfr/core.hpp"

#include "data_model.h"

//-------------------------------------------------
// std::array constexpr equality overloads
//-------------------------------------------------
template<std::size_t N>
struct operator_details {
    template <std::size_t start, std::size_t end, std::size_t inc>
    static constexpr typename std::enable_if<(start < end), bool>::type constexpr_for(std::array<std::size_t, N> lhs, std::array<std::size_t, N> rhs)
    {
        if(std::get<start>(lhs) != std::get<start>(rhs)) {
            return false;
        }
        return constexpr_for<start + inc, end, inc>(lhs, rhs);
    }

    template <std::size_t start, std::size_t end, std::size_t inc>
    static constexpr typename std::enable_if<!(start < end), bool>::type constexpr_for(std::array<std::size_t, N> lhs, std::array<std::size_t, N> rhs)
    {
        return true;
    }
};

template<std::size_t N>
static constexpr bool operator==(std::array<std::size_t, N> lhs, std::array<std::size_t, N> rhs) {
    return operator_details<N>::template constexpr_for<0, N, 1>(lhs, rhs);
}

template<std::size_t N1, std::size_t N2>
static constexpr bool operator==(std::array<std::size_t, N1> lhs, std::array<std::size_t, N2> rhs) {
    return false;
}

template<std::size_t N>
static constexpr bool operator!=(std::array<std::size_t, N> lhs, std::array<std::size_t, N> rhs) {
    return !operator_details<N>::template constexpr_for<0, N, 1>(lhs, rhs);
}

template<std::size_t N1, std::size_t N2>
static constexpr bool operator!=(std::array<std::size_t, N1> lhs, std::array<std::size_t, N2> rhs) {
    return true;
}

constexpr std::array<std::size_t, 0> empty_array{};
constexpr std::array<std::size_t, 2> some_array{1, 2};
constexpr std::array<std::size_t, 2> other_array{2, 1};
static_assert(empty_array == empty_array, "These are the same array and should be equal.");
static_assert(some_array == some_array, "These are the same array and should be equal.");
static_assert(some_array != empty_array, "These are not the same array and should NOT be equal.");
static_assert(some_array != other_array, "These are not the same array and should NOT be equal.");
//-------------------------------------------------


// YAGNI; only interested in supporting 8-bit-byte implementations at this time. Yes, they do
// still exist and are not just relics of the past within the embedded space. See this 
// discussion and this active TI DSP:
// - https://groups.google.com/g/comp.lang.c++/c/14QZUvWE_Xc
// - https://www.ti.com/product/TMS320F2809
static_assert(CHAR_BIT == 8, "Serialization implementation assumes/depends-on 8-bit bytes.");

enum class endianess : char {
    kBigendian = 0,
    kLittleendian = 1,
};

// Compiler-Dependent-Code: Clang and GCC have macros defining endianess for target
#ifdef __BYTE_ORDER__
constexpr endianess GetThisArchitectureendianess() {
    // Tested w/ Big-endian architectures: M68K gcc 13.2.0 and SPARC LEON gcc 13.2.0
    // Tested w/ Little-endian architecture: x86-64 clang (trunk) 
    #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__ 
    return endianess::kBigendian;
    #endif
    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return endianess::kLittleendian;
    #endif
}
#endif //__BYTE_ORDER__

template <typename T>
struct IsSupportedType {
    static const bool value = std::is_arithmetic<T>::value ||
    std::is_enum<T>::value ||
    std::is_class<T>::value;
};





template<typename SerializableClassType, typename DataModelType>
struct SerialiazableClassAlignment {
    template <std::size_t start, std::size_t end, std::size_t inc>
    static constexpr typename std::enable_if<(start < end), std::size_t>::type constexpr_for(std::size_t current_alignment = 0)
    {
        // start is iterated; (the template recursion performs iteration)
        using FieldType = typename boost::pfr::tuple_element_t<start, SerializableClassType>;
        static_assert(IsSupportedType<FieldType>::value, "Field type not supported.");

        std::size_t field_alignment = DataModelType::template GetAlignment<FieldType>();
        current_alignment = (field_alignment > current_alignment) ? field_alignment : current_alignment;

        return constexpr_for<start + inc, end, inc>(current_alignment);
    }

    template <std::size_t start, std::size_t end, std::size_t inc>
    static constexpr typename std::enable_if<!(start < end), std::size_t>::type constexpr_for(std::size_t current_alignment = 0)
    {
        return current_alignment;
    }

    static constexpr std::size_t value = constexpr_for<0, boost::pfr::tuple_size_v<SerializableClassType>, 1>();
};

struct TEST_SerialiazableClassAlignment{
    using Simple32BitDataModel = DataModel<
        1,  1,
        1,  1,
        1,  1,
        2,  2,
        
        2,  2,
        2,  2,
        
        4,  4,
        4,  4,
        
        4,  4,
        4,  4,
        
        8,  8,
        8,  8,
        
        4,  4,
        8,  8,
        8,  8,
        
        1,  1
    >;
    
    struct Empty{

    };

    struct OneByte{
        uint8_t one;
    };

    struct TwoBytes{
        uint16_t two;
    };

    struct ArbitraryFields1{
        uint8_t first;
        uint32_t second;
        uint16_t last;
    };

    static_assert(SerialiazableClassAlignment<Empty, Simple32BitDataModel>::value == 0, "Empty struct should have no alignment.");
    static_assert(SerialiazableClassAlignment<OneByte, Simple32BitDataModel>::value == 1, "1-byte struct should have 1 alignment.");
    static_assert(SerialiazableClassAlignment<TwoBytes, Simple32BitDataModel>::value == 2, "2-byte struct should have 2 alignment.");
    static_assert(SerialiazableClassAlignment<ArbitraryFields1, Simple32BitDataModel>::value == 4, "Arbitrary struct should have 4 alignment.");
};

template<typename SerializableClassType, typename DataModelType>
struct SerializableClassPaddingIndexes {
    // Helpers (must be defined before these functions)
    template< std::size_t I, class T, std::size_t N >
    static constexpr typename std::enable_if<!(I < N), const T>::type SFINAE_Get(const std::array<T,N>& a) noexcept {
        return T{};
    }

    template< std::size_t I, class T, std::size_t N >
    static constexpr typename std::enable_if<(I < N), const T>::type SFINAE_Get(const std::array<T,N>& a) noexcept {
        return std::get<I>(a);
    }

    template< std::size_t I, class T, std::size_t N >
    static constexpr typename std::enable_if<!(I < N), void>::type SFINAE_Set(std::array<T,N>& a, T val) noexcept {}

    template< std::size_t I, class T, std::size_t N >
    static constexpr typename std::enable_if<(I < N), void>::type SFINAE_Set(std::array<T,N>& a, T val) noexcept {
        std::get<I>(a) = val;
    }

    // To create the compile-time array of padding indexes, ths struct is traversed multiple times returning an array describing the padding in greater detail each time. Note, AFAIK,
    // there isn't a way to combine these traversals since the array's type is bound to its size. In summary:
    //   1. Count the number of locations padding occurs; this information is used in the next step (because the size of a array is bound the array type)
    //   2. For each location where padding occurs, count the number padding bytes.
    //   3. For each location where padding occurs, write the index of each byte to an array (using iteration with the previously found constexpr padding byte count). 

    //vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvSTEP 1vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
    template <std::size_t start, std::size_t end, std::size_t inc>
    static constexpr typename std::enable_if<(start < end), std::size_t>::type PaddingLocationsCount(std::size_t padding_locations_count = 0, std::size_t current_alignment_filled = 0)
    {
        // start is iterated; (the template recursion performs iteration)
        using FieldType = typename boost::pfr::tuple_element_t<start, SerializableClassType>;
        static_assert(IsSupportedType<FieldType>::value, "Field type not supported.");
        
        constexpr std::size_t field_size = DataModelType::template GetSize<FieldType>();
        constexpr std::size_t field_alignment = DataModelType::template GetAlignment<FieldType>();
        
        // detect when padding should be inserted (including trailing padding)
        if(current_alignment_filled % field_alignment != 0) {
            padding_locations_count++;
            std::size_t padding_bytes = (current_alignment_filled > field_alignment) ? current_alignment_filled % field_alignment : field_alignment - current_alignment_filled;
            current_alignment_filled += padding_bytes;
        }

        // account for the field
        current_alignment_filled += field_size;

        return PaddingLocationsCount<start + inc, end, inc>(padding_locations_count, current_alignment_filled);
    }

    template <std::size_t start, std::size_t end, std::size_t inc>
    static constexpr typename std::enable_if<!(start < end), std::size_t>::type PaddingLocationsCount(std::size_t padding_locations_count = 0, std::size_t current_alignment_filled = 0)
    {
        constexpr std::size_t struct_alignment = SerialiazableClassAlignment<SerializableClassType, DataModelType>::value;

        // account for trailing padding
        if(struct_alignment > 0) { // when processing empty struct, its alignment will be 0 and will cause a divide-by-zero error.
            if(current_alignment_filled % struct_alignment != 0) {
                padding_locations_count++;
            }
        }

        return padding_locations_count;
    }

    static constexpr std::size_t padding_locations_count = PaddingLocationsCount<0, boost::pfr::tuple_size_v<SerializableClassType>, 1>();
    //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^STEP 1^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    //vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvSTEP 2vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
    using PaddingLocationsPaddingByteCountType = std::array<std::size_t, padding_locations_count>;

    template <std::size_t start, std::size_t end, std::size_t inc, std::size_t append_index = 0>
    static constexpr typename std::enable_if<(start < end), PaddingLocationsPaddingByteCountType>::type PaddingLocationsPaddingByteCount(PaddingLocationsPaddingByteCountType locations_byte_count = PaddingLocationsPaddingByteCountType{}, std::size_t current_alignment_filled = 0)
    {
        // start is iterated; (the template recursion performs iteration)
        using FieldType = typename boost::pfr::tuple_element_t<start, SerializableClassType>;
        static_assert(IsSupportedType<FieldType>::value, "Field type not supported.");
        
        constexpr std::size_t field_size = DataModelType::template GetSize<FieldType>();
        constexpr std::size_t field_alignment = DataModelType::template GetAlignment<FieldType>();

        bool is_locations_byte_count_written = false;
        
        // detect when padding should be inserted
        if(current_alignment_filled % field_alignment != 0) {
            std::size_t padding_bytes = (current_alignment_filled > field_alignment) ? current_alignment_filled % field_alignment : field_alignment - current_alignment_filled;
            current_alignment_filled += padding_bytes;
            SFINAE_Set<append_index>(locations_byte_count, padding_bytes);
            is_locations_byte_count_written = true;
        }

        // account for the field
        current_alignment_filled += field_size;

        if(is_locations_byte_count_written) {
            return PaddingLocationsPaddingByteCount<start + inc, end, inc, append_index + 1>(locations_byte_count, current_alignment_filled);
        } else {
            return PaddingLocationsPaddingByteCount<start + inc, end, inc, append_index>(locations_byte_count, current_alignment_filled);
        }
    }

    template <std::size_t start, std::size_t end, std::size_t inc, std::size_t append_index = 0>
    static constexpr typename std::enable_if<!(start < end), PaddingLocationsPaddingByteCountType>::type PaddingLocationsPaddingByteCount(PaddingLocationsPaddingByteCountType locations_byte_count = PaddingLocationsPaddingByteCountType{}, std::size_t current_alignment_filled = 0)
    {
        constexpr std::size_t struct_alignment = SerialiazableClassAlignment<SerializableClassType, DataModelType>::value;

        // account for trailing padding
        if(struct_alignment > 0) { // when processing empty struct, its alignment will be 0 and will cause a divide-by-zero error.
            if(current_alignment_filled % struct_alignment != 0) {
                std::size_t padding_bytes = struct_alignment - (current_alignment_filled % struct_alignment);
                SFINAE_Set<append_index>(locations_byte_count, padding_bytes);
            }
        }
        return locations_byte_count;
    }

    static constexpr PaddingLocationsPaddingByteCountType padding_locations_padding_byte_count = PaddingLocationsPaddingByteCount<0, boost::pfr::tuple_size_v<SerializableClassType>, 1>();
    //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^STEP 2^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    //vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvSTEP 2.5vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
    template <std::size_t start, std::size_t end, std::size_t inc>
    static constexpr typename std::enable_if<(start < end), std::size_t>::type PaddingBytesCount(std::size_t padding_bytes_count = 0)
    {
       padding_bytes_count += std::get<start>(padding_locations_padding_byte_count);
       return PaddingBytesCount<start + inc, end, inc>(padding_bytes_count);
    }

    template <std::size_t start, std::size_t end, std::size_t inc>
    static constexpr typename std::enable_if<!(start < end), std::size_t>::type PaddingBytesCount(std::size_t padding_bytes_count = 0)
    {
       return padding_bytes_count;
    }

    static constexpr std::size_t padding_bytes_count = PaddingBytesCount<0, padding_locations_count, 1>();
    //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^STEP 2.5^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    //vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvSTEP 3vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
    using PaddingByteIndexesType = std::array<std::size_t, padding_bytes_count>;

    template <std::size_t start, std::size_t end, std::size_t inc, std::size_t append_index = 0>
    static constexpr typename std::enable_if<(start < end), PaddingByteIndexesType>::type AppendHelper_PaddingByteIndexes(PaddingByteIndexesType padding_byte_indexes, std::size_t first_padding_byte_index)
    {
        SFINAE_Set<append_index>(padding_byte_indexes, first_padding_byte_index);

        return AppendHelper_PaddingByteIndexes<start + inc, end, inc, append_index + 1>(padding_byte_indexes, first_padding_byte_index + 1);
    }

    template <std::size_t start, std::size_t end, std::size_t inc, std::size_t append_index = 0>
    static constexpr typename std::enable_if<!(start < end), PaddingByteIndexesType>::type AppendHelper_PaddingByteIndexes(PaddingByteIndexesType padding_byte_indexes, std::size_t first_padding_byte_index)
    {
        return padding_byte_indexes;
    }

    template <std::size_t start, std::size_t end, std::size_t inc, std::size_t padding_locations_padding_byte_count_index = 0, std::size_t padding_byte_indexes_append_index = 0>
    static constexpr typename std::enable_if<(start < end), PaddingByteIndexesType>::type PaddingByteIndexes(PaddingByteIndexesType padding_byte_indexes = PaddingByteIndexesType{}, std::size_t current_alignment_filled = 0)
    {
        // start is iterated; (the template recursion performs iteration)
        using FieldType = typename boost::pfr::tuple_element_t<start, SerializableClassType>;
        static_assert(IsSupportedType<FieldType>::value, "Field type not supported.");
        
        constexpr std::size_t struct_alignment = SerialiazableClassAlignment<SerializableClassType, DataModelType>::value;
        constexpr std::size_t field_size = DataModelType::template GetSize<FieldType>();
        constexpr std::size_t field_alignment = DataModelType::template GetAlignment<FieldType>();

        bool is_locations_byte_count_written = false;
        constexpr std::size_t padding_bytes = SFINAE_Get<padding_locations_padding_byte_count_index>(padding_locations_padding_byte_count);
        // detect when padding should be inserted (including trailing padding)
        if(current_alignment_filled % field_alignment != 0) {
            padding_byte_indexes = AppendHelper_PaddingByteIndexes<0, padding_bytes, 1, padding_byte_indexes_append_index>(padding_byte_indexes, current_alignment_filled);
            is_locations_byte_count_written = true;

            current_alignment_filled += padding_bytes;
        }

        // account for the field
        current_alignment_filled += field_size;

        if(is_locations_byte_count_written) {
            return PaddingByteIndexes<start + inc, end, inc, padding_locations_padding_byte_count_index + 1, padding_byte_indexes_append_index + padding_bytes>(padding_byte_indexes, current_alignment_filled);
        } else {
            return PaddingByteIndexes<start + inc, end, inc, padding_locations_padding_byte_count_index, padding_byte_indexes_append_index>(padding_byte_indexes, current_alignment_filled);
        }
    }

    template <std::size_t start, std::size_t end, std::size_t inc, std::size_t padding_locations_padding_byte_count_index = 0, std::size_t padding_byte_indexes_append_index = 0>
    static constexpr typename std::enable_if<!(start < end), PaddingByteIndexesType>::type PaddingByteIndexes(PaddingByteIndexesType padding_byte_indexes = PaddingByteIndexesType{}, std::size_t current_alignment_filled = 0)
    {
        constexpr std::size_t struct_alignment = SerialiazableClassAlignment<SerializableClassType, DataModelType>::value;
        constexpr std::size_t padding_bytes = SFINAE_Get<padding_locations_padding_byte_count_index>(padding_locations_padding_byte_count);

        // account for trailing padding
        if(struct_alignment > 0) { // when processing empty struct, its alignment will be 0 and will cause a divide-by-zero error.
            if(current_alignment_filled % struct_alignment != 0) {
                padding_byte_indexes = AppendHelper_PaddingByteIndexes<0, padding_bytes, 1, padding_byte_indexes_append_index>(padding_byte_indexes, current_alignment_filled);
            }
        }
        return padding_byte_indexes;
    }

    static constexpr PaddingByteIndexesType padding_byte_indexes = PaddingByteIndexes<0, boost::pfr::tuple_size_v<SerializableClassType>, 1>();
    //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^STEP 3^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
};

struct TEST_SerializableClassPaddingIndexes{
    // IMPORTANT: When defining test structures make sure to define them using the exact primitive types. Do not use fixed-width integers becasue the aliasing
    //            to the underlying types is platform dependent and can break these tests.
    using Simple32BitDataModel = DataModel<
        1,  1,
        1,  1,
        1,  1,
        2,  2,
        
        2,  2,
        2,  2,
        
        4,  4,
        4,  4,
        
        8,  8,
        8,  8,
        
        8,  8,
        8,  8,
        
        4,  4,
        8,  8,
        8,  8,
        
        1,  1
    >;
    
    struct Empty{

    };
    static constexpr std::array<std::size_t, 0> expected_padding_locations_padding_byte_count_Empty{};
    static constexpr std::array<std::size_t, 0> expected_padding_indexes_Empty{};

    
    struct NoPadding{
        unsigned short a;
        unsigned char b;
        unsigned char c;
        int d;
    };
    static constexpr std::array<std::size_t, 0> expected_padding_locations_padding_byte_count_NoPadding{};
    static constexpr std::array<std::size_t, 0> expected_padding_indexes_NoPadding{};


    struct OnlyInternalPadding3_Simple{
        unsigned char foo;
        // unsigned char padding[3];
        unsigned int bar;
    };
    static constexpr std::array<std::size_t, 1> expected_padding_locations_padding_byte_count_OnlyInternalPadding3_Simple{3};
    static constexpr std::array<std::size_t, 3> expected_padding_indexes_OnlyInternalPadding3_Simple{1, 2, 3};

    struct OnlyInternalPadding3_DifferentStructAlignment{
        unsigned char foo;
        // unsigned char padding[3];
        unsigned int bar;
        unsigned long buzz;
    };
    static constexpr std::array<std::size_t, 1> expected_padding_locations_padding_byte_count_OnlyInternalPadding3_DifferentStructAlignment{3};
    static constexpr std::array<std::size_t, 3> expected_padding_indexes_OnlyInternalPadding3_DifferentStructAlignment{1, 2, 3};

    struct OnlyInternalPadding1_Simple{
        unsigned char foo;
        // unsigned char padding[1];
        unsigned char bar;
    };
    static constexpr std::array<std::size_t, 1> expected_padding_locations_padding_byte_count_OnlyInternalPadding1_Simple{1};
    static constexpr std::array<std::size_t, 1> expected_padding_indexes_OnlyInternalPadding1_Simple{1};

    struct OnlyInternalPadding1_DifferentStructAlignment{
        unsigned char foo;
        // unsigned char padding[1];
        unsigned short bar;
        unsigned int buzz;
    };
    static constexpr std::array<std::size_t, 1> expected_padding_locations_padding_byte_count_OnlyInternalPadding1_DifferentStructAlignment{1};
    static constexpr std::array<std::size_t, 1> expected_padding_indexes_OnlyInternalPadding1_DifferentStructAlignment{1};

    struct OnlyTrailingPadding3_Simple{
        unsigned int bar;
        unsigned char foo;
        // unsigned char padding[3];
    };
    static constexpr std::array<std::size_t, 1> expected_padding_locations_padding_byte_count_OnlyTrailingPadding3_Simple{3};
    static constexpr std::array<std::size_t, 3> expected_padding_indexes_OnlyTrailingPadding3_Simple{5, 6, 7};

    struct OnlyTrailingPadding3_DifferentStructAlignment{
        unsigned long buzz;
        unsigned int bar;
        unsigned char foo;
        // unsigned char padding[3];
    };
    static constexpr std::array<std::size_t, 1> expected_padding_locations_padding_byte_count_OnlyTrailingPadding3_DifferentStructAlignment{3};
    static constexpr std::array<std::size_t, 3> expected_padding_indexes_OnlyTrailingPadding3_DifferentStructAlignment{13, 14, 15};

    struct ArbitraryInternalMultiPadding1Then2 {
        char foo;
        // unsigned char padding[1];
        short bar;
        float a;
        short b;
        // unsigned char padding[2];
        int c;
    };
    static constexpr std::array<std::size_t, 2> expected_padding_locations_padding_byte_count_ArbitraryInternalMultiPadding1Then2{1, 2};
    static constexpr std::array<std::size_t, 3> expected_padding_indexes_ArbitraryInternalMultiPadding1Then2{1, 10, 11};

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
    static constexpr std::array<std::size_t, 3> expected_padding_locations_padding_byte_count_ArbitraryMixedPaddings1Then2Then3{1, 2, 3};
    static constexpr std::array<std::size_t, 6> expected_padding_indexes_ArbitraryMixedPaddings1Then2Then3{1, 10, 11, 17, 18, 19};

    struct ArbitraryMixedPaddingOneDataMemberBetween {
        long bar;
        char baz;
        // unsigned char padding[1]
        short foo;
        // unsigned char padding[4]
    };
    static constexpr std::array<std::size_t, 2> expected_padding_locations_padding_byte_count_ArbitraryMixedPaddingOneDataMemberBetween{1, 4};
    static constexpr std::array<std::size_t, 5> expected_padding_indexes_ArbitraryMixedPaddingOneDataMemberBetween{9, 12, 13, 14, 15};
    
    // A data model in which 32-bit ints can be aligned to 16-bit words
    using Simple32BitDatModelButIntsNotSelfAligned = DataModel<
        1,  1,
        1,  1,
        1,  1,
        2,  2,
        
        2,  2,
        2,  2,
        
        4,  2,
        4,  2,
        
        8,  8,
        8,  8,
        
        8,  8,
        8,  8,
        
        4,  4,
        8,  8,
        8,  8,
        
        1,  1
    >;
    struct ForNotSelfAligned_ArbitraryMixedPaddings1Then3 {
        char foo;
        // unsigned char padding[1];
        short bar;
        float a;
        short b;
        int c;
        unsigned char d;    // 15 bytes from beginning to here
        // unsigned char padding[1];
    };
    static constexpr std::array<std::size_t, 2> expected_padding_locations_padding_byte_count_ForNotSelfAligned_ArbitraryMixedPaddings1Then3{1, 1};
    static constexpr std::array<std::size_t, 2> expected_padding_indexes_ForNotSelfAligned_ArbitraryMixedPaddings1Then3{1, 15};

    // TODO:
    // - test array data members
    // - test nested struct/class

    // Test padding locations count
    static_assert(SerializableClassPaddingIndexes<Empty, Simple32BitDataModel>::padding_locations_count == 0, "Empty struct should have no padding.");
    static_assert(SerializableClassPaddingIndexes<Empty, Simple32BitDatModelButIntsNotSelfAligned>::padding_locations_count == 0, "Empty struct should have no padding.");
    static_assert(SerializableClassPaddingIndexes<NoPadding, Simple32BitDataModel>::padding_locations_count == 0, "Struct should have specified padding.");
    static_assert(SerializableClassPaddingIndexes<NoPadding, Simple32BitDatModelButIntsNotSelfAligned>::padding_locations_count == 0, "Struct should have specified padding.");

    static_assert(SerializableClassPaddingIndexes<OnlyInternalPadding3_Simple, Simple32BitDataModel>::padding_locations_count == 1, "Struct should have specified padding.");
    static_assert(SerializableClassPaddingIndexes<OnlyInternalPadding3_DifferentStructAlignment, Simple32BitDataModel>::padding_locations_count == 1, "Struct should have specified padding.");
    
    static_assert(SerializableClassPaddingIndexes<OnlyInternalPadding1_Simple, Simple32BitDataModel>::padding_locations_count == 1, "Struct should have specified padding.");
    static_assert(SerializableClassPaddingIndexes<OnlyInternalPadding1_DifferentStructAlignment, Simple32BitDataModel>::padding_locations_count == 1, "Struct should have specified padding.");
 
    static_assert(SerializableClassPaddingIndexes<OnlyTrailingPadding3_Simple, Simple32BitDataModel>::padding_locations_count == 1, "Struct should have specified padding.");
    static_assert(SerializableClassPaddingIndexes<OnlyTrailingPadding3_DifferentStructAlignment, Simple32BitDataModel>::padding_locations_count == 1, "Struct should have specified padding.");

    static_assert(SerializableClassPaddingIndexes<ArbitraryInternalMultiPadding1Then2, Simple32BitDataModel>::padding_locations_count == 2, "Struct should have specified padding.");
    static_assert(SerializableClassPaddingIndexes<ArbitraryMixedPaddings1Then2Then3, Simple32BitDataModel>::padding_locations_count == 3, "Struct should have specified padding.");
    static_assert(SerializableClassPaddingIndexes<ArbitraryMixedPaddingOneDataMemberBetween, Simple32BitDataModel>::padding_locations_count == 2, "Struct should have specified padding.");

    static_assert(SerializableClassPaddingIndexes<ForNotSelfAligned_ArbitraryMixedPaddings1Then3, Simple32BitDatModelButIntsNotSelfAligned>::padding_locations_count == 2, "Struct should have specified padding.");

    // Test padding locations padding bytes count
    static_assert(SerializableClassPaddingIndexes<Empty, Simple32BitDataModel>::padding_locations_padding_byte_count == expected_padding_locations_padding_byte_count_Empty, "Empty struct should have no padding.");
    static_assert(SerializableClassPaddingIndexes<NoPadding, Simple32BitDataModel>::padding_locations_padding_byte_count == expected_padding_locations_padding_byte_count_NoPadding, "Struct should have specified padding.");

    static_assert(SerializableClassPaddingIndexes<OnlyInternalPadding3_Simple, Simple32BitDataModel>::padding_locations_padding_byte_count == expected_padding_locations_padding_byte_count_OnlyInternalPadding3_Simple, "Struct should have specified padding.");
    static_assert(SerializableClassPaddingIndexes<OnlyInternalPadding3_DifferentStructAlignment, Simple32BitDataModel>::padding_locations_padding_byte_count == expected_padding_locations_padding_byte_count_OnlyInternalPadding3_DifferentStructAlignment, "Struct should have specified padding.");
    
    static_assert(SerializableClassPaddingIndexes<OnlyInternalPadding1_Simple, Simple32BitDataModel>::padding_locations_padding_byte_count == expected_padding_locations_padding_byte_count_OnlyInternalPadding1_Simple, "Struct should have specified padding.");
    static_assert(SerializableClassPaddingIndexes<OnlyInternalPadding1_DifferentStructAlignment, Simple32BitDataModel>::padding_locations_padding_byte_count == expected_padding_locations_padding_byte_count_OnlyInternalPadding1_DifferentStructAlignment, "Struct should have specified padding.");
 
    static_assert(SerializableClassPaddingIndexes<OnlyTrailingPadding3_Simple, Simple32BitDataModel>::padding_locations_padding_byte_count == expected_padding_locations_padding_byte_count_OnlyTrailingPadding3_Simple, "Struct should have specified padding.");
    static_assert(SerializableClassPaddingIndexes<OnlyTrailingPadding3_DifferentStructAlignment, Simple32BitDataModel>::padding_locations_padding_byte_count == expected_padding_locations_padding_byte_count_OnlyTrailingPadding3_DifferentStructAlignment, "Struct should have specified padding.");

    static_assert(SerializableClassPaddingIndexes<ArbitraryInternalMultiPadding1Then2, Simple32BitDataModel>::padding_locations_padding_byte_count == expected_padding_locations_padding_byte_count_ArbitraryInternalMultiPadding1Then2, "Struct should have specified padding.");
    static_assert(SerializableClassPaddingIndexes<ArbitraryMixedPaddings1Then2Then3, Simple32BitDataModel>::padding_locations_padding_byte_count == expected_padding_locations_padding_byte_count_ArbitraryMixedPaddings1Then2Then3, "Struct should have specified padding.");
    static_assert(SerializableClassPaddingIndexes<ArbitraryMixedPaddingOneDataMemberBetween, Simple32BitDataModel>::padding_locations_padding_byte_count == expected_padding_locations_padding_byte_count_ArbitraryMixedPaddingOneDataMemberBetween, "Struct should have specified padding.");

    static_assert(SerializableClassPaddingIndexes<ForNotSelfAligned_ArbitraryMixedPaddings1Then3, Simple32BitDatModelButIntsNotSelfAligned>::padding_locations_padding_byte_count == expected_padding_locations_padding_byte_count_ForNotSelfAligned_ArbitraryMixedPaddings1Then3, "Struct should have specified padding.");

    // Test padding byte count
    static_assert(SerializableClassPaddingIndexes<Empty, Simple32BitDataModel>::padding_bytes_count == 0, "Empty struct should have no padding.");
    static_assert(SerializableClassPaddingIndexes<NoPadding, Simple32BitDataModel>::padding_bytes_count == 0, "Struct should have specified padding.");

    static_assert(SerializableClassPaddingIndexes<OnlyInternalPadding3_Simple, Simple32BitDataModel>::padding_bytes_count == 3, "Struct should have specified padding.");
    static_assert(SerializableClassPaddingIndexes<OnlyInternalPadding3_DifferentStructAlignment, Simple32BitDataModel>::padding_bytes_count == 3, "Struct should have specified padding.");
    
    static_assert(SerializableClassPaddingIndexes<OnlyInternalPadding1_Simple, Simple32BitDataModel>::padding_bytes_count == 1, "Struct should have specified padding.");
    static_assert(SerializableClassPaddingIndexes<OnlyInternalPadding1_DifferentStructAlignment, Simple32BitDataModel>::padding_bytes_count == 1, "Struct should have specified padding.");
 
    static_assert(SerializableClassPaddingIndexes<OnlyTrailingPadding3_Simple, Simple32BitDataModel>::padding_bytes_count == 3, "Struct should have specified padding.");
    static_assert(SerializableClassPaddingIndexes<OnlyTrailingPadding3_DifferentStructAlignment, Simple32BitDataModel>::padding_bytes_count == 3, "Struct should have specified padding.");

    static_assert(SerializableClassPaddingIndexes<ArbitraryInternalMultiPadding1Then2, Simple32BitDataModel>::padding_bytes_count == 3, "Struct should have specified padding.");
    static_assert(SerializableClassPaddingIndexes<ArbitraryMixedPaddings1Then2Then3, Simple32BitDataModel>::padding_bytes_count == 6, "Struct should have specified padding.");
    static_assert(SerializableClassPaddingIndexes<ArbitraryMixedPaddingOneDataMemberBetween, Simple32BitDataModel>::padding_bytes_count == 5, "Struct should have specified padding.");

    static_assert(SerializableClassPaddingIndexes<ForNotSelfAligned_ArbitraryMixedPaddings1Then3, Simple32BitDatModelButIntsNotSelfAligned>::padding_bytes_count == 2, "Struct should have specified padding.");

    // Test padding indexes method
    static_assert(SerializableClassPaddingIndexes<Empty, Simple32BitDataModel>::padding_byte_indexes == expected_padding_indexes_Empty, "Empty struct should have no padding.");
    static_assert(SerializableClassPaddingIndexes<NoPadding, Simple32BitDataModel>::padding_byte_indexes == expected_padding_indexes_NoPadding, "Struct should have specified padding.");

    static_assert(SerializableClassPaddingIndexes<OnlyInternalPadding3_Simple, Simple32BitDataModel>::padding_byte_indexes == expected_padding_indexes_OnlyInternalPadding3_Simple, "Struct should have specified padding.");
    static_assert(SerializableClassPaddingIndexes<OnlyInternalPadding3_DifferentStructAlignment, Simple32BitDataModel>::padding_byte_indexes == expected_padding_indexes_OnlyInternalPadding3_DifferentStructAlignment, "Struct should have specified padding.");
    
    static_assert(SerializableClassPaddingIndexes<OnlyInternalPadding1_Simple, Simple32BitDataModel>::padding_byte_indexes == expected_padding_indexes_OnlyInternalPadding1_Simple, "Struct should have specified padding.");
    static_assert(SerializableClassPaddingIndexes<OnlyInternalPadding1_DifferentStructAlignment, Simple32BitDataModel>::padding_byte_indexes == expected_padding_indexes_OnlyInternalPadding1_DifferentStructAlignment, "Struct should have specified padding.");
 
    static_assert(SerializableClassPaddingIndexes<OnlyTrailingPadding3_Simple, Simple32BitDataModel>::padding_byte_indexes == expected_padding_indexes_OnlyTrailingPadding3_Simple, "Struct should have specified padding.");
    static_assert(SerializableClassPaddingIndexes<OnlyTrailingPadding3_DifferentStructAlignment, Simple32BitDataModel>::padding_byte_indexes == expected_padding_indexes_OnlyTrailingPadding3_DifferentStructAlignment, "Struct should have specified padding.");

    static_assert(SerializableClassPaddingIndexes<ArbitraryInternalMultiPadding1Then2, Simple32BitDataModel>::padding_byte_indexes == expected_padding_indexes_ArbitraryInternalMultiPadding1Then2, "Struct should have specified padding.");
    static_assert(SerializableClassPaddingIndexes<ArbitraryMixedPaddings1Then2Then3, Simple32BitDataModel>::padding_byte_indexes == expected_padding_indexes_ArbitraryMixedPaddings1Then2Then3, "Struct should have specified padding.");
    static_assert(SerializableClassPaddingIndexes<ArbitraryMixedPaddingOneDataMemberBetween, Simple32BitDataModel>::padding_byte_indexes == expected_padding_indexes_ArbitraryMixedPaddingOneDataMemberBetween, "Struct should have specified padding.");

    static_assert(SerializableClassPaddingIndexes<ForNotSelfAligned_ArbitraryMixedPaddings1Then3, Simple32BitDatModelButIntsNotSelfAligned>::padding_byte_indexes == expected_padding_indexes_ForNotSelfAligned_ArbitraryMixedPaddings1Then3, "Struct should have specified padding.");
};

int main() {

    auto thing_under_debug = SerializableClassPaddingIndexes<TEST_SerializableClassPaddingIndexes::ForNotSelfAligned_ArbitraryMixedPaddings1Then3, TEST_SerializableClassPaddingIndexes::Simple32BitDatModelButIntsNotSelfAligned>::padding_byte_indexes;
    std::cout << "# of elements = " << thing_under_debug.size() << std::endl;

    for(int i = 0; i < thing_under_debug.size(); i++) {
        std::cout << thing_under_debug[i] << std::endl;
    }

    return 0;
}
