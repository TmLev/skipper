#include <benchmark/benchmark.h>

#include <memory>

#include "utils/random.hpp"

#include "skipper/concurrent_map.hpp"
#include "skipper/guarded_map.hpp"

template <typename Key, typename Value>
using GSL = skipper::GuardedSkipListMap<Key, Value>;

template <typename Key, typename Value>
using SL = skipper::ConcurrentSkipListMap<Key, Value>;

auto guarded = std::unique_ptr<GSL<int, int>>{};
auto concurrent = std::unique_ptr<SL<int, int>>{};

static constexpr auto kThousand = 1'000;

// Initially insert some numbers
// Many threads check for contain
//
static auto ConcurrentManyContainsQueries(benchmark::State& state) -> void {
  auto gen = std::mt19937{std::random_device{}()};
  auto dis = std::uniform_int_distribution{-1 * kThousand, 1 * kThousand};

  if (state.thread_index == 0) {
    concurrent = std::make_unique<SL<int, int>>();
    for (auto i = 0; i < kThousand * kThousand; ++i) {
      auto num = dis(gen);
      concurrent->Insert(num, num);
    }
  }

  for (auto _ : state) {
    auto num = dis(gen);
    concurrent->Contains(num);
  }

  if (state.thread_index == 0) {
    concurrent.reset();
  }
}

// Initially insert some numbers
// One thread inserts, others check for contain
//
static auto ConcurrentInitialOneInsertManyContainsQueries(
    benchmark::State& state) -> void {
  auto gen = std::mt19937{std::random_device{}()};
  auto dis = std::uniform_int_distribution{-1 * kThousand, 1 * kThousand};

  if (state.thread_index == 0) {
    concurrent = std::make_unique<SL<int, int>>();
    for (auto i = 0; i < kThousand * kThousand; ++i) {
      auto num = dis(gen);
      concurrent->Insert(num, num);
    }
  }

  for (auto _ : state) {
    auto num = dis(gen);
    if (state.thread_index == 0) {
      concurrent->Insert(num, num);
    } else {
      concurrent->Contains(num);
    }
  }

  if (state.thread_index == 0) {
    concurrent.reset();
  }
}

// No initial setup
// One thread inserts, others check for contain
//
static auto ConcurrentOneInsertManyContainsQueries(benchmark::State& state)
    -> void {
  auto gen = std::mt19937{std::random_device{}()};
  auto dis = std::uniform_int_distribution{-1 * kThousand, 1 * kThousand};

  if (state.thread_index == 0) {
    concurrent = std::make_unique<SL<int, int>>();
  }

  for (auto _ : state) {
    auto num = dis(gen);
    if (state.thread_index == 0) {
      concurrent->Insert(num, num);
    } else {
      concurrent->Contains(num);
    }
  }

  if (state.thread_index == 0) {
    concurrent.reset();
  }
}

// Initially insert some numbers
// One thread inserts, second (if exists) erases, others (if exist) check for
// contain, every action in a loop
//
static auto ConcurrentOneInsertOneEraseManyContainsQueries(
    benchmark::State& state) -> void {
  auto gen = std::mt19937{std::random_device{}()};
  auto dis = std::uniform_int_distribution{-1 * kThousand, 1 * kThousand};

  if (state.thread_index == 0) {
    concurrent = std::make_unique<SL<int, int>>();
    for (auto i = 0; i < kThousand * kThousand; ++i) {
      auto num = dis(gen);
      concurrent->Insert(num, num);
    }
  }

  for (auto _ : state) {
    if (state.thread_index == 0) {
      for (auto i = 0; i < kThousand; ++i) {
        auto num = dis(gen);
        concurrent->Insert(num, num);
      }
    } else if (state.threads > 1 && state.thread_index == 1) {
      for (auto i = 0; i < kThousand; ++i) {
        concurrent->Erase(dis(gen));
      }
    } else {
      for (auto i = 0; i < kThousand; ++i) {
        concurrent->Contains(dis(gen));
      }
    }
  }

  if (state.thread_index == 0) {
    concurrent.reset();
  }
}

////////////////////////////////////////////////////////////////////////////////
////
//// Run benchmarks
////

BENCHMARK(ConcurrentManyContainsQueries)
    ->Threads(1)
    ->Threads(2)
    ->Threads(4)
    ->Threads(6)
    ->Threads(8)
    ->Threads(10)
    ->Threads(12)
    ->Threads(14)
    ->Threads(16)
    ->UseRealTime();

BENCHMARK(ConcurrentInitialOneInsertManyContainsQueries)
    ->Threads(1)
    ->Threads(2)
    ->Threads(4)
    ->Threads(6)
    ->Threads(8)
    ->Threads(10)
    ->Threads(12)
    ->Threads(14)
    ->Threads(16)
    ->UseRealTime();

BENCHMARK(ConcurrentOneInsertManyContainsQueries)
    ->Threads(1)
    ->Threads(2)
    ->Threads(4)
    ->Threads(6)
    ->Threads(8)
    ->Threads(10)
    ->Threads(12)
    ->Threads(14)
    ->Threads(16)
    ->UseRealTime();

BENCHMARK(ConcurrentOneInsertOneEraseManyContainsQueries)
    ->Threads(1)
    ->Threads(2)
    ->Threads(4)
    ->Threads(6)
    ->Threads(8)
    ->Threads(10)
    ->Threads(12)
    ->Threads(14)
    ->Threads(16)
    ->UseRealTime();
