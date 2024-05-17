#ifndef UNIQUE_PADDING_ELEMENT_H
#define UNIQUE_PADDING_ELEMENT_H

#include <type_traits>

namespace little_pp {
namespace unique_padding_element {

struct UniquePaddingElement {
  std::size_t padding_first_index;
  std::size_t padding_span_length;
  bool is_padding_for_first_passed_data_model;
};

}  // namespace unique_padding_element
}  // namespace little_pp

#endif  // UNIQUE_PADDING_ELEMENT_H
