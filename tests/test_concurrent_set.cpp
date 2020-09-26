#include <catch2/catch.hpp>

#include <algorithm>
#include <thread>
#include <unordered_set>

#include "skipper/concurrent_set.hpp"
#include "skipper/sequential_set.hpp"

using Catch::Generators::chunk;
using Catch::Generators::random;

template <typename T>
using SL = skipper::ConcurrentSkipListSet<T>;

static constexpr auto kThousand = 1'000;

TEST_CASE("Insert() returns true for new elements and false otherwise",
          "[Correctness]") {
  auto skip_list = SL<int>{};

  auto numbers = chunk(kThousand, random(0, 100)).get();
  auto unique_numbers = std::unordered_set<int>{};

  for (auto n : numbers) {
    if (unique_numbers.find(n) != std::end(unique_numbers)) {
      REQUIRE(!skip_list.Insert(n));
    } else {
      REQUIRE(skip_list.Insert(n));
      unique_numbers.insert(n);
    }
  }
}

TEST_CASE("Contains() returns true for existing elements and false otherwise",
          "[Correctness]") {
  auto skip_list = SL<int>{};

  auto numbers = chunk(kThousand, random(0, kThousand)).get();
  auto other_numbers =
      chunk(kThousand, random(2 * kThousand, 3 * kThousand)).get();

  for (auto n : numbers) {
    skip_list.Insert(n);
  }

  for (auto n : numbers) {
    REQUIRE(skip_list.Contains(n));
  }

  for (auto n : other_numbers) {
    REQUIRE(!skip_list.Contains(n));
  }
}

TEST_CASE("Erase() removes requested elements and does not remove other",
          "[Correctness]") {
  auto skip_list = SL<int>{};

  auto numbers = chunk(kThousand, random(0, 100)).get();
  for (auto n : numbers) {
    skip_list.Insert(n);
  }

  auto unique_numbers =
      std::unordered_set<int>{std::begin(numbers), std::end(numbers)};
  auto half = unique_numbers.size() / 2;
  auto it = std::begin(unique_numbers);
  for (auto size = std::size_t{0}; size < half; ++size) {
    REQUIRE(skip_list.Erase(*it));
    ++it;
  }

  for (; it != std::end(unique_numbers); ++it) {
    REQUIRE(skip_list.Contains(*it));
  }
}

TEST_CASE("Sanity check", "[Correctness]") {
  auto skip_list = SL<int>{};

  auto numbers = chunk(10, random(0, 100)).get();
  for (auto n : numbers) {
    skip_list.Insert(n);
  }

  for (auto n : numbers) {
    REQUIRE(skip_list.Contains(n));
  }

  auto other_numbers = chunk(100, random(-999, -900)).get();
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

TEST_CASE(
    "One thread inserts, another is trying to erase non-existent elements",
    "[Concurrency]") {
  auto skip_list = SL<int>{};

  auto to_insert = chunk(100 * kThousand, random(0, kThousand)).get();
  auto to_erase =
      chunk(100 * kThousand, random(2 * kThousand, 3 * kThousand)).get();

  auto inserter = std::thread([&]() {
    for (auto n : to_insert) {
      skip_list.Insert(n);
    }
  });
  auto eraser = std::thread([&]() {
    for (auto n : to_erase) {
      REQUIRE(!skip_list.Erase(n));
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

TEST_CASE("Stress test", "[Concurrency]") {
  auto skip_list = SL<int>{};
  constexpr auto kThreads = 16;
  constexpr auto kSingleChunk = 100 * kThousand;

  auto chunks = std::vector<std::vector<int>>{};
  chunks.reserve(kThreads);

  auto merged = std::vector<int>{};
  merged.reserve(kThreads * kSingleChunk);

  auto gen = chunk(kSingleChunk, random(kThousand, 10 * kThousand));

  for (auto i = 0; i < kThreads; ++i, gen.next()) {
    chunks.push_back(gen.get());
    merged.insert(std::end(merged), std::begin(chunks.back()),
                  std::end(chunks.back()));
  }

  auto routine = [&](std::size_t id) {
    for (auto n : chunks[id]) {
      skip_list.Insert(n);
    }
  };

  auto threads = std::vector<std::thread>{};
  threads.reserve(kThreads);

  for (auto i = std::size_t{0}; i < kThreads; ++i) {
    auto id = i;
    threads.emplace_back(routine, id);
  }

  for (auto&& thread : threads) {
    thread.join();
  }

  for (auto n : merged) {
    REQUIRE(skip_list.Contains(n));
  }
}

TEST_CASE("One thread inserts and second tries to erase the same element",
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
      skip_list.Erase(n);
    }
  });

  first.join();
  second.join();

  if (!skip_list.Contains(num)) {
    REQUIRE(skip_list.Insert(num));
  } else {
    REQUIRE(skip_list.Erase(num));
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
