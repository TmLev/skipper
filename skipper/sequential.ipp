#include <iostream>
#include <string_view>
#include <vector>

#include <experimental/iterator>

namespace skipper {

template <class T>
auto Print(const std::vector<T>& vector, const std::string_view& delimiter)
    -> void {
  std::copy(vector.begin(), vector.end(),
            std::experimental::make_ostream_joiner(std::cout, delimiter));
}

}  // namespace skipper
