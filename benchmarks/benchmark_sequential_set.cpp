#include <benchmark/benchmark.h>

#include <random>
#include <set>
#include <vector>

#include "skipper/sequential_set.hpp"

template <typename T>
using SL = skipper::SequentialSkipListSet<T>;

static auto GenerateRandomNumbers(std::size_t count) -> std::vector<int> {
  auto result = std::vector<int>{};
  result.reserve(count);

  auto rd = std::random_device{};
  auto gen = std::mt19937{rd()};
  auto dis = std::uniform_int_distribution<>{0, 2'000'000};

  for (auto i = std::size_t{0}; i < count; ++i) {
    result.push_back(dis(gen));
  }

  return result;
}

static auto SetInsertComplexity(benchmark::State& state) -> void {
  auto n = state.range(0);
  auto random_numbers = GenerateRandomNumbers(static_cast<std::size_t>(n));

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
  auto random_numbers = GenerateRandomNumbers(static_cast<std::size_t>(n));

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
