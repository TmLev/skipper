#include <benchmark/benchmark.h>

#include <memory>

#include "utils/random.hpp"

#include "skipper/concurrent_set.hpp"

template <typename T>
using SL = skipper::ConcurrentSkipListSet<T>;

auto concurrent = std::unique_ptr<SL<int>>{};

static constexpr auto kThousand = 1'000;

static auto ConcurrentContainsQueries(benchmark::State& state) -> void {
  auto gen = std::mt19937{std::random_device{}()};
  auto dis = std::uniform_int_distribution{-10 * kThousand, 10 * kThousand};

  if (state.thread_index == 0) {
    concurrent = std::make_unique<SL<int>>();
    for (auto i = 0; i < kThousand * kThousand; ++i) {
      concurrent->Insert(dis(gen));
    }
  }

  for (auto _ : state) {
    concurrent->Contains(dis(gen));
  }

  if (state.thread_index == 0) {
    concurrent.reset();
  }
}

static auto ConcurrentInsertQueries(benchmark::State& state) -> void {
  auto gen = std::mt19937{std::random_device{}()};
  auto dis = std::uniform_int_distribution{-10 * kThousand, 10 * kThousand};

  if (state.thread_index == 0) {
    concurrent = std::make_unique<SL<int>>();
    for (auto i = 0; i < kThousand; ++i) {
      concurrent->Insert(dis(gen));
    }
  }

  for (auto _ : state) {
    concurrent->Insert(dis(gen));
  }

  if (state.thread_index == 0) {
    concurrent.reset();
  }
}

static auto ConcurrentOneInsertManyContainsQueries(benchmark::State& state)
    -> void {
  auto gen = std::mt19937{std::random_device{}()};
  auto dis = std::uniform_int_distribution{-10 * kThousand, 10 * kThousand};

  if (state.thread_index == 0) {
    concurrent = std::make_unique<SL<int>>();
    for (auto i = 0; i < 10 * kThousand; ++i) {
      concurrent->Insert(dis(gen));
    }
  }

  for (auto _ : state) {
    if (state.thread_index == 0) {
      for (auto i = 0; i < kThousand; ++i) {
        concurrent->Insert(dis(gen));
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

BENCHMARK(ConcurrentContainsQueries)
    ->Threads(1)
//        ->Threads(2)
//        ->Threads(4)
//        ->Threads(6)
//        ->Threads(8)
//        ->Threads(10)
//        ->Threads(12)
//        ->Threads(14)
//        ->Threads(16)
    ->UseRealTime();

BENCHMARK(ConcurrentInsertQueries)
    ->Threads(1)
//        ->Threads(2)
//        ->Threads(4)
//        ->Threads(6)
//        ->Threads(8)
//        ->Threads(10)
//        ->Threads(12)
//        ->Threads(14)
//        ->Threads(16)
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
