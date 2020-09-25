#include <benchmark/benchmark.h>

#include <memory>

#include "utils/random.hpp"

#include "skipper/lock_free_set.hpp"

template <typename T>
using SL = skipper::LockFreeSkipListSet<T>;

auto lock_free = std::unique_ptr<SL<int>>{};

static constexpr auto kThousand = 1'000;

static auto LockFreeContainsQueries(benchmark::State& state) -> void {
  auto gen = std::mt19937{std::random_device{}()};
  auto dis = std::uniform_int_distribution{-10 * kThousand, 10 * kThousand};

  if (state.thread_index == 0) {
    lock_free = std::make_unique<SL<int>>();
    for (auto i = 0; i < kThousand * kThousand; ++i) {
      lock_free->Insert(dis(gen));
    }
  }

  for (auto _ : state) {
    lock_free->Contains(dis(gen));
  }

  if (state.thread_index == 0) {
    lock_free.reset();
  }
}

static auto LockFreeInsertQueries(benchmark::State& state) -> void {
  auto gen = std::mt19937{std::random_device{}()};
  auto dis = std::uniform_int_distribution{-10 * kThousand, 10 * kThousand};

  if (state.thread_index == 0) {
    lock_free = std::make_unique<SL<int>>();
    for (auto i = 0; i < kThousand; ++i) {
      lock_free->Insert(dis(gen));
    }
  }

  for (auto _ : state) {
    lock_free->Insert(dis(gen));
  }

  if (state.thread_index == 0) {
    lock_free.reset();
  }
}

static auto LockFreeOneInsertManyContainsQueries(benchmark::State& state)
    -> void {
  auto gen = std::mt19937{std::random_device{}()};
  auto dis = std::uniform_int_distribution{-10 * kThousand, 10 * kThousand};

  if (state.thread_index == 0) {
    lock_free = std::make_unique<SL<int>>();
    for (auto i = 0; i < 10 * kThousand; ++i) {
      lock_free->Insert(dis(gen));
    }
  }

  for (auto _ : state) {
    if (state.thread_index == 0) {
      for (auto i = 0; i < kThousand; ++i) {
        lock_free->Insert(dis(gen));
      }
    } else {
      for (auto i = 0; i < kThousand; ++i) {
        lock_free->Contains(dis(gen));
      }
    }
  }

  if (state.thread_index == 0) {
    lock_free.reset();
  }
}

BENCHMARK(LockFreeContainsQueries)
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

BENCHMARK(LockFreeInsertQueries)
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

BENCHMARK(LockFreeOneInsertManyContainsQueries)
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
