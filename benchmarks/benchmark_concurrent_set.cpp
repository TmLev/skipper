#include <benchmark/benchmark.h>

#include <memory>

#include "utils/random.hpp"

#include "skipper/concurrent_set.hpp"
#include "skipper/guarded_set.hpp"

template <typename T>
using GSL = skipper::GuardedSkipListSet<T>;

template <typename T>
using SL = skipper::ConcurrentSkipListSet<T>;

auto guarded = std::unique_ptr<GSL<int>>{};
auto concurrent = std::unique_ptr<SL<int>>{};

static constexpr auto kThousand = 1'000;

static auto GuardedManyContainsQueries(benchmark::State& state) -> void {
  auto gen = std::mt19937{std::random_device{}()};
  auto dis = std::uniform_int_distribution{-10 * kThousand, 10 * kThousand};

  if (state.thread_index == 0) {
    guarded = std::make_unique<GSL<int>>();
    for (auto i = 0; i < kThousand * kThousand; ++i) {
      (*guarded)->Insert(dis(gen));
    }
  }

  for (auto _ : state) {
    if (state.thread_index == 0) {
      (*guarded)->Insert(dis(gen));
    } else {
      (*guarded)->Find(dis(gen));
    }
  }

  if (state.thread_index == 0) {
    guarded.reset();
  }
}

static auto ConcurrentManyContainsQueries(benchmark::State& state) -> void {
  auto gen = std::mt19937{std::random_device{}()};
  auto dis = std::uniform_int_distribution{-10 * kThousand, 10 * kThousand};

  if (state.thread_index == 0) {
    concurrent = std::make_unique<SL<int>>();
    for (auto i = 0; i < kThousand * kThousand; ++i) {
      concurrent->Insert(dis(gen));
    }
  }

  for (auto _ : state) {
    if (state.thread_index == 0) {
      concurrent->Insert(dis(gen));
    } else {
      concurrent->Contains(dis(gen));
    }
  }

  if (state.thread_index == 0) {
    concurrent.reset();
  }
}

BENCHMARK(GuardedManyContainsQueries)
    ->Threads(2)
    ->Threads(4)
    ->Threads(8)
    ->Threads(16)
    ->Threads(32)
    ->UseRealTime();

BENCHMARK(ConcurrentManyContainsQueries)
    ->Threads(2)
    ->Threads(4)
    ->Threads(8)
    ->Threads(16)
    ->Threads(32)
    ->UseRealTime();
