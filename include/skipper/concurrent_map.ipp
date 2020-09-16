#ifndef SKIPPER_CONCURRENT_MAP_IPP
#define SKIPPER_CONCURRENT_MAP_IPP

#include <algorithm>

#include "skipper/concurrent_set.hpp"
#include "concurrent_map.hpp"

namespace skipper {

////////////////////////////////////////////////////////////////////////////////
////
//// SequentialSkipListMap::Node
////

template <typename Key, typename Value>
struct ConcurrentSkipListMap<Key, Value>::Node {
 public:
  Node(Key key, Value value, Level level);

 public:
  Key key;
  Value value;
  Level level;
  NodePtrList forward;

  Lock lock;
  Flag is_erased{false};  // Is node erased from the list?
  Flag is_linked{false};  // Is node fully linked on all levels?
};

template <typename Key, typename Value>
ConcurrentSkipListMap<Key, Value>::Node::Node(Key k, Value val, Level lvl)
    : key(std::move(k)),
      value(std::move(val)),
      level(lvl),
      forward(static_cast<std::size_t>(lvl) + 1) {
}

template <typename Key, typename Value>
struct ConcurrentSkipListMap<Key, Value>::FindResult {
 public:
  MaybeLevel level{std::nullopt};
  NodePtrList predecessors{static_cast<std::size_t>(kMaxLevel) + 1};
  NodePtrList successors{static_cast<std::size_t>(kMaxLevel) + 1};
};

////////////////////////////////////////////////////////////////////////////////
////
//// ConcurrentSkipListSet: public interface
////

template <typename Key, typename Value>
ConcurrentSkipListMap<Key, Value>::ConcurrentSkipListMap() {
  std::fill(std::begin(head_->forward), std::end(head_->forward), tail_);
}

template <typename Key, typename Value>
auto ConcurrentSkipListMap<Key, Value>::Contains(const Key& key) -> bool {
  if (auto [maybe_level, _, successors] = Find(key); !maybe_level) {
    return false;
  } else {
    auto level = static_cast<std::size_t>(maybe_level.value());
    auto is_linked = successors[level]->is_linked.load();
    auto is_erased = successors[level]->is_erased.load();
    return is_linked && !is_erased;
  }
}

template <typename Key, typename Value>
auto ConcurrentSkipListMap<Key, Value>::Insert(const Key& key,
                                               const Value& value) -> bool {
  auto node_level = GenerateRandomLevel();

  while (true) {
    auto [maybe_level, predecessors, successors] = Find(key);
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

    auto node = std::make_shared<Node>(key, value, node_level);
    for (auto level = 0; level <= node_level; ++level) {
      auto i = static_cast<std::size_t>(level);
      node->forward[i] = successors[i];
      predecessors[i]->forward[i] = node;
    }

    node->is_linked.store(true);

    return true;
  }
}

template <typename Key, typename Value>
auto ConcurrentSkipListMap<Key, Value>::Erase(const Key& key) -> bool {
  return false;
}

////////////////////////////////////////////////////////////////////////////////
////
//// ConcurrentSkipListSet: private interface
////

template <typename Key, typename Value>
auto ConcurrentSkipListMap<Key, Value>::Find(const Key& key)
    -> ConcurrentSkipListMap::FindResult {
  auto result = FindResult{};
  auto pred = head_;

  for (auto level = kMaxLevel; level >= 0; --level) {
    auto i = static_cast<std::size_t>(level);
    auto curr = pred->forward[i];

    while (curr != tail_ && curr->key < key) {
      pred = curr;
      curr = pred->forward[i];
    }

    if (!result.level && curr != tail_ && curr->key == key) {
      result.level.emplace(level);
    }

    result.predecessors[i] = pred;
    result.successors[i] = curr;
  }

  return result;
}

template <typename Key, typename Value>
auto ConcurrentSkipListMap<Key, Value>::GenerateRandomLevel()
    -> ConcurrentSkipListMap::Level {
  auto level = Level{0};
  while (level < kMaxLevel &&
         static_cast<Probability>(std::rand()) / RAND_MAX < kProbability) {
    ++level;
  }
  return level;
}

}  // namespace skipper

#endif  // SKIPPER_CONCURRENT_MAP_IPP