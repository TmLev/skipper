#include <benchmark/benchmark.h>

#include <memory>

#include "utils/random.hpp"

#include "skipper/guarded_set.hpp"

template <typename T>
using GSL = skipper::GuardedSkipListSet<T>;

auto guarded = std::unique_ptr<GSL<int>>{};

static constexpr auto kThousand = 1'000;

static auto GuardedContainsQueries(benchmark::State& state) -> void {
  auto gen = std::mt19937{std::random_device{}()};
  auto dis = std::uniform_int_distribution{-10 * kThousand, 10 * kThousand};

  if (state.thread_index == 0) {
    guarded = std::make_unique<GSL<int>>();
    for (auto i = 0; i < kThousand * kThousand; ++i) {
      (*guarded)->Insert(dis(gen));
    }
  }

  for (auto _ : state) {
    (*guarded)->Find(dis(gen));
  }

  if (state.thread_index == 0) {
    guarded.reset();
  }
}

static auto GuardedInsertQueries(benchmark::State& state) -> void {
  auto gen = std::mt19937{std::random_device{}()};
  auto dis = std::uniform_int_distribution{-10 * kThousand, 10 * kThousand};

  if (state.thread_index == 0) {
    guarded = std::make_unique<GSL<int>>();
    for (auto i = 0; i < kThousand; ++i) {
      (*guarded)->Insert(dis(gen));
    }
  }

  for (auto _ : state) {
    (*guarded)->Insert(dis(gen));
  }

  if (state.thread_index == 0) {
    guarded.reset();
  }
}

BENCHMARK(GuardedContainsQueries)
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

BENCHMARK(GuardedInsertQueries)
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