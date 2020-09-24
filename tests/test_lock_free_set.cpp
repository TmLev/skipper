#include <catch2/catch.hpp>

#include <thread>
#include <unordered_set>

#include "skipper/lock_free_set.hpp"

using Catch::Generators::chunk;
using Catch::Generators::random;

static constexpr auto kThousand = 1'000;

template <typename T>
using SL = skipper::LockFreeSkipListSet<T>;

TEST_CASE("Insert() returns true for new elements and false otherwise",
          "[Correctness]") {
  auto skip_list = SL<int>{};
  REQUIRE(skip_list.Insert(1));
  REQUIRE(!skip_list.Insert(1));
}

TEST_CASE("Contains() returns true for existing elements and false otherwise",
          "[Correctness]") {
  auto skip_list = SL<int>{};
  REQUIRE(skip_list.Insert(1));
  REQUIRE(skip_list.Contains(1));
  REQUIRE(skip_list.Contains(1));
  REQUIRE(!skip_list.Contains(2));
  REQUIRE(!skip_list.Contains(0));
}

TEST_CASE("Stress test for single thread", "[Correctness]") {
  auto skip_list = SL<int>{};

  auto numbers = chunk(10 * kThousand, random(0, 100)).get();
  auto unique_numbers = std::unordered_set<int>{};

  for (auto n : numbers) {
    if (unique_numbers.find(n) == std::end(unique_numbers)) {
      REQUIRE(skip_list.Insert(n));
      unique_numbers.insert(n);
    } else {
      REQUIRE(!skip_list.Insert(n));
    }
  }

  for (auto n : unique_numbers) {
    REQUIRE(skip_list.Contains(n));
  }
}

TEST_CASE("Two threads insert repeating numbers simultaneously",
          "[Concurrency]") {
  auto skip_list = SL<int>{};

  auto first_numbers = chunk(100 * kThousand, random(0, kThousand)).get();
  auto second_numbers = chunk(100 * kThousand, random(0, kThousand)).get();

  auto first = std::thread([&]() {
    for (auto n : first_numbers) {
      skip_list.Insert(n);
    }
  });
  auto second = std::thread([&]() {
    for (auto n : second_numbers) {
      skip_list.Insert(n);
    }
  });

  first.join();
  second.join();

  auto merged = std::vector<int>{};
  std::merge(std::begin(first_numbers), std::end(first_numbers),
             std::begin(second_numbers), std::end(second_numbers),
             std::back_inserter(merged));
  for (auto n : merged) {
    REQUIRE(skip_list.Contains(n));
  }
}
