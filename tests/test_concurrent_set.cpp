#include <catch2/catch.hpp>

#include <thread>
#include <algorithm>

#include "skipper/concurrent_set.hpp"
#include "skipper/sequential_set.hpp"

#include "../utils/random.hpp"

template <typename T>
using SL = skipper::ConcurrentSkipListSet<T>;

TEST_CASE("Sanity check", "[Correctness]") {
  auto skip_list = SL<int>{};

  auto numbers = GenerateNumbers(10);
  for (auto n : numbers) {
    skip_list.Insert(n);
  }

  for (auto n : numbers) {
    REQUIRE(skip_list.Contains(n));
  }

  auto other_numbers = GenerateNumbers(100, -999, -900);
  for (auto n : other_numbers) {
    REQUIRE(!skip_list.Contains(n));
  }

  for (auto n : numbers) {
    skip_list.Erase(n);
  }

  for (auto n : numbers) {
    REQUIRE(!skip_list.Erase(n));
    REQUIRE(!skip_list.Contains(n));
  }
}

TEST_CASE("One inserts, another is trying to erase non-existent elements",
          "[Concurrency]") {
  auto skip_list = SL<int>{};

  auto to_insert = GenerateNumbers(100 * 1'000, 0, 1'000);
  auto to_erase = GenerateNumbers(100 * 1'000, 1'000 + 1, 2'000);

  auto inserter = std::thread([&]() {
    for (auto n : to_insert) {
      skip_list.Insert(n);
    }
  });
  auto eraser = std::thread([&]() {
    for (auto n : to_erase) {
      skip_list.Erase(n);
    }
  });

  inserter.join();
  eraser.join();

  for (auto n : to_insert) {
    REQUIRE(skip_list.Contains(n));
  }
  for (auto n : to_erase) {
    REQUIRE(!skip_list.Contains(n));
  }
}
