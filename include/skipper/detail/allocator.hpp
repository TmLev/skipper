#ifndef SKIPPER_DETAIL_ALLOCATOR_HPP
#define SKIPPER_DETAIL_ALLOCATOR_HPP

#include <cstddef>  // std::size_t

namespace skipper::detail {

struct Allocator {
  virtual auto Allocate(std::size_t bytes) -> char* = 0;

  virtual ~Allocator() = default;
};

}  // namespace skipper::detail

#endif  // SKIPPER_DETAIL_ALLOCATOR_HPP
