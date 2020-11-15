#ifndef SKIPPER_LOCK_FREE_SET_IPP
#define SKIPPER_LOCK_FREE_SET_IPP

#include <utility>

#include "skipper/lock_free_set.hpp"

namespace skipper {

////////////////////////////////////////////////////////////////////////////////

template <typename T, class TAllocator>
struct LockFreeSkipListSet<T, TAllocator>::Node {
 public:
  Node(T val, Level level);

 public:
  T value;
  NodePtrList forward;
  Flag is_erased{false};
};

template <typename T, class TAllocator>
LockFreeSkipListSet<T, TAllocator>::Node::Node(T val, Level level)
    : value(std::move(val)), forward(static_cast<std::size_t>(level) + 1) {
}

////////////////////////////////////////////////////////////////////////////////

template <typename T, class TAllocator>
struct LockFreeSkipListSet<T, TAllocator>::FindResult {
 public:
  bool found;
  NodePtrList predecessors{static_cast<std::size_t>(kMaxLevel) + 1};
  NodePtrList successors{static_cast<std::size_t>(kMaxLevel) + 1};
};

////////////////////////////////////////////////////////////////////////////////

template <typename T, class TAllocator>
LockFreeSkipListSet<T, TAllocator>::LockFreeSkipListSet() {
  auto head = head_.load();
  for (auto& f : head->forward) {
    f.store(tail_);
  }
}

template <typename T, class TAllocator>
auto LockFreeSkipListSet<T, TAllocator>::Contains(const T& value) -> bool {
  auto [found, s, p] = Find(value);
  return found;
}

template <typename T, class TAllocator>
auto LockFreeSkipListSet<T, TAllocator>::Insert(const T& value) -> bool {
  auto node_level = GenerateRandomLevel();

  while (true) {
    auto [found, predecessors, successors] = Find(value);
    if (found) {
      return false;
    }

    auto node = New(value, node_level);
    if (!node) {
      return false;
    }

    for (auto level = 0; level <= node_level; ++level) {
      auto i = static_cast<std::size_t>(level);
      auto succ = successors[i].load();
      node->forward[i].store(succ);
    }

    auto pred = predecessors[0].load();
    auto succ = successors[0].load();

    if (!pred->forward[0].compare_exchange_strong(succ, node)) {
      continue;
    }

    for (auto level = 1; level <= node_level; ++level) {
      auto i = static_cast<std::size_t>(level);
      if (node->is_erased.load()) {
        return false;
      }

      while (true) {
        pred = predecessors[i];
        succ = successors[i];
        if (pred->forward[i].compare_exchange_strong(succ, node)) {
          break;
        }

        auto res = Find(value);
        predecessors = std::move(res.predecessors);
        successors = std::move(res.successors);
      }
    }

    return true;
  }
}

////////////////////////////////////////////////////////////////////////////////

template <typename T, class TAllocator>
auto LockFreeSkipListSet<T, TAllocator>::New(const T& value, Level level)
    -> LockFreeSkipListSet::Node* {
  if (auto raw = allocator_->Allocate(sizeof(Node))) {
    return new (raw) Node(value, level);
  } else {
    return nullptr;
  }
}

template <typename T, class TAllocator>
auto LockFreeSkipListSet<T, TAllocator>::Find(const T& value)
    -> LockFreeSkipListSet::FindResult {
  auto result = FindResult{};

retry:
  while (true) {
    auto pred = head_.load();
    auto curr = NodePtr{}.load();

    for (auto level = kMaxLevel; level >= 0; --level) {
      auto i = static_cast<std::size_t>(level);

      curr = pred->forward[i];
      while (true) {
        auto succ = curr->forward[i].load();

        while (curr->is_erased.load()) {
          if (!pred->forward[i].compare_exchange_strong(curr, succ)) {
            goto retry;
          }

          curr = pred->forward[i];
          succ = curr->forward[i];
        }

        if (curr != tail_ && curr->value < value) {
          pred = std::exchange(curr, succ);
        } else {
          break;
        }
      }

      result.predecessors[i] = pred;
      result.successors[i] = curr;
    }

    result.found = curr != tail_ && curr->value == value;
    return result;
  }
}

template <typename T, class TAllocator>
auto LockFreeSkipListSet<T, TAllocator>::GenerateRandomLevel()
    -> LockFreeSkipListSet::Level {
  auto level = Level{0};
  while (level < kMaxLevel &&
         static_cast<Probability>(std::rand()) / RAND_MAX < kProbability) {
    ++level;
  }
  return level;
}

}  // namespace skipper

#endif  // SKIPPER_LOCK_FREE_SET_IPP
