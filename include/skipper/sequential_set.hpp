#ifndef SKIPPER_SEQUENTIAL_SET_HPP
#define SKIPPER_SEQUENTIAL_SET_HPP

#include <iostream>
#include <memory>
#include <vector>

namespace skipper {

template <typename T>
class SequentialSkipListSet {
 private:
  struct Node;  // Forward declaration for Iterator

 public:
  using Level = int;
  using Probability = double;

  using NodePtr = std::shared_ptr<Node>;
  using NodePtrList = std::vector<NodePtr>;

  static constexpr auto kMaxLevel = Level{4};
  static constexpr auto kProbability = Probability{0.2};

  static constexpr auto kSupportsMove = false;

 public:
  class Iterator {
   public:
    explicit Iterator(Node* ptr);

    auto operator*() const -> const T&;
    auto operator->() const -> const T*;
    auto operator++(/* prefix */) -> Iterator&;
    auto operator++(int /* postfix */) -> Iterator;
    auto operator==(const Iterator& other) const -> bool;
    auto operator!=(const Iterator& other) const -> bool;

   private:
    Node* ptr_;
  };

 public:
  SequentialSkipListSet() = default;

  // TODO(Lev): investigate possible dangers of default ones
  SequentialSkipListSet(SequentialSkipListSet&& other) = delete;
  SequentialSkipListSet(const SequentialSkipListSet& other) = delete;
  SequentialSkipListSet& operator=(SequentialSkipListSet&& other) = delete;
  SequentialSkipListSet& operator=(const SequentialSkipListSet& other) = delete;

  ~SequentialSkipListSet();

  // STL set-like interface
  auto Find(const T& value) const -> Iterator;
  auto Insert(const T& value) -> std::pair<Iterator, bool>;
  auto Erase(const T& value) -> std::size_t;

  // Iteration interface
  auto Begin() const -> Iterator;
  auto End() const -> Iterator;

 private:
  auto Traverse(const T& value, NodePtrList* update = nullptr) const -> NodePtr;

  auto GenerateRandomLevel() const -> Level;

 private:
  Level level_{0};
  NodePtr head_{std::make_shared<Node>(T{}, kMaxLevel)};
};

}  // namespace skipper

#endif  // SKIPPER_SEQUENTIAL_SET_HPP

#include "skipper/sequential_set.ipp"
