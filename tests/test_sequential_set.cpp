#include <catch2/catch.hpp>

#include <sstream>
#include <utility>
#include <vector>

#include "skipper/sequential_set.hpp"

template <typename T>
using SL = skipper::SequentialSkipListSet<T>;

TEST_CASE("Empty SL has same Begin() and End() iterators") {
  auto skip_list = SL<int>{};
  REQUIRE(skip_list.Begin() == skip_list.End());
}

TEST_CASE("Find() in empty SL returns End() iterator") {
  auto skip_list = SL<int>{};
  auto it = skip_list.Find(2);
  REQUIRE(it == skip_list.End());
}

TEST_CASE("Insert()-ing same element returns same iterator") {
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
  REQUIRE(other_it == skip_list.Begin());
  REQUIRE(it != other_it);
}

TEST_CASE("Insert() maintains sortedness") {
  auto skip_list = SL<int>{};

  skip_list.Insert(2);
  skip_list.Insert(1);
  skip_list.Insert(0);

  auto got = std::vector<int>{};
  for (auto it = skip_list.Begin(); it != skip_list.End(); ++it) {
    got.push_back(*it);
  }
  auto expected = std::vector<int>{0, 1, 2};
  REQUIRE(got == expected);
}

TEST_CASE("Erase() does nothing if SL is empty") {
  auto skip_list = SL<int>{};
  REQUIRE(skip_list.Erase(0) == 0);
}

TEST_CASE("Erase() erases same element only once") {
  auto skip_list = SL<int>{};
  skip_list.Insert(1);
  REQUIRE(skip_list.Erase(1) == 1);
  REQUIRE(skip_list.Erase(1) == 0);
  REQUIRE(skip_list.Erase(1) == 0);
}

TEST_CASE("Iterator::operator->() returns pointer to value") {
  auto skip_list = SL<std::string>{};
  auto value = std::string{"123"};
  skip_list.Insert(value);
  auto it = skip_list.Find(value);
  REQUIRE(it->size() == value.size());
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
