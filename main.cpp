#include <iostream>

#include "skipper/sequential.hpp"

int main() {
  auto countdown = std::vector{1, 2, 3};
  skipper::Print(countdown, ", ");
  std::cout << "! Skipper, we are ready!" << std::endl;
  return 0;
}
