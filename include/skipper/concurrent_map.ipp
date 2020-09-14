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
  return false;
}

template <typename Key, typename Value>
auto ConcurrentSkipListMap<Key, Value>::Insert(const Key& key,
                                               const Value& value) -> bool {
  return false;
}

template <typename Key, typename Value>
auto ConcurrentSkipListMap<Key, Value>::Erase(const Key& key) -> bool {
  return false;
}

}  // namespace skipper

#endif  // SKIPPER_CONCURRENT_MAP_IPP