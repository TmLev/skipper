#ifndef SKIPPER_SEQUENTIAL_MAP_HPP
#define SKIPPER_SEQUENTIAL_MAP_HPP

#include <memory>
#include <vector>
#include <tuple>

namespace skipper {

template <typename Key, typename Value>
class SequentialSkipListMap {
 private:
  struct Node;

 public:
  using Level = int;
  using Probability = double;

  static constexpr auto kMaxLevel = Level{4};
  static constexpr auto kProbability = Probability{0.2};

 public:
  class Element {
   public:
    Key key;
    Value value;
  };

  class Iterator {
   public:
    explicit Iterator(Node* ptr);

    auto operator*() -> Element&;
    auto operator*() const -> const Element&;
    auto operator->() -> Element*;
    auto operator->() const -> const Element*;
    auto operator++(/* prefix */) -> Iterator&;
    auto operator++(int /* postfix */) -> Iterator;
    auto operator==(const Iterator& other) const -> bool;
    auto operator!=(const Iterator& other) const -> bool;

   private:
    Node* ptr_;
  };

 public:
  SequentialSkipListMap() = default;

  SequentialSkipListMap(SequentialSkipListMap&& other) = delete;
  SequentialSkipListMap(const SequentialSkipListMap& other) = delete;
  SequentialSkipListMap& operator=(SequentialSkipListMap&& other) = delete;
  SequentialSkipListMap& operator=(const SequentialSkipListMap& other) = delete;

  ~SequentialSkipListMap();

  // STL map-like interface
  auto Find(const Key& key) const -> Iterator;

  auto Insert(const Key& key, const Value& value) -> std::pair<Iterator, bool>;
  auto operator[](const Key& key) -> Value&;

  auto Erase(const Key& key) -> std::size_t;

  // Iteration interface
  auto Begin() const -> Iterator;
  auto End() const -> Iterator;

 private:
  using NodePtr = std::shared_ptr<Node>;
  using NodePtrList = std::vector<NodePtr>;

 private:
  auto Traverse(const Key& key, NodePtrList* update = nullptr) const
      -> NodePtr;

  auto GenerateRandomLevel() const -> Level;

 private:
  Level level_{0};
  NodePtr head_{std::make_shared<Node>(Key{}, Value{}, kMaxLevel)};
};

}  // namespace skipper

#endif  // SKIPPER_SEQUENTIAL_MAP_HPP

#include "skipper/sequential_map.ipp"
