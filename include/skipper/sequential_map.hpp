#ifndef SKIPPER_SEQUENTIAL_MAP_HPP
#define SKIPPER_SEQUENTIAL_MAP_HPP

#include <memory>
#include <vector>

namespace skipper {

template <typename Key, typename Value>
class SequentialSkipListMap {
 private:
  struct Node;

 public:
  using Level = int;
  using Probability = double;

  using NodePtr = std::shared_ptr<Node>;
  using ForwardNodePtrs = std::vector<NodePtr>;

  static constexpr auto kMaxLevel = Level{4};
  static constexpr auto kProbability = Probability{0.2};

 public:
  class Iterator {
   public:
    explicit Iterator(Node* ptr);

    auto operator*() const -> const std::pair<Key, Value>&;
    auto operator->() const -> const std::pair<Key, Value>*;
    auto operator++(/* prefix */) -> Iterator&;
    auto operator++(int /* postfix */) -> Iterator;
    auto operator==(const Iterator& other) const -> bool;
    auto operator!=(const Iterator& other) const -> bool;

   private:
    Node* ptr_;
  };

 public:
  SequentialSkipListMap() = default;

  // STL map-like interface
  auto Find(const Key& key) const -> Iterator;

  auto Insert(const Key& key, const Value& value) -> std::pair<Iterator, bool>;
  auto operator[](const Key& key) -> Value&;
  auto operator[](const Key& key) const -> const Value&;

  auto Erase(const Key& key) -> std::size_t;

  // Iteration interface
  auto Begin() const -> Iterator;
  auto End() const -> Iterator;

 private:
  auto Traverse(const Key& key, ForwardNodePtrs* update = nullptr) const
      -> NodePtr;

  auto GenerateRandomLevel() const -> Level;

 private:
  Level level_{0};
  NodePtr head_{std::make_shared<Node>(Key{}, Value{}, kMaxLevel)};
};

}  // namespace skipper

#endif  // SKIPPER_SEQUENTIAL_MAP_HPP

#include "skipper/sequential_map.ipp"
