#include <catch2/catch.hpp>

#include <algorithm>
#include <thread>
#include <unordered_set>

#include "skipper/guarded_set.hpp"

using Catch::Generators::chunk;
using Catch::Generators::random;

template <typename T>
using GSL = skipper::GuardedSkipListSet<T>;

static constexpr auto kThousand = 1'000;

TEST_CASE("Check Sequential SkipList Map functionality", "[Functionality]") {
  auto numbers = chunk(kThousand, random(0, 100)).get();

  SECTION("Insert() returns true for new elements and false otherwise",
          "[Correctness]") {
    auto skip_list = GSL<int>{};

    auto unique_numbers = std::unordered_set<int>{};

    for (auto n : numbers) {
      if (unique_numbers.find(n) != std::end(unique_numbers)) {
        auto [it, success] = skip_list->Insert(n);
        REQUIRE(!success);
      } else {
        auto [it, success] = skip_list->Insert(n);
        REQUIRE(success);
        unique_numbers.insert(n);
      }
    }
  }

  auto unique_numbers = std::unordered_map<int, int>{};
  for (auto n : numbers) {
    unique_numbers[n] = n;
  }

  SECTION("Contains() returns true for existing elements and false otherwise",
          "[Correctness]") {
    auto skip_list = GSL<int>{};

    auto other_numbers =
        chunk(kThousand, random(2 * kThousand, 3 * kThousand)).get();

    for (auto n : numbers) {
      skip_list->Insert(n);
    }

    auto end = skip_list->End();
    for (auto n : numbers) {
      auto it = skip_list->Find(n);
      REQUIRE(it != end);
    }

    for (auto n : other_numbers) {
      auto it = skip_list->Find(n);
      REQUIRE(it == end);
    }
  }

  SECTION("Erase() removes requested elements and does not remove other",
          "[Correctness]") {
    auto skip_list = GSL<int>{};

    for (auto n : numbers) {
      skip_list->Insert(n);
    }

    auto half = unique_numbers.size() / 2;
    auto it = std::begin(unique_numbers);
    for (auto size = std::size_t{0}; size < half; ++size) {
      REQUIRE(skip_list->Erase(it->first));
      ++it;
    }

    auto end = skip_list->End();
    for (; it != std::end(unique_numbers); ++it) {
      REQUIRE(skip_list->Find(it->first) != end);
    }
  }
}

TEST_CASE(
    "One thread inserts, another is trying to erase non-existent elements",
    "[Concurrency]") {
  auto skip_list = GSL<int>{};

  auto to_insert = chunk(100 * kThousand, random(0, kThousand)).get();
  auto to_erase =
      chunk(100 * kThousand, random(2 * kThousand, 3 * kThousand)).get();

  auto inserter = std::thread([&]() {
    for (auto n : to_insert) {
      skip_list->Insert(n);
    }
  });
  auto eraser = std::thread([&]() {
    for (auto n : to_erase) {
      REQUIRE(!skip_list->Erase(n));
    }
  });

  inserter.join();
  eraser.join();

  auto end = skip_list->End();
  for (auto n : to_insert) {
    REQUIRE(skip_list->Find(n) != end);
  }
  for (auto n : to_erase) {
    REQUIRE(skip_list->Find(n) == end);
  }
}
