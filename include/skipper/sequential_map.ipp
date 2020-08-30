#ifndef SKIPPER_SEQUENTIAL_MAP_IPP
#define SKIPPER_SEQUENTIAL_MAP_IPP

#include "skipper//sequential_map.hpp"

namespace skipper {

////////////////////////////////////////////////////////////////////////////////
////
//// SequentialSkipListMap::Node
////

template <typename Key, typename Value>
struct SequentialSkipListMap<Key, Value>::Node {
 public:
  Node(Key key, Value value, Level level);

  auto Next() const -> Node*;

 public:
  Key key;
  Value value;
  ForwardNodePtrs forward;
};

template <typename Key, typename Value>
SequentialSkipListMap<Key, Value>::Node::Node(Key key, Value value, Level level)
    : key(std::move(key)),
      value(std::move(value)),
      forward(static_cast < std::size_t(level) + 1) {
}

template <typename Key, typename Value>
auto SequentialSkipListMap<Key, Value>::Node::Next() const
    ->SequentialSkipListMap<Key, Value>::Node* {
  return forward[0].get();
}

}  // namespace skipper

#endif  // SKIPPER_SEQUENTIAL_MAP_IPP
