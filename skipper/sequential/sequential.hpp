#include <vector>

template <class T>
auto Print(const std::vector<T>& vector, const std::string_view& delimiter) -> void;

// TODO: fix CMake to specify full path (skipper/sequential/sequential.ipp)
#include "sequential.ipp"
