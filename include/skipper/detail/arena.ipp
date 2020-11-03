#ifndef SKIPPER_DETAIL_ARENA_IPP
#define SKIPPER_DETAIL_ARENA_IPP

#include "skipper/detail/arena.hpp"

namespace skipper::detail {

////////////////////////////////////////////////////////////////////////////////

struct Arena::Cell {
 public:
  ~Cell();

 public:
  char* raw{nullptr};
};

Arena::Cell::~Cell() {
  delete[] raw;
}

////////////////////////////////////////////////////////////////////////////////

auto Arena::Allocate(std::size_t bytes) -> char* {
  auto c = cursor_.fetch_add(1);
  if (c >= kMaxSize) {
    return nullptr;
  }

  auto& cell = memory_[c];
  cell.raw = new char[bytes];

  return cell.raw;
}

}  // namespace skipper::detail

#endif  // SKIPPER_DETAIL_ARENA_IPP
