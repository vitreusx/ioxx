#pragma once
#include <string>

template <typename V, typename U> struct convert_impl {
  static V impl(U const &x) {
    auto repr = convert_impl<std::string, U>::impl(x);
    return convert_impl<V, std::string>::impl(repr);
  }
};

template <typename U> struct convert_impl<std::string, U> {
  static std::string impl(U const &x) { return std::to_string(x); }
};

template <typename V, typename U> V convert(U const &x) {
  return convert_impl<V, U>::impl(x);
}

#define PARSE(T)                                                               \
  template <> struct convert_impl<T, std::string> {                            \
    static T impl(std::string const &s);                                       \
  };

PARSE(std::string);
PARSE(float);
PARSE(double);
PARSE(int);
PARSE(size_t);
PARSE(char);
