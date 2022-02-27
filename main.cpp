#include <iostream>
#include <ioxx/xyaml/node.h>
#include <ioxx/xyaml/subnode.h>

int main() {
  using namespace ioxx::xyaml;
  auto saved = node::new_file("data/saved.yml");
  auto orig = node::import("data/orig.yml");
  orig.loc = "data/orig2.yml";
  saved["orig"] = subnode(orig);
  saved.save();
  return 0;
}