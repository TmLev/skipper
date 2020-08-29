#include <catch2/catch.hpp>

#include <set>
#include <sstream>
#include <utility>
#include <vector>

#include "skipper/sequential_set.hpp"

using Catch::Generators::chunk;
using Catch::Generators::random;

template <typename T>
using SL = skipper::SequentialSkipListSet<T>;

TEST_CASE("Find() returns End() iterator when no element was found", "[Find]") {
  auto skip_list = SL<int>{};

  {
    auto it = skip_list.Find(2);
    REQUIRE(it == skip_list.End());
  }
  {
    skip_list.Insert(3);
    auto it = skip_list.Find(2);
    REQUIRE(it == skip_list.End());
  }
}

TEST_CASE("Find() returns equal iterators for same number", "[Find]") {
  auto skip_list = SL<int>{};

  skip_list.Insert(2);

  auto it = skip_list.Find(2);
  auto same_it = skip_list.Find(2);
  REQUIRE(it == skip_list.Begin());
  REQUIRE(it == same_it);
  REQUIRE(*it == 2);

  auto other = skip_list.Find(3);
  REQUIRE(it != other);
}

TEST_CASE("Insert() returns same iterator for same element", "[Insert]") {
  auto skip_list = SL<int>{};

  auto [it, success] = skip_list.Insert(1);
  REQUIRE(success);
  REQUIRE(it == skip_list.Begin());
  REQUIRE(it != skip_list.End());

  auto [same_it, same_success] = skip_list.Insert(1);
  REQUIRE(!same_success);
  REQUIRE(it == same_it);

  auto [other_it, other_success] = skip_list.Insert(0);
  REQUIRE(other_success);
  REQUIRE(it != other_it);
}

TEST_CASE("Insert() maintains sortedness", "[Insert]") {
  auto skip_list = SL<int>{};

  auto numbers = chunk(100'000, random(-10'000, 10'000)).get();
  for (auto n : numbers) {
    skip_list.Insert(n);
  }

  auto sorted_numbers = std::set<int>{std::begin(numbers), std::end(numbers)};
  auto it = skip_list.Begin();
  for (auto n : sorted_numbers) {
    REQUIRE(*it == n);
    ++it;
  }
}

TEST_CASE("Erase() does nothing if SL is empty", "[Erase]") {
  auto skip_list = SL<int>{};
  REQUIRE(skip_list.Erase(0) == 0);
}

TEST_CASE("Erase() erases same element only once", "[Erase]") {
  auto skip_list = SL<int>{};
  skip_list.Insert(1);
  REQUIRE(skip_list.Erase(1) == 1);
  REQUIRE(skip_list.Erase(1) == 0);
  REQUIRE(skip_list.Erase(1) == 0);
}

TEST_CASE("Erase() empties SL after removing all elements", "[Erase]") {
  auto skip_list = SL<int>{};

  auto numbers = chunk(100'000, random(-10'000, 10'000)).get();
  numbers.push_back(numbers.front() - 1);  // In case all numbers are the same
  for (auto n : numbers) {
    skip_list.Insert(n);
  }
  REQUIRE(skip_list.Begin() != skip_list.End());

  for (auto n : numbers) {
    skip_list.Erase(n);
  }
  REQUIRE(skip_list.Begin() == skip_list.End());
}

TEST_CASE("Iterator: empty SL has same Begin() and End()", "[Iterator]") {
  auto skip_list = SL<int>{};
  REQUIRE(skip_list.Begin() == skip_list.End());
}

TEST_CASE("Iterator: operator->() returns pointer to value", "[Iterator]") {
  auto skip_list = SL<std::string>{};
  auto value = std::string{"123"};
  skip_list.Insert(value);
  auto it = skip_list.Find(value);
  REQUIRE(it->size() == value.size());
}

TEST_CASE("Iterator: properly increments", "[Iterator]") {
  auto skip_list = SL<int>{};

  skip_list.Insert(1);
  skip_list.Insert(2);
  skip_list.Insert(3);

  auto one = skip_list.Find(1);
  auto two = skip_list.Find(2);
  auto three = skip_list.Find(3);

  REQUIRE(one != two);
  REQUIRE(two != three);
  REQUIRE(three != one);

  auto one_copy = one++;
  REQUIRE(one == two);
  REQUIRE(one != one_copy);

  auto two_copy = ++two;
  REQUIRE(two == two_copy);
  REQUIRE(two == three);

  auto three_copy = three++;
  REQUIRE(two == three_copy);
  REQUIRE(three == skip_list.End());
}

struct MovableOnly {
 public:
  MovableOnly() = default;

  explicit MovableOnly(std::ostream* o) : output(o) {
    if (output) {
      *output << "ctor";
    }
  }

  MovableOnly(const MovableOnly&) = delete;
  MovableOnly& operator=(const MovableOnly&) = delete;

  MovableOnly(MovableOnly&& other) {
    std::swap(value, other.value);
    std::swap(output, other.output);
  }

  MovableOnly& operator=(MovableOnly&& other) {
    std::swap(value, other.value);
    output = std::exchange(other.output, nullptr);
    return *this;
  }

  ~MovableOnly() {
    if (output) {
      *output << " dtor";
    }
  }

 public:
  int value{0};
  std::ostream* output{nullptr};
};

auto operator<(const MovableOnly& left, const MovableOnly& right) -> bool {
  return left.value < right.value;
}

TEST_CASE("Insert() supports movable-only objects", "[!hide]") {
  using SLuptr = SL<std::unique_ptr<int>>;

  if constexpr (SLuptr::kSupportsMove) {
    auto skip_list = SLuptr{};
    auto ptr = std::make_unique<int>(0);
    skip_list.Insert(std::move(ptr));
  }

  using SLmo = SL<MovableOnly>;
  if constexpr (SLmo::kSupportsMove) {
    auto ss = std::stringstream{};
    {
      auto skip_list = SLmo{};
      auto only_movable = MovableOnly{&ss};
      skip_list.Insert(std::move(only_movable));
    }
    REQUIRE(ss.str() == "ctor dtor");
  }
}
