#ifndef SKIPPER_SEQUENTIAL_HPP
#define SKIPPER_SEQUENTIAL_HPP

#include <iostream>
#include <memory>
#include <vector>

namespace skipper {

template <typename T>
class SequentialSkipList {
 private:
  struct Node;  // Forward declaration for Iterator

 public:
  using Level = int;
  using Probability = double;

  using NodePtr = std::shared_ptr<Node>;
  using ForwardNodePtrs = std::vector<NodePtr>;

  static constexpr Level kMaxLevel = 4;
  static constexpr Probability kProbability = 0.2;

  static constexpr bool kSupportsMove = false;

 public:
  class Iterator {
   public:
    explicit Iterator(Node* ptr);

    auto operator*() const -> const T&;
    auto operator++(/* prefix */) -> Iterator&;
    auto operator++(int /* postfix */) -> Iterator;
    auto operator==(const Iterator& other) const -> bool;
    auto operator!=(const Iterator& other) const -> bool;

   private:
    Node* ptr_;
  };

 public:
  SequentialSkipList() = default;

  // TODO(Lev): investigate possible dangers of default ones
  SequentialSkipList(SequentialSkipList&& other) = delete;
  SequentialSkipList(const SequentialSkipList& other) = delete;
  SequentialSkipList& operator=(SequentialSkipList&& other) = delete;
  SequentialSkipList& operator=(const SequentialSkipList& other) = delete;

  ~SequentialSkipList() = default;

  // STL set-like interface
  auto Find(const T& value) const -> Iterator;
  auto Insert(const T& value) -> std::pair<Iterator, bool>;
  auto Erase(const T& value) -> std::size_t;

  // Iteration interface
  auto Begin() const -> Iterator;
  auto End() const -> Iterator;

 private:
  auto GenerateRandomLevel() const -> Level;

 private:
  Level level_{0};
  NodePtr head_{std::make_shared<Node>(T{}, kMaxLevel)};
};

}  // namespace skipper

#endif  // SKIPPER_SEQUENTIAL_HPP

#include "skipper/sequential.ipp"
