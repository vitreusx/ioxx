#include "convert.h"

#define PARSE_IMPL(T) T convert_impl<T, std::string>::impl(std::string const &s)

PARSE_IMPL(std::string) { return s; }

PARSE_IMPL(float) { return std::stof(s); }

PARSE_IMPL(double) { return std::stod(s); }

PARSE_IMPL(int) { return std::stoi(s); }

PARSE_IMPL(size_t) { return std::stoull(s); }

PARSE_IMPL(char) {
  if (s.size() != 1)
    throw;
  else
    return s[0];
}
