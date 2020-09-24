#include <catch2/catch.hpp>

#include <iostream>
#include <sstream>

#include "skipper/detail/arena.hpp"

TEST_CASE("Arena allocates", "[Correctness]") {
  auto arena = skipper::detail::Arena{};

  auto ss = std::stringstream{};

  for (int i = 0; i < 4; ++i) {
    auto raw = arena.Allocate(sizeof(int));
    auto number = new (raw) int{i * i};
    ss << *number << ' ';
  }

  REQUIRE(ss.str() == "0 1 4 9 ");
}
