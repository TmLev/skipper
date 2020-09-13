# Examples

__Skipper__ provides different variations of `Set` and `Map` implementations with STL-like interface 
which use __Skip List__ under the hood.

## Sequential

Sequential classes like [`SequentialSkipListSet`](../include/skipper/sequential_set.hpp) 
and [`SequentialSkipListMap`](../include/skipper/sequential_map.hpp) are meant to be used in a single threaded
environment, i.e. they are not thread-safe. 

### Interface

Both of them provide a subset of STL-like interface:
```cpp
template <typename T>
class SequentialSkipListSet {
 public:
  // O(log N) complexity
  auto Find(const T& value) const -> Iterator;
  auto Insert(const T& value) -> std::pair<Iterator, bool>;
  auto Erase(const T& value) -> std::size_t;

  // O(1) complexity
  auto Begin() const -> Iterator;
  auto End() const -> Iterator;
};

template <typename Key, typename Value>
class SequentialSkipListMap {
 public:
  // O(log N) complexity
  auto Find(const Key& key) const -> Iterator;
  auto Insert(const Key& key, const Value& value) -> std::pair<Iterator, bool>;
  auto operator[](const Key& key) -> Value&;
  auto Erase(const Key& key) -> std::size_t;

  // O(1) complexity
  auto Begin() const -> Iterator;
  auto End() const -> Iterator;
};
```

### Iterator

Note that `Iterator` is of Forward category (see [here](https://en.cppreference.com/w/cpp/iterator/forward_iterator)):
```cpp
template <typename T>
class SequentialSkipListSet<T>::Iterator {
 public:
  // O(1) complexity
  auto operator*() const -> const T&;
  auto operator->() const -> const T*;
  auto operator++(/* prefix */) -> Iterator&;
  auto operator++(int /* postfix */) -> Iterator;
  auto operator==(const Iterator& other) const -> bool;
  auto operator!=(const Iterator& other) const -> bool;
};

template <typename Key, typename Value>
class SequentialSkipListMap<Key, Value>::Iterator {
 public:
  // O(1) complexity
  auto operator*() -> Element&;
  auto operator*() const -> const Element&;
  auto operator->() -> Element*;
  auto operator->() const -> const Element*;
  auto operator++(/* prefix */) -> Iterator&;
  auto operator++(int /* postfix */) -> Iterator;
  auto operator==(const Iterator& other) const -> bool;
  auto operator!=(const Iterator& other) const -> bool;
};
```

### Example

Minimal working example:
```cpp
#include <iostream>

#include <skipper/sequential_set.hpp>

auto main() -> int {
  auto skip_list = skipper::SequentialSkipListSet<int>{};

  for (auto number : {10, 5, 3, 1, 2}) {
    skip_list.Insert(number);
  }

  for (auto it = skip_list.Find(3); it != skip_list.End(); ++it) {
    std::cout << *it << ' ';
  } // 3 5 10

  skip_list.Erase(2);
  skip_list.Erase(10);

  for (auto it = skip_list.Begin(); it != skip_list.End(); ++it) {    
    std::cout << *it << ' ';
  } // 1 3 5
}
```

## Concurrent

Concurrent classes like [`ConcurrentSkipListSet`](../include/skipper/concurrent_set.hpp) 
and [`ConcurrentSkipListMap`](../include/skipper/concurrent_map.hpp) 
are, on the contrary to Sequential ones, thread-safe,
meaning any thread can call any method without the need for manual synchronization.  

### Interface

Available interface is as follows:
```cpp
template <typename T>
class ConcurrentSkipListSet {
 public:
  auto Contains(const T& value) -> bool;
  auto Insert(const T& value) -> bool;
  auto Erase(const T& value) -> bool;
};

template <typename Key, typename Value>
class ConcurrentSkipListMap {
 public:
  auto Contains(const Key& key) -> bool;
  auto Insert(const Key& key, const Value& value) -> bool;
  auto Erase(const Key& key) -> bool;
};
```

Methods `Insert` and `Erase` return `true` if call was successful and `false` otherwise.

### Example

Minimal working example:
```cpp
#include <iostream>
#include <thread>

#include <skipper/concurrent_set.hpp>

auto main() -> int {
  auto skip_list = skipper::ConcurrentSkipListSet<int>{};

  auto first = std::thread([&skip_list]() {
    for (auto number : {2, 10, 4}) {
      skip_list.Insert(number);
    }
  });
  auto second = std::thread([&skip_list]() {
    for (auto number : {5, 4, 9}) {
      skip_list.Insert(number);
    }
  });

  first.join();
  second.join();
  
  skip_list.Erase(5);

  for (auto number : {2, 4, 5, 9, 10}) {
    std::cout << skip_list.Contains(number) << ' ';
  } // 1 1 0 1 1
}
```
