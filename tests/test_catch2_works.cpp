#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

TEST_CASE("Catch2 works / stub test") {
  constexpr size_t kOne = 1;
  REQUIRE(kOne + 1 == 2);
}
