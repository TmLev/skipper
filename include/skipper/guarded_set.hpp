#ifndef SKIPPER_GUARDED_SET_HPP
#define SKIPPER_GUARDED_SET_HPP

#include "skipper/sequential_set.hpp"
#include "skipper/detail/guarded.hpp"

namespace skipper {

template <typename T>
using GuardedSkipListSet = detail::Guarded<SequentialSkipListSet<T>>;

}

#endif  // SKIPPER_GUARDED_SET_HPP
