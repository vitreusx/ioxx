#include <iostream>
#include <ioxx/xyaml.h>

int main() {
  using namespace ioxx;
  auto defaults_yml = xyaml_node::from_path("data/defaults.yml");
  auto proxy = xyaml_proxy(defaults_yml, node_proxy_mode::LOAD);

  xyaml_node amino_acid_data;
  proxy["amino acid data"] & amino_acid_data;
  auto aa_proxy = proxy(amino_acid_data);

  csv<> heur_angles;
  proxy["heurestic angles"]["coefficients"] & heur_angles;

  std::cout << aa_proxy["default atom radii"]["C"].as<std::string>() << '\n';

  return 0;
}