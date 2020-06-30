#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

TEST_CASE("Catch2 works") {
  constexpr size_t one = 1;
  REQUIRE(one + 1 >= 2);
}
