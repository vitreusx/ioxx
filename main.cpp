#include <iostream>
#include <ioxx/xyaml.h>

int main() {
  using namespace ioxx;
  auto defaults_yml = xyaml_node::from_path("data/default/inputfile.yml");
  std::cout << YAML::Dump(defaults_yml) << '\n';
  return 0;
}