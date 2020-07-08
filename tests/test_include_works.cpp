#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <iostream>

#include "skipper/sequential.hpp"

TEST_CASE("Include works") {
  auto vec = std::vector{1, 2, 3};
  std::ostringstream oss;
  skipper::Print(vec, ", ", oss);
  REQUIRE(oss.str() == "1, 2, 3");
}
