#include <benchmark/benchmark.h>

#include <memory>

#include "utils/random.hpp"

#include "skipper/lock_free_set.hpp"

template <typename T>
using SL = skipper::LockFreeSkipListSet<T>;

auto lock_free = std::unique_ptr<SL<int>>{};

static constexpr auto kThousand = 1'000;

static auto LockFreeManyContainsQueries(benchmark::State& state) -> void {
  auto gen = std::mt19937{std::random_device{}()};
  auto dis = std::uniform_int_distribution{-10 * kThousand, 10 * kThousand};

  if (state.thread_index == 0) {
    lock_free = std::make_unique<SL<int>>();
    for (auto i = 0; i < kThousand * kThousand; ++i) {
      lock_free->Insert(dis(gen));
    }
  }

  for (auto _ : state) {
    if (state.thread_index == 0) {
      lock_free->Insert(dis(gen));
    } else {
      lock_free->Contains(dis(gen));
    }
  }

  if (state.thread_index == 0) {
    lock_free.reset();
  }
}

BENCHMARK(LockFreeManyContainsQueries)
    ->Threads(2)
    ->Threads(4)
    ->Threads(8)
    ->Threads(16)
    ->Threads(32)
    ->UseRealTime();
