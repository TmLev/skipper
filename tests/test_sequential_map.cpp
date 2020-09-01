#include <catch2/catch.hpp>

#include <set>
#include <sstream>
#include <utility>
#include <vector>

#include "../utils/random.hpp"

#include "skipper/sequential_map.hpp"

template <typename Key, typename Value>
using SM = skipper::SequentialSkipListMap<Key, Value>;

TEST_CASE("Check Sequential SkipList Map coverage", "[Coverage]") {
  auto skip_list = SM<int, int>{};
  auto [it1, success1] = skip_list.Insert(1, 1);
  auto [it2, success2] = skip_list.Insert(2, 2);

  SECTION("Iterator::operator*") {
    REQUIRE((*it1).first == 1);
  }
  SECTION("Iterator::operator->") {
    REQUIRE(it1->first == 1);
  }
  SECTION("Iterator::operator++") {
    auto copy = it1;
    ++copy;
    REQUIRE(copy->first == it2->first);
  }
  SECTION("Iterator::operator++(int)") {
    auto copy = it1;
    copy++;
    REQUIRE(copy->first == it2->first);
  }
  SECTION("Iterator::operator==") {
    auto copy = it1;
    ++copy;
    REQUIRE(copy == it2);
  }
  SECTION("Iterator::operator!=") {
    REQUIRE(it1 != it2);
  }

  SECTION("Find()") {
    auto f1 = skip_list.Find(1);
    REQUIRE(f1 == it1);

    auto f3 = skip_list.Find(3);
    REQUIRE(f3 != it1);
  }
  SECTION("Insert()") {
    auto [it, success] = skip_list.Insert(1, 1);
    REQUIRE(!success);

    auto [it5, success5] = skip_list.Insert(5, 5);
    REQUIRE(success5);
    REQUIRE(it5->first == 5);
    REQUIRE(it5->second == 5);
  }
  SECTION("operator[]") {
    int value = skip_list[1];
    REQUIRE(value == it1->second);
  }
}
