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

TEST_CASE("Two threads insert the same numbers simultaneously",
          "[Concurrency]") {
  auto skip_list = SL<int>{};

  auto first_numbers = chunk(10 * kThousand, random(0, kThousand)).get();
  auto second_numbers = first_numbers;

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

  for (auto n : first_numbers) {
    REQUIRE(skip_list.Contains(n));
  }
}

TEST_CASE("One thread inserts and second check for contain the same element",
          "[Concurrency]") {
  auto skip_list = SL<int>{};

  int num = 10;

  auto first_numbers = chunk(kThousand, random(num, num)).get();
  auto second_numbers = chunk(kThousand, random(num, num)).get();

  auto first = std::thread([&]() {
    for (auto n : first_numbers) {
      skip_list.Insert(n);
    }
  });

  auto second = std::thread([&]() {
    for (auto n : second_numbers) {
      skip_list.Contains(n);
    }
  });

  first.join();
  second.join();

  REQUIRE(skip_list.Contains(num));
}

TEST_CASE(
    "Tho threads insert different numbers, then four threads check for contain",
    "[Concurrency]") {
  auto skip_list = SL<int>{};

  constexpr auto kWriteThreadCount = std::size_t{2};
  constexpr auto kReadThreadCount = std::size_t{4};

  auto write_numbers = std::vector<std::vector<int>>{
      chunk(kThousand, random(0, kThousand - 1)).get(),
      chunk(kThousand, random(kThousand, 2 * kThousand)).get(),
  };

  auto first_mid = std::begin(write_numbers[0]) + write_numbers[0].size() / 2;
  auto second_mid = std::begin(write_numbers[1]) + write_numbers[1].size() / 2;

  auto read_numbers =
      std::vector<std::vector<int>>{{std::begin(write_numbers[0]), first_mid},
                                    {first_mid, std::end(write_numbers[0])},
                                    {std::begin(write_numbers[1]), second_mid},
                                    {second_mid, std::end(write_numbers[1])}};

  auto write_threads = std::vector<std::thread>{};
  write_threads.reserve(kWriteThreadCount);
  for (auto i = std::size_t{0}; i < kWriteThreadCount; ++i) {
    write_threads.emplace_back([&skip_list, &write_numbers, i]() {
      for (auto n : write_numbers[i]) {
        skip_list.Insert(n);
      }
    });
  }

  for (auto&& t : write_threads) {
    t.join();
  }

  auto read_threads = std::vector<std::thread>{};
  for (auto i = std::size_t{0}; i < kReadThreadCount; ++i) {
    read_threads.emplace_back([&skip_list, &read_numbers, i]() {
      for (auto n : read_numbers[i]) {
        REQUIRE(skip_list.Contains(n));
      }
    });
  }

  for (auto&& t : read_threads) {
    t.join();
  }
}
