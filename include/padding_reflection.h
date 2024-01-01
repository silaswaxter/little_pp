// ABOUT: The public API for LittlePP's padding reflection features
#ifndef LITTLE_PP_PADDING_REFLECTION_H
#define LITTLE_PP_PADDING_REFLECTION_H

#include "impl/padding_reflection.h"

namespace little_pp {

namespace padding_reflection {

// Only supporting 8-bit bytes for now to avoid scope creep.
// Within the embedded space (motivation for LittlePP in the first place),
// non-8-bit byte architectures do still exist and are not just relics of the
// past. See this active TI DSP and this discussion:
// - https://www.ti.com/product/TMS320F2809
// - https://groups.google.com/g/comp.lang.c++/c/14QZUvWE_Xc
const std::size_t kRequiredCharBit = 8;
static_assert(CHAR_BIT == kRequiredCharBit,
              "LittlePP requires 8-bit bytes at this time. "
              "(Rationale explained in comment above.)");

// Violate the google style guide in favor of std library convention.
// NOLINTBEGIN(readability-identifier-naming)

// Check if type is serializable.
template <typename T>
constexpr bool is_serializable_type_v =
    little_pp::impl::IsSerializableType<T>::kValue;

// Get the alignment of the serializable class type.
template <typename SerializableClassType, typename DataModelType>
constexpr std::size_t serializable_class_alignment_v =
    little_pp::impl::SerializableClassAlignment<SerializableClassType,
                                                DataModelType>::kValue;

// Get the number of padding locations (the number of places where there is one
// or more bytes of consecutive padding).
template <typename SerializableClassType, typename DataModelType>
constexpr std::size_t serializable_class_padding_locations_v =
    little_pp::impl::SerializableClassPaddingLocations<SerializableClassType,
                                                       DataModelType>::kValue;

// Get an array where each element (in order) describes the padding location's
// number of padding bytes.
template <typename SerializableClassType, typename DataModelType>
constexpr std::array<std::size_t, serializable_class_padding_locations_v<
                                      SerializableClassType, DataModelType>>
    serializable_class_padding_locations_byte_counts_v =
        little_pp::impl::SerializableClassPaddingLocationsByteCounts<
            SerializableClassType, DataModelType>::kValue;

// Get the total number of padding bytes.
template <typename SerializableClassType, typename DataModelType>
constexpr std::size_t serializable_class_padding_count_v =
    little_pp::impl::SerializableClassPaddingByteCount<SerializableClassType,
                                                       DataModelType>::kValue;

// Get the padding byte indexes (in order)
template <typename SerializableClassType, typename DataModelType>
constexpr std::array<std::size_t, serializable_class_padding_count_v<
                                      SerializableClassType, DataModelType>>
    serializable_class_padding_indexes_v =
        little_pp::impl::SerializableClassPaddingIndexes<SerializableClassType,
                                                         DataModelType>::kValue;
// NOLINTEND(readability-identifier-naming)

}  // namespace padding_reflection

}  // namespace little_pp

#endif  // LITTLE_PP_PADDING_REFLECTION_H
