#include <catch2/catch.hpp>

#include <algorithm>
#include <thread>
#include <unordered_map>

#include "skipper/concurrent_map.hpp"

using Catch::Generators::chunk;
using Catch::Generators::random;

template <typename Key, typename Value>
using SL = skipper::ConcurrentSkipListMap<Key, Value>;

static constexpr auto kThousand = 1'000;

TEST_CASE("Check Sequential SkipList Map functionality", "[Functionality]") {
  auto numbers = chunk(kThousand, random(0, 100)).get();

  SECTION("Insert() returns true for new elements and false otherwise",
          "[Correctness]") {
    auto skip_list = SL<int, int>{};

    auto unique_numbers = std::unordered_map<int, int>{};

    for (auto n : numbers) {
      if (unique_numbers.find(n) != std::end(unique_numbers)) {
        REQUIRE(!skip_list.Insert(n, n));
      } else {
        REQUIRE(skip_list.Insert(n, n));
        unique_numbers.insert({n, n});
      }
    }
  }

  auto unique_numbers = std::unordered_map<int, int>{};
  for (auto n : numbers) {
    unique_numbers[n] = n;
  }

  SECTION("Contains() returns true for existing elements and false otherwise",
          "[Correctness]") {
    auto skip_list = SL<int, int>{};

    auto other_numbers =
        chunk(kThousand, random(2 * kThousand, 3 * kThousand)).get();

    for (auto n : numbers) {
      skip_list.Insert(n, n);
    }

    for (auto n : numbers) {
      REQUIRE(skip_list.Contains(n));
    }

    for (auto n : other_numbers) {
      REQUIRE(!skip_list.Contains(n));
    }
  }

  SECTION("Erase() removes requested elements and does not remove other",
          "[Correctness]") {
    auto skip_list = SL<int, int>{};

    for (auto n : numbers) {
      skip_list.Insert(n, n);
    }

    auto half = unique_numbers.size() / 2;
    auto it = std::begin(unique_numbers);
    for (auto size = std::size_t{0}; size < half; ++size) {
      REQUIRE(skip_list.Erase(it->first));
      ++it;
    }

    for (; it != std::end(unique_numbers); ++it) {
      REQUIRE(skip_list.Contains(it->first));
    }
  }
}

TEST_CASE("Two threads insert repeating numbers simultaneously",
          "[Concurrency]") {
  auto skip_list = SL<int, int>{};

  auto first_numbers = chunk(10 * kThousand, random(0, kThousand)).get();
  auto second_numbers = chunk(10 * kThousand, random(0, kThousand)).get();

  auto first = std::thread([&]() {
    for (auto n : first_numbers) {
      skip_list.Insert(n, n);
    }
  });

  auto second = std::thread([&]() {
    for (auto n : second_numbers) {
      skip_list.Insert(n, n);
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
  auto skip_list = SL<int, int>{};

  auto first_numbers = chunk(10 * kThousand, random(0, kThousand)).get();
  auto second_numbers =
      std::vector<int>(std::begin(first_numbers), std::end(first_numbers));

  auto first = std::thread([&]() {
    for (auto n : first_numbers) {
      skip_list.Insert(n, n);
    }
  });

  auto second = std::thread([&]() {
    for (auto n : second_numbers) {
      skip_list.Insert(n, n);
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

TEST_CASE("Two threads insert different repeating numbers simultaneously",
          "[Concurrency]") {
  auto skip_list = SL<int, int>{};

  auto first_numbers =
      chunk(10 * kThousand, random(2 * kThousand, 3 * kThousand)).get();
  auto second_numbers = chunk(10 * kThousand, random(0, kThousand)).get();

  auto first = std::thread([&]() {
    for (auto n : first_numbers) {
      skip_list.Insert(n, n);
    }
  });

  auto second = std::thread([&]() {
    for (auto n : second_numbers) {
      skip_list.Insert(n, n);
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

TEST_CASE(
    "One thread inserts, another is trying to erase non-existent elements",
    "[Concurrency]") {
  auto skip_list = SL<int, int>{};

  auto first_numbers = chunk(10 * kThousand, random(0, kThousand)).get();
  auto second_numbers =
      chunk(10 * kThousand, random(2 * kThousand, 3 * kThousand)).get();

  auto first = std::thread([&]() {
    for (auto n : first_numbers) {
      skip_list.Insert(n, n);
    }
  });

  auto second = std::thread([&]() {
    for (auto n : second_numbers) {
      skip_list.Erase(n);
    }
  });

  first.join();
  second.join();

  for (auto n : first_numbers) {
    REQUIRE(skip_list.Contains(n));
  }
  for (auto n : second_numbers) {
    REQUIRE(!skip_list.Contains(n));
  }
}

TEST_CASE("One thread inserts and second tries to erase the same element",
          "[Concurrency]") {
  auto skip_list = SL<int, int>{};

  int num = 10;

  auto first_numbers = chunk(kThousand, random(num, num)).get();
  auto second_numbers = chunk(kThousand, random(num, num)).get();

  auto first = std::thread([&]() {
    for (auto n : first_numbers) {
      skip_list.Insert(n, n);
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
    REQUIRE(skip_list.Insert(num, num));
  } else {
    REQUIRE(skip_list.Erase(num));
  }
}

TEST_CASE("One thread inserts and second check for contain the same element",
          "[Concurrency]") {
  auto skip_list = SL<int, int>{};

  int num = 10;

  auto first_numbers = chunk(kThousand, random(num, num)).get();
  auto second_numbers = chunk(kThousand, random(num, num)).get();

  auto first = std::thread([&]() {
    for (auto n : first_numbers) {
      skip_list.Insert(n, n);
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
  auto skip_list = SL<int, int>{};

  auto first_write_numbers =
      chunk(10 * kThousand, random(0, kThousand - 1)).get();
  auto second_write_numbers =
      chunk(10 * kThousand, random(kThousand, 2 * kThousand)).get();

  auto first_write_size = static_cast<int>(first_write_numbers.size());
  auto second_write_size = static_cast<int>(second_write_numbers.size());

  auto first_read_numbers =
      std::vector<int>(std::begin(first_write_numbers),
                       std::begin(first_write_numbers) + first_write_size / 2);
  auto second_read_numbers = std::vector<int>(
      std::begin(first_write_numbers) + first_write_size / 2 + 1,
      std::end(first_write_numbers));

  auto third_read_numbers = std::vector<int>(
      std::begin(second_write_numbers),
      std::begin(second_write_numbers) + second_write_size / 2);
  auto fourth_read_numbers = std::vector<int>(
      std::begin(second_write_numbers) + second_write_size / 2 + 1,
      std::end(second_write_numbers));

  auto first_write = std::thread([&]() {
    for (auto n : first_write_numbers) {
      skip_list.Insert(n, n);
    }
  });
  auto second_write = std::thread([&]() {
    for (auto n : second_write_numbers) {
      skip_list.Insert(n, n);
    }
  });

  first_write.join();
  second_write.join();

  auto first_read = std::thread([&]() {
    for (auto n : first_read_numbers) {
      REQUIRE(skip_list.Contains(n));
    }
  });
  auto second_read = std::thread([&]() {
    for (auto n : second_read_numbers) {
      REQUIRE(skip_list.Contains(n));
    }
  });
  auto third_read = std::thread([&]() {
    for (auto n : third_read_numbers) {
      REQUIRE(skip_list.Contains(n));
    }
  });
  auto fourth_read = std::thread([&]() {
    for (auto n : fourth_read_numbers) {
      REQUIRE(skip_list.Contains(n));
    }
  });

  first_read.join();
  second_read.join();
  third_read.join();
  fourth_read.join();
}
