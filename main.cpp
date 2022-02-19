#include <fstream>
#include <iostream>
#include <ioxx/csv.h>
#include <ioxx/xyaml.h>

struct defaults {
  std::string total_time;
  int seed;

  void connect(xyaml_node_proxy &proxy) {
    proxy["general"]["total time"] & total_time;
    proxy["general"]["seed"] & seed;
  }
};

int main() {
  auto node = xyaml_node::from_path("data/defaults.yml");
  auto value = node.as<defaults>();
  xyaml_node saved = value;
  std::cout << YAML::Dump(saved) << '\n';

  return 0;
}