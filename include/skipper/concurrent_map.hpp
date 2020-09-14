#ifndef SKIPPER_CONCURRENT_MAP_HPP
#define SKIPPER_CONCURRENT_MAP_HPP

#include <memory>
#include <mutex>
#include <optional>
#include <vector>

namespace skipper {

template <typename Key, typename Value>
class ConcurrentSkipListMap {
 public:
  using Level = int;
  using Probability = double;

  static constexpr auto kMaxLevel = Level{4};
  static constexpr auto kProbability = Probability{0.2};

 public:
  ConcurrentSkipListMap();

  ConcurrentSkipListMap(ConcurrentSkipListMap&& other) = delete;
  ConcurrentSkipListMap(const ConcurrentSkipListMap& other) = delete;
  ConcurrentSkipListMap& operator=(ConcurrentSkipListMap&& other) = delete;
  ConcurrentSkipListMap& operator=(const ConcurrentSkipListMap& other) = delete;

  ~ConcurrentSkipListMap() = default;

  auto Contains(const Key& key) -> bool;
  auto Insert(const Key& key, const Value& value) -> bool;
  auto Erase(const Key& key) -> bool;

 private:
  struct Node;  // Forward declaration for `using` declarations

 private:
  using NodePtr = std::shared_ptr<Node>;
  using NodePtrList = std::vector<NodePtr>;

  using Flag = std::atomic<bool>;
  using Lock = std::recursive_mutex;

 private:
  NodePtr head_{std::make_shared<Node>(Key{}, Value{}, kMaxLevel)};
  NodePtr tail_{std::make_shared<Node>(Key{}, Value{}, kMaxLevel)};
};

}  // namespace skipper

#endif  // SKIPPER_CONCURRENT_MAP_HPP

#include "skipper/concurrent_map.ipp"