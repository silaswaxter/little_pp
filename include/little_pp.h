#include <iostream>
#include <array>
#include <climits>

#include <boost/pfr/core.hpp>

#include "data_model.h"

// Only supporting 8-bit bytes for now; author was trying to avoid scope creep.
// Within the embedded space (for which this library was written), non-8-bit 
// byte architectures do still exist and are not just relics of the past. See
// this active TI DSP and this discussion:
// - https://www.ti.com/product/TMS320F2809
// - https://groups.google.com/g/comp.lang.c++/c/14QZUvWE_Xc
static_assert(CHAR_BIT == 8, "LittlePP requires 8-bit bytes at this time. "
        "(Rationale explained in comment above.)");

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

