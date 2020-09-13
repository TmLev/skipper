#ifndef SKIPPER_GUARDED_MAP_HPP
#define SKIPPER_GUARDED_MAP_HPP

#include "skipper/sequential_map.hpp"
#include "skipper/detail/guarded.hpp"

namespace skipper {

template <typename Key, typename Value>
using GuardedSkipListMap = detail::Guarded<SequentialSkipListMap<Key, Value>>;

}

#endif  // SKIPPER_GUARDED_MAP_HPP
