#ifndef SKIPPER_DETAIL_GUARDED_HPP
#define SKIPPER_DETAIL_GUARDED_HPP

#include <mutex>

namespace skipper::detail {

template <typename T>
class Guarded {
 public:
  class Proxy {
   public:
    Proxy(T& object, std::mutex& mutex);

    auto operator->() -> T*;

   private:
    T& object_;
    std::unique_lock<std::mutex> lock_;
  };

 public:
  Guarded() = default;

  auto operator->() -> Proxy;

 private:
  T object_;
  std::mutex mutex_;
};

}  // namespace skipper::detail

#endif  // SKIPPER_DETAIL_GUARDED_HPP

#include "skipper/detail/guarded.ipp"
