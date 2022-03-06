#include <iostream>
#include <ioxx/ioxx.h>

int main() {
  using namespace ioxx::xyaml;
  auto first = node::import("data/first/ref.yml");
  std::cout << "first: " << first["data"].as<file>().fetch() << '\n';
  auto second = node::import("data/second/ref.yml");
  std::cout << "second: " << second["data"].as<file>().fetch() << '\n';
  first.merge(second);
  std::cout << "merged: " << first["data"].as<file>().fetch() << '\n';
  //  auto first = node::import("data/first/ref.yml");
  //  std::cout << "non-existent defined: " << first["non-existent"].IsDefined()
  //            << '\n';
  //  std::cout << "other defined: " << first["other"].IsDefined() << '\n';
  //  std::cout << "non-existent null: " << first["non-existent"].IsNull() <<
  //  '\n'; std::cout << "other null: " << first["other"].IsNull() << '\n';
  //  std::cout << "non-existent bool: " << std::boolalpha
  //            << (bool)first["non-existent"] << '\n';
  //  std::cout << "other bool: " << std::boolalpha << (bool)first["other"] <<
  //  '\n';
  return 0;
}