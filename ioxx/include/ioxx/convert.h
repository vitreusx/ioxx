#pragma once
#include <sstream>
#include <string>

namespace ioxx {
template <typename V, typename U> struct convert_impl {
  static V impl(U const &x) {
    auto repr = convert_impl<std::string, U>::impl(x);
    return convert_impl<V, std::string>::impl(repr);
  }
};

template<> struct convert_impl<std::string, std::string> {
  static std::string impl(std::string const& s);
};

template <typename U> struct convert_impl<std::string, U> {
  static std::string impl(U const &x) {
    std::stringstream ss;
    ss << x;
    return ss.str();
  }
};

template <typename V> struct convert_impl<V, std::string> {
  static V impl(std::string const &repr) {
    std::stringstream ss;
    ss << repr;
    V value;
    ss >> value;
    return value;
  }
};

template <typename V, typename U> V convert(U const &x) {
  return convert_impl<V, U>::impl(x);
}
}; // namespace ioxx
