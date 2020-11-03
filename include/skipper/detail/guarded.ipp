#ifndef SKIPPER_DETAIL_GUARDED_IPP
#define SKIPPER_DETAIL_GUARDED_IPP

#include "skipper/detail/guarded.hpp"

namespace skipper::detail {

////////////////////////////////////////////////////////////////////////////////

template <typename T>
Guarded<T>::Proxy::Proxy(T& object, std::mutex& mutex)
    : object_(object), lock_(mutex) {
}

template <typename T>
auto Guarded<T>::Proxy::operator->() -> T* {
  return &object_;
}

////////////////////////////////////////////////////////////////////////////////

template <typename T>
auto Guarded<T>::operator->() -> Proxy {
  return {object_, mutex_};
}

}  // namespace skipper::detail

#endif  // SKIPPER_DETAIL_GUARDED_IPP
