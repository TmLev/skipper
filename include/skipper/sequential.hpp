#include <iostream>
#include <string_view>
#include <vector>

namespace skipper {

template <class T>
auto Print(const std::vector<T>& vector, const std::string_view& delimiter,
           std::ostream& out) -> void;

}  // namespace skipper

#include "skipper/sequential.ipp"
