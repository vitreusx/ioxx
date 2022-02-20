#include "convert.h"
using namespace ioxx;

std::string convert_impl<std::string, std::string>::impl(std::string const& s) {
  return s;
}