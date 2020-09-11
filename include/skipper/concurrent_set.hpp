#ifndef SKIPPER_CONCURRENT_SET_HPP
#define SKIPPER_CONCURRENT_SET_HPP

#include <atomic>
#include <memory>
#include <mutex>
#include <optional>
#include <vector>

namespace skipper {

template <typename T>
class ConcurrentSkipListSet {
 public:
  using Level = int;
  using Probability = double;

  static constexpr auto kMaxLevel = Level{4};
  static constexpr auto kProbability = Probability{0.2};

 public:
  ConcurrentSkipListSet();

  ConcurrentSkipListSet(ConcurrentSkipListSet&& other) = delete;
  ConcurrentSkipListSet(const ConcurrentSkipListSet& other) = delete;
  ConcurrentSkipListSet& operator=(ConcurrentSkipListSet&& other) = delete;
  ConcurrentSkipListSet& operator=(const ConcurrentSkipListSet& other) = delete;

  ~ConcurrentSkipListSet() = default;

  auto Contains(const T& value) -> bool;
  auto Insert(const T& value) -> bool;
  auto Erase(const T& value) -> bool;

 private:
  struct Node;  // Forward declaration for `using` declarations

 private:
  using MaybeLevel = std::optional<Level>;

  using NodePtr = std::shared_ptr<Node>;
  using NodePtrList = std::vector<NodePtr>;

  using Flag = std::atomic<bool>;
  using Lock = std::recursive_mutex;
  using Guard = std::unique_lock<Lock>;
  using MaybeGuard = std::optional<Guard>;
  using GuardList = std::vector<Guard>;

 private:
  struct FindResult;

 private:
  auto Find(const T& value) -> FindResult;

  auto GenerateRandomLevel() -> Level;

 private:
  NodePtr head_{std::make_shared<Node>(T{}, kMaxLevel)};
  NodePtr tail_{std::make_shared<Node>(T{}, kMaxLevel)};
};

}  // namespace skipper

#endif  // SKIPPER_CONCURRENT_SET_HPP

#include "skipper/concurrent_set.ipp"
