#ifndef SKIPPER_CONCURRENT_SET_IPP
#define SKIPPER_CONCURRENT_SET_IPP

#include <algorithm>

#include "skipper/concurrent_set.hpp"

namespace skipper {

////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct ConcurrentSkipListSet<T>::Node {
 public:
  Node(T v, Level level);

 public:
  T value;
  Level level;
  NodePtrList forward;

  Lock lock;
  Flag is_erased{false};  // Is node erased from the list?
  Flag is_linked{false};  // Is node fully linked on all levels?
};

template <typename T>
ConcurrentSkipListSet<T>::Node::Node(T val, Level lvl)
    : value(std::move(val)),
      level(lvl),
      forward(static_cast<std::size_t>(lvl) + 1) {
}

////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct ConcurrentSkipListSet<T>::FindResult {
 public:
  MaybeLevel level{std::nullopt};
  NodePtrList predecessors{static_cast<std::size_t>(kMaxLevel) + 1};
  NodePtrList successors{static_cast<std::size_t>(kMaxLevel) + 1};
};

////////////////////////////////////////////////////////////////////////////////

template <typename T>
ConcurrentSkipListSet<T>::ConcurrentSkipListSet() {
  std::fill(std::begin(head_->forward), std::end(head_->forward), tail_);
}

template <typename T>
auto ConcurrentSkipListSet<T>::Contains(const T& value) -> bool {
  if (auto [maybe_level, _, successors] = Find(value); !maybe_level) {
    return false;
  } else {
    auto level = static_cast<std::size_t>(maybe_level.value());
    auto is_linked = successors[level]->is_linked.load();
    auto is_erased = successors[level]->is_erased.load();
    return is_linked && !is_erased;
  }
}

// Start with finding predecessors and successors for the node,
// which will be (possibly) inserted.
//
// If such node is already present and not yet erased,
// spin until it is fully linked. Otherwise, start again.
//
// Now verify that all of the node's predecessors and successors
// are fully linked, not erased and adjacent to each other.
// Return if not. Otherwise, insert the node and mark it as fully linked.
//
template <typename T>
auto ConcurrentSkipListSet<T>::Insert(const T& value) -> bool {
  auto node_level = GenerateRandomLevel();

  while (true) {
    auto [maybe_level, predecessors, successors] = Find(value);
    if (maybe_level) {
      auto level = static_cast<std::size_t>(maybe_level.value());
      auto node = successors[level];

      if (!node->is_erased.load()) {
        while (!node->is_linked.load()) {
        }

        return false;
      }

      continue;
    }

    auto guards = GuardList{};
    auto valid = true;

    for (auto level = 0; valid && level <= node_level; ++level) {
      auto i = static_cast<std::size_t>(level);
      auto pred = predecessors[i];
      auto succ = successors[i];
      guards.emplace_back(pred->lock);

      auto pred_is_erased = pred->is_erased.load();
      auto succ_is_erased = succ->is_erased.load();
      auto linked = pred->forward[i] == succ;
      valid = !pred_is_erased && !succ_is_erased && linked;
    }

    if (!valid) {
      continue;
    }

    auto node = std::make_shared<Node>(value, node_level);
    for (auto level = 0; level <= node_level; ++level) {
      auto i = static_cast<std::size_t>(level);
      node->forward[i] = successors[i];
      predecessors[i]->forward[i] = node;
    }
    node->is_linked.store(true);

    return true;
  }
}

// First, find possible candidate for erasion by calling `Find`.
// Return if no node was found.
//
// Second, lock possible candidate and check whether it was erased by another
// thread and return if so. Hold the lock until the node is erased.
//
// Third, mark candidate as erased and lock all of its predecessors.
// If all of them are still adjacent to candidate and not one of them is erased,
// physically remove candidate from the list.
// Otherwise, collect new predecessors of the candidate while holding the lock.
//
template <typename T>
auto ConcurrentSkipListSet<T>::Erase(const T& value) -> bool {
  auto candidate = NodePtr{};
  auto maybe_node_level = MaybeLevel{};
  auto maybe_guard = MaybeGuard{};

  while (true) {
    auto [maybe_level, predecessors, successors] = Find(value);
    if (maybe_level) {
      auto level = static_cast<std::size_t>(maybe_level.value());
      candidate = successors[level];
    }

    auto has_candidate =
        maybe_guard || (maybe_level && candidate->is_linked.load() &&
                        candidate->level == maybe_level.value() &&
                        !candidate->is_erased.load());
    if (!has_candidate) {
      return false;
    }

    if (!maybe_guard) {
      maybe_node_level.emplace(candidate->level);
      maybe_guard.emplace(candidate->lock);

      if (candidate->is_erased.load()) {
        return false;
      }

      candidate->is_erased.store(true);
    }

    auto guards = GuardList{};
    auto valid = maybe_node_level.has_value();

    for (auto level = 0; valid && level <= maybe_node_level.value(); ++level) {
      auto i = static_cast<std::size_t>(level);
      auto pred = predecessors[i];
      guards.emplace_back(pred->lock);
      valid = !pred->is_erased.load() && pred->forward[i] == candidate;
    }

    if (!valid) {
      continue;
    }

    for (auto level = maybe_node_level.value(); level >= 0; --level) {
      auto i = static_cast<std::size_t>(level);
      predecessors[i]->forward[i] = candidate->forward[i];
    }

    return true;
  }
}

////////////////////////////////////////////////////////////////////////////////

template <typename T>
auto ConcurrentSkipListSet<T>::Find(const T& value)
    -> ConcurrentSkipListSet::FindResult {
  auto result = FindResult{};

  auto pred = head_;

  for (auto level = kMaxLevel; level >= 0; --level) {
    auto i = static_cast<std::size_t>(level);

    auto curr = pred->forward[i];
    while (curr != tail_ && curr->value < value) {
      pred = curr;
      curr = pred->forward[i];
    }

    if (!result.level && curr != tail_ && curr->value == value) {
      result.level.emplace(level);
    }

    result.predecessors[i] = pred;
    result.successors[i] = curr;
  }

  return result;
}

template <typename T>
auto ConcurrentSkipListSet<T>::GenerateRandomLevel()
    -> ConcurrentSkipListSet::Level {
  auto level = Level{0};
  while (level < kMaxLevel &&
         static_cast<Probability>(std::rand()) / RAND_MAX < kProbability) {
    ++level;
  }
  return level;
}

}  // namespace skipper

#endif  // SKIPPER_CONCURRENT_SET_IPP
