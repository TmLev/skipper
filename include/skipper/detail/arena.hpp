#ifndef SKIPPER_DETAIL_ARENA_HPP
#define SKIPPER_DETAIL_ARENA_HPP

#include <atomic>
#include <memory>

#include "skipper/detail/allocator.hpp"

namespace skipper::detail {

class Arena : public Allocator {
 private:
  struct Cell;

 public:
  static constexpr auto kMaxSize = 10'000'000;

 public:
  Arena() = default;

  // Copying is not allowed
  Arena(const Arena& other) = delete;
  Arena& operator=(const Arena& other) = delete;

  // Allocator interface
  auto Allocate(std::size_t bytes) -> char* override;

 private:
  using Memory = std::vector<Cell>;
  using Cursor = std::atomic<std::size_t>;

 private:
  Memory memory_{kMaxSize};
  Cursor cursor_{0};
};

}  // namespace skipper::detail

#endif  // SKIPPER_DETAIL_ARENA_HPP

#include "skipper/detail/arena.ipp"
