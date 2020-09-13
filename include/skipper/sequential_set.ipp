#ifndef SKIPPER_SEQUENTIAL_SET_IPP
#define SKIPPER_SEQUENTIAL_SET_IPP

#include <algorithm>
#include <iostream>
#include <memory>
#include <utility>

#include "skipper/sequential_set.hpp"

namespace skipper {

////////////////////////////////////////////////////////////////////////////////
////
//// SequentialSkipListSet::Node
////

template <typename T>
struct SequentialSkipListSet<T>::Node {
 public:
  Node(T v, Level level);

  auto Next() const -> Node*;

 public:
  T value;
  NodePtrList forward;
};

template <typename T>
SequentialSkipListSet<T>::Node::Node(T v, Level level)
    : value(std::move(v)), forward(static_cast<std::size_t>(level) + 1) {
}

template <typename T>
auto SequentialSkipListSet<T>::Node::Next() const
    -> SequentialSkipListSet<T>::Node* {
  return forward[0].get();
}

////////////////////////////////////////////////////////////////////////////////
////
//// SequentialSkipListSet::Iterator
////

template <typename T>
SequentialSkipListSet<T>::Iterator::Iterator(SequentialSkipListSet::Node* ptr)
    : ptr_(ptr) {
}

template <typename T>
auto SequentialSkipListSet<T>::Iterator::operator*() const -> const T& {
  return ptr_->value;
}

template <typename T>
auto SequentialSkipListSet<T>::Iterator::operator->() const -> const T* {
  return &ptr_->value;
}

template <typename T>
auto SequentialSkipListSet<T>::Iterator::operator++(/* prefix */)
    -> SequentialSkipListSet::Iterator& {
  ptr_ = ptr_->Next();
  return *this;
}

template <typename T>
auto SequentialSkipListSet<T>::Iterator::operator++(int /* postfix */)
    -> SequentialSkipListSet::Iterator {
  auto copy = *this;
  ++(*this);
  return copy;
}

template <typename T>
auto SequentialSkipListSet<T>::Iterator::operator==(
    const SequentialSkipListSet::Iterator& other) const -> bool {
  return ptr_ == other.ptr_;
}

template <typename T>
auto SequentialSkipListSet<T>::Iterator::operator!=(
    const SequentialSkipListSet::Iterator& other) const -> bool {
  return !(*this == other);  // NOLINT (simplification will lead to recursion)
}

////////////////////////////////////////////////////////////////////////////////
////
//// SequentialSkipListSet: public interface
////

template <typename T>
SequentialSkipListSet<T>::~SequentialSkipListSet() {
  for (auto node = head_; node;) {
    auto next = node->forward[0];
    node->forward.clear();
    node = next;
  }
}

// Example: searching for 20 in SkipList illustrated below
// [kMaxLevel = 4, kProbability = 0.5]
//
// ┌––┐                                                    ┌––┐
// | 4├–––––––––––––––––––––––––––––––––––––––––––––––––––>│ 4|
// |  |          ┌––┐                        ┌––┐          |  |
// | 3├–––––––––>│ 3├–––––––––––––––––––––––>│ 3├–––––––––>│ 3|
// |  |   ┌––┐   |  |          ┌––┐          |  |          |  |
// | 2├––>│ 2├––>│ 2├–––––––––>│ 2├–––––––––>│ 2├–––––––––>│ 2|
// |  |   |  |   |  |   ┌––┐   |  |   ┌––┐   |  |   ┌––┐   |  |
// | 1├––>│ 1├––>│ 1├––>│ 1├––>│ 1├––>│ 1├––>│ 1├––>| 1├––>| 1|
// |  |   |  |   |  |   |  |   |  |   |  |   |  |   |  |   |  |
// |––|   |––|   |––|   |––|   |––|   |––|   |––|   |––|   |––|
// |hd|   | 6|   | 8|   |11|   |16|   |19|   |21|   |24|   |25|
// └––┘   └––┘   └––┘   └––┘   └––┘   └––┘   └––┘   └––┘   └––┘
//
// The main idea is using so-called `express lanes`.
// Start from the highest level of `head_` and traverse forward-down:
//   hd->forward[4]->value = 25 > 20 -> down a level
//   hd->forward[3]->value =  8 < 20 -> traverse forward
//    8->forward[3]->value = 21 > 20 -> down a level
//    8->forward[2]->value = 16 < 20 -> traverse forward
//   16->forward[2]->value = 21 > 20 -> down a level
//   16->forward[1]->value = 19 < 20 -> traverse forward
//   19->forward[1]->value = 21 > 20 -> last level, value not found
//
template <typename T>
auto SequentialSkipListSet<T>::Find(const T& value) const
    -> SequentialSkipListSet::Iterator {
  if (auto node = Traverse(value); node && !(value < node->value)) {
    return Iterator{node.get()};
  } else {
    return End();
  }
}

// Example: inserting value 21 with level 2 into SkipList illustrated below
// [kMaxLevel = 4, kProbability = 0.5]
//
// ┌––┐                                      ┌––┐
// | 4├–––––––––––––––––––––––––––––––––––––>│ 4|
// |  |                 ┌––┐                 |  |
// | 3├––––––––––––––––>│ 3├––––––––––––––––>│ 3|
// |  |   ┌––┐          |  |                 |  |
// | 2├––>│ 2├–––––––––>│ 2├––––––––––––––––>│ 2|
// |  |   |  |   ┌––┐   |  |   ┌––┐   ┌––┐   |  |
// | 1├––>│ 1├––>│ 1├––>│ 1├––>│ 1├––>| 1├––>| 1|
// |  |   |  |   |  |   |  |   |  |   |  |   |  |
// |––|   |––|   |––|   |––|   |––|   |––|   |––|
// |hd|   | 6|   |13|   |15|   |19|   |24|   |25|
// └––┘   └––┘   └––┘   └––┘   └––┘   └––┘   └––┘
//
// First, we need to find a position at which new value is going to be inserted,
// that is the node with value 19.
//
// When a new value is inserted, at most one forward pointer on every level
// will be updated. Let's denote an array of nodes which would receive
// new forward pointer as `update[]`, where `update[level]` holds a node
// on level `level`.
//
// Every time we go down a level while searching, we save the current node
// to the array `update`. When a new node is being inserted, we need to redirect
// its forward pointer to `update[level]->forward[level]` and forward pointer of
// `update[level]->forward[level]` to a new node.
//
// |..|       |..|         |..|   ┌––┐   |..|         |..|   ┌––┐   |..|
// │ 2├––..––>│ 2|         │ 2|   │ 2├––>│ 2|         │ 2├––>│ 2├––>│ 2|
// |  |       |  |         |  |   |  |   |  |         |  |   |  |   |  |
// │ 1|       | 1|  ~~~~>  │ 1|   | 1|   | 1|  ~~~~>  │ 1|   | 1|   | 1|
// |  |       |  |         |  |   |  |   |  |         |  |   |  |   |  |
// |––|       |––|         |––|   |––|   |––|         |––|   |––|   |––|
// |15|       |25|         |15|   |21|   |25|         |15|   |21|   |25|
// └––┘       └––┘         └––┘   └––┘   └––┘         └––┘   └––┘   └––┘
//
// ┌––┐                                             ┌––┐
// | 4├––––––––––––––––––––––––––––––––––––––––––––>│ 4|
// |  |                 ┌––┐                        |  |
// | 3├––––––––––––––––>│ 3├–––––––––––––––––––––––>│ 3|
// |  |   ┌––┐          |  |          ┌––┐          |  |
// | 2├––>│ 2├–––––––––>│ 2├–––––––––>│ 2├–––––––––>│ 2|
// |  |   |  |   ┌––┐   |  |   ┌––┐   |  |   ┌––┐   |  |
// | 1├––>│ 1├––>│ 1├––>│ 1├––>│ 1├––>│ 1├––>| 1├––>| 1|
// |  |   |  |   |  |   |  |   |  |   |  |   |  |   |  |
// |––|   |––|   |––|   |––|   |––|   |––|   |––|   |––|
// |hd|   | 6|   |13|   |15|   |19|   |21|   |24|   |25|
// └––┘   └––┘   └––┘   └––┘   └––┘   └––┘   └––┘   └––┘
//
template <typename T>
auto SequentialSkipListSet<T>::Insert(const T& value)
    -> std::pair<Iterator, bool> {
  auto update = NodePtrList{kMaxLevel + 1};
  auto node = Traverse(value, &update);

  // Test for equality without using operator==
  // (at this point, value is guaranteed to be lesser or equal to node->value)
  if (node && !(value < node->value)) {
    return {Iterator{node.get()}, false};
  }

  auto node_level = GenerateRandomLevel();
  if (node_level > level_) {
    std::fill(std::begin(update) + level_ + 1,
              std::begin(update) + node_level + 1, head_);
    level_ = node_level;
  }

  auto new_node = std::make_shared<Node>(value, node_level);
  for (auto level = Level{0}; level <= node_level; ++level) {
    auto i = static_cast<std::size_t>(level);
    new_node->forward[i] = std::exchange(update[i]->forward[i], new_node);
  }

  return {Iterator{new_node.get()}, true};
}

template <typename T>
auto SequentialSkipListSet<T>::Erase(const T& value) -> std::size_t {
  auto update = NodePtrList{kMaxLevel + 1};
  auto node = Traverse(value, &update);

  // Test for inequality without using operator==
  // (at this point, value is guaranteed to be lesser or equal to node->value)
  if (!node || value < node->value) {
    return 0;
  }

  for (auto level = Level{0}; level <= level_; ++level) {
    auto i = static_cast<std::size_t>(level);
    if (update[i]->forward[i] != node) {
      break;
    }
    update[i]->forward[i] = node->forward[i];
  }

  while (level_ > 0 && !head_->forward[static_cast<std::size_t>(level_)]) {
    --level_;
  }

  return 1;
}

template <typename T>
auto SequentialSkipListSet<T>::Begin() const
    -> SequentialSkipListSet::Iterator {
  return Iterator{head_->Next()};
}

template <typename T>
auto SequentialSkipListSet<T>::End() const -> SequentialSkipListSet::Iterator {
  return Iterator{nullptr};
}

////////////////////////////////////////////////////////////////////////////////
////
//// SequentialSkipListSet: private interface
////

template <typename T>
auto SequentialSkipListSet<T>::Traverse(
    const T& value, SequentialSkipListSet::NodePtrList* update) const
    -> SequentialSkipListSet::NodePtr {
  auto node = head_;

  for (auto level = level_; level >= 0; --level) {
    auto i = static_cast<std::size_t>(level);
    while (node->forward[i] && node->forward[i]->value < value) {
      node = node->forward[i];
    }
    if (update) {
      (*update)[i] = node;
    }
  }

  return node->forward[0];
}

template <typename T>
auto SequentialSkipListSet<T>::GenerateRandomLevel() const
    -> SequentialSkipListSet::Level {
  auto level = Level{0};
  while (level < kMaxLevel &&
         static_cast<Probability>(std::rand()) / RAND_MAX < kProbability) {
    ++level;
  }
  return level;
}

}  // namespace skipper

#endif  // SKIPPER_SEQUENTIAL_SET_IPP
