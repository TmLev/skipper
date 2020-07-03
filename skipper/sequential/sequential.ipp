#include <iostream>
#include <vector>

template <class T>
auto Print(
const std::vector<T>& vector,
   const std::string_view& delimiter)
 -> void
{
  for (size_t i = 0; i < vector.size(); ++i) {
    if (i > 0) {
      std::cout << delimiter;
    }

    std::cout << vector[i];
  }
}
