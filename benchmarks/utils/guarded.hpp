#ifndef SKIPPER_BENCHMARK_UTILS_GUARDED_HPP
#define SKIPPER_BENCHMARK_UTILS_GUARDED_HPP

#include <mutex>

template <typename T>
class Guarded {
 public:
  class Proxy {
   public:
    Proxy(T& object, std::mutex& mutex) : object_(object), mutex_(mutex) {
      mutex_.lock();
    }

    auto operator->() -> T* {
      return &object_;
    }

    ~Proxy() {
      mutex_.unlock();
    }

   private:
    T& object_;
    std::mutex& mutex_;
  };

 public:
  Guarded() = default;

  auto operator->() -> Proxy {
    return Proxy{object_, mutex_};
  }

 private:
  T object_;
  std::mutex mutex_;
};

#endif  // SKIPPER_BENCHMARK_UTILS_GUARDED_HPP
