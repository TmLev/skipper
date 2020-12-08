# CMake

> There are only two kinds of build systems: the ones people complain about and the ones nobody uses.  
> 
> &mdash; <cite>[lt_algorithm_gt](https://www.reddit.com/r/cpp/comments/ihi37f/what_is_the_point_of_this_talk_just_making_fun_of/g34ex3x?utm_source=share&utm_medium=web2x&context=3)</cite>
> (adapted from Bjarne Stroustrup)

Note that building tests and benchmarks for Skipper is enabled by default 
(and for that, `Catch2` and `Google/Benchmark` are required).  
In order to turn them off, set corresponding variables to `OFF`:
```cmake
set(SKIPPER_ENABLE_TESTS OFF CACHE BOOL "Enable testing of the skipper library")
set(SKIPPER_ENABLE_BENCHMARKS OFF CACHE BOOL "Enable benchmarking of the skipper library")
```

First things first, let CMake know about Skipper by adding directory with project:
```cmake
add_subdirectory(skipper)
```

Next, link your target against project-provided `skipper::skipper` target alias using `target_link_libraries`:
```cmake
target_link_libraries(MyTarget PRIVATE skipper::skipper)
```

Now, Skipper is available for `#include`-ing:
```cpp
#include <skipper/sequential_set.hpp>

auto main() -> int {
  auto skip_list = skipper::SequentialSkipListSet<int>{};
  skip_list.Insert(1);
}
```
