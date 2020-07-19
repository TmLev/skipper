#include <benchmark/benchmark.h>

#include <memory>
#include <mutex>

int counter = 0;
std::shared_ptr<std::mutex> mutex;

static auto Run(benchmark::State& state) -> void {
  if (state.thread_index == 0) {
    counter = 0;
    mutex = std::make_shared<std::mutex>();
  }

  while (state.KeepRunning()) {
    std::unique_lock lock(*mutex);
    ++counter;
  }
}

BENCHMARK(Run)->UseRealTime()->Threads(1)->Threads(2)->Threads(4)->Threads(8);

BENCHMARK_MAIN();
