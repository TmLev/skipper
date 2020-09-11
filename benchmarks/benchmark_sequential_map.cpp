#include <benchmark/benchmark.h>

#include <map>
#include <vector>

#include "../utils/random.hpp"

#include "skipper/sequential_map.hpp"

template <typename Key, typename Value>
using SM = skipper::SequentialSkipListMap<Key, Value>;

static auto SetIntInsertComplexity(benchmark::State& state) -> void {
  auto n = state.range(0);
  auto random_numbers =
      GenerateNumbers(static_cast<std::size_t>(n), 0, 2'000'000);

  for (auto _ : state) {
    auto map = std::map<int, int>{};
    for (auto number : random_numbers) {
      map.insert({number, number});
    }
  }

  state.SetComplexityN(n);
}

static auto SLIntInsertComplexity(benchmark::State& state) -> void {
  auto n = state.range(0);
  auto random_numbers =
      GenerateNumbers(static_cast<std::size_t>(n), 0, 2'000'000);

  for (auto _ : state) {
    auto skip_list = SM<int, int>{};
    for (auto number : random_numbers) {
      skip_list.Insert(number, number);
    }
  }

  state.SetComplexityN(n);
}

BENCHMARK(SetIntInsertComplexity)
    ->DenseRange(1'000, 10'000, 1'000)
    ->Complexity(benchmark::oNLogN);
BENCHMARK(SLIntInsertComplexity)
    ->DenseRange(1'000, 10'000, 1'000)
    ->Complexity(benchmark::oNLogN);
