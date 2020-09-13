#include <catch2/catch.hpp>

#include <set>
#include <sstream>
#include <utility>
#include <vector>

#include "skipper/sequential_map.hpp"

template <typename Key, typename Value>
using SM = skipper::SequentialSkipListMap<Key, Value>;

TEST_CASE("Check Sequential SkipList Map functionality", "[Functionality]") {
  auto skip_list = SM<int, int>{};
  auto [it1, success1] = skip_list.Insert(1, 1);
  auto [it2, success2] = skip_list.Insert(2, 2);

  SECTION("Iterator::operator*") {
    REQUIRE((*it1).key == 1);
  }
  SECTION("Iterator::operator->") {
    REQUIRE(it1->key == 1);
  }
  SECTION("Iterator::operator++") {
    auto copy = it1;
    ++copy;
    REQUIRE(copy->key == it2->key);
  }
  SECTION("Iterator::operator++(int)") {
    auto copy = it1;
    copy++;
    REQUIRE(copy->key == it2->key);
  }
  SECTION("Iterator::operator==") {
    auto copy = it1;
    ++copy;
    REQUIRE(copy == it2);
  }
  SECTION("Iterator::operator!=") {
    REQUIRE(it1 != it2);
  }

  SECTION("Find() works correctly with existent values") {
    auto f = skip_list.Find(1);
    REQUIRE(f == it1);
  }
  SECTION("Find() works correctly with non-existent values") {
    auto f = skip_list.Find(3);
    REQUIRE(f != it1);
  }
  SECTION("Insert() works correctly with existent values") {
    auto [it, success] = skip_list.Insert(1, 1);
    REQUIRE(!success);
  }
  SECTION("Insert() works correctly with non-existent values") {
    auto [it, success] = skip_list.Insert(5, 5);
    REQUIRE(success);
    REQUIRE(it->key == 5);
    REQUIRE(it->value == 5);
  }
  SECTION("operator[] access works correctly") {
    int value = skip_list[1];
    REQUIRE(value == it1->value);
  }
  SECTION("operator[] insert work correctly") {
    auto f = skip_list.Find(4);
    REQUIRE(f == skip_list.End());
    skip_list[4] = 4;
    f = skip_list.Find(4);
    REQUIRE(f != skip_list.End());
    REQUIRE(f->value == 4);
  }
  SECTION("Erase() works correctly with non-existent values") {
    auto ret = skip_list.Erase(0);
    REQUIRE(ret == 0);
  }
  SECTION("Erase() works correctly with existent values") {
    auto ret = skip_list.Erase(1);
    REQUIRE(ret == 1);
    REQUIRE(skip_list.Find(1) == skip_list.End());
  }
}
