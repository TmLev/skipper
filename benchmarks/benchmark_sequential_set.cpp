#include <benchmark/benchmark.h>

#include <set>
#include <vector>

#include "../utils/random.hpp"

#include "skipper/sequential_set.hpp"

template <typename T>
using SL = skipper::SequentialSkipListSet<T>;

static auto SetInsertComplexity(benchmark::State& state) -> void {
  auto n = state.range(0);
  auto random_numbers =
      GenerateNumbers(static_cast<std::size_t>(n), 0, 2'000'000);

  for (auto _ : state) {
    auto set = std::set<int>{};
    for (auto number : random_numbers) {
      set.insert(number);
    }
  }

  state.SetComplexityN(n);
}

static auto SSLSInsertComplexity(benchmark::State& state) -> void {
  auto n = state.range(0);
  auto random_numbers =
      GenerateNumbers(static_cast<std::size_t>(n), 0, 2'000'000);

  for (auto _ : state) {
    auto skip_list = SL<int>{};
    for (auto number : random_numbers) {
      skip_list.Insert(number);
    }
  }

  state.SetComplexityN(n);
}

BENCHMARK(SetInsertComplexity)
    ->DenseRange(1'000, 10'000, 1'000)
    ->Complexity(benchmark::oNLogN);
BENCHMARK(SSLSInsertComplexity)
    ->DenseRange(1'000, 10'000, 1'000)
    ->Complexity(benchmark::oNLogN);
