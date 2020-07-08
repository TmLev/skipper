#include <string_view>
#include <vector>

namespace skipper {

template <class T>
auto Print(const std::vector<T>& vector, const std::string_view& delimiter)
    -> void;

}  // namespace skipper

#include "skipper/sequential.ipp"
