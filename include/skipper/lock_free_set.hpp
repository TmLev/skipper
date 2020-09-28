#ifndef SKIPPER_LOCK_FREE_SET_HPP
#define SKIPPER_LOCK_FREE_SET_HPP

#include <atomic>
#include <memory>
#include <vector>

#include "skipper/detail/allocator.hpp"
#include "skipper/detail/arena.hpp"

namespace skipper {

template <typename T, class TAllocator = skipper::detail::Arena>
class LockFreeSkipListSet {
 private:
  struct Node;

 public:
  using Level = int;
  using Probability = double;

  static constexpr auto kMaxLevel = Level{4};
  static constexpr auto kProbability = Probability{0.2};

 public:
  LockFreeSkipListSet();

  LockFreeSkipListSet(LockFreeSkipListSet&& other) = delete;
  LockFreeSkipListSet(const LockFreeSkipListSet& other) = delete;
  LockFreeSkipListSet& operator=(LockFreeSkipListSet&& other) = delete;
  LockFreeSkipListSet& operator=(const LockFreeSkipListSet& other) = delete;

  auto Contains(const T& value) -> bool;
  auto Insert(const T& value) -> bool;

 private:
  using Allocator = skipper::detail::Allocator;
  using AllocatorPtr = std::shared_ptr<Allocator>;

  using Flag = std::atomic<bool>;
  using NodePtr = std::atomic<Node*>;
  using NodePtrList = std::vector<NodePtr>;

 private:
  struct FindResult;

 private:
  auto New(const T& value, Level level) -> Node*;
  auto Find(const T& value) -> FindResult;
  auto GenerateRandomLevel() -> Level;

 private:
  AllocatorPtr allocator_{std::make_shared<TAllocator>()};
  NodePtr head_{New({}, kMaxLevel + 1)};
  NodePtr tail_{New({}, kMaxLevel + 1)};
};

}  // namespace skipper

#endif  // SKIPPER_LOCK_FREE_SET_HPP

#include "skipper/lock_free_set.ipp"
