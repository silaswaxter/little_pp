#ifndef TESTED_DATA_MODELS_H
#define TESTED_DATA_MODELS_H

#include "include/little_pp.h"

namespace test_data {
namespace data_models {
// clang-format off
// NOLINTBEGIN(*-magic-numbers)
using NoPaddingDataModel =
    little_pp::data_model::DataModel<little_pp::data_model::Endianess::kLittleEndian,
                                     1, 1, 1, 1, 1, 1, 2, 1,
                                     2, 1, 2, 1,
                                     4, 1, 4, 1,
                                     8, 1, 8, 1,
                                     8, 1, 8, 1,
                                     4, 1, 8, 1, 8, 1,
                                     1, 1>;

using Simple32BitDataModel =
    little_pp::data_model::DataModel<little_pp::data_model::Endianess::kLittleEndian,
                                     1, 1, 1, 1, 1, 1, 2, 2,
                                     2, 2, 2, 2,
                                     4, 4, 4, 4,
                                     8, 4, 8, 4,
                                     8, 4, 8, 4,
                                     4, 4, 8, 4, 8, 4,
                                     1, 1>;

// NOLINTEND(*-magic-numbers)
// clang-format on
}  // namespace data_models
}  // namespace test_data

#endif  // TESTED_DATA_MODELS_H
