#include "xyaml_def.h"
#include <fstream>
#include <sstream>
#include <utility>
using namespace ioxx;

xyaml_node xyaml_node::from_path(const std::filesystem::path &path) {
  auto data = YAML::LoadFile(path.string());
  return xyaml_node::from_data(data, path);
}

xyaml_node
xyaml_node::from_data(const YAML::Node &node,
                      std::optional<std::filesystem::path> location) {
  return xyaml_node(node, std::move(location));
}

xyaml_node::xyaml_node(const YAML::Node &node,
                       std::optional<std::filesystem::path> location)
    : YAML::Node(node), location(std::move(location)) {}

xyaml_proxy::xyaml_proxy(const xyaml_node &data, node_proxy_mode mode)
    : xyaml_node(data), mode{mode} {}

bool xyaml_proxy::loading() const { return mode == node_proxy_mode::LOAD; };

bool xyaml_proxy::saving() const { return mode == node_proxy_mode::SAVE; }
xyaml_proxy xyaml_proxy::operator()(const xyaml_node &node) const {
  return xyaml_proxy(node, mode);
}

xyaml_node &xyaml_node::operator=(const xyaml_node &other) {
  this->YAML::Node::operator=(static_cast<YAML::Node const &>(other));
  location = other.location;
  is_file = other.is_file;
  return *this;
}

YAML::Emitter &operator<<(YAML::Emitter &out, const xyaml_node &node) {
  if (node.is_file) {
    out << YAML::EMITTER_MANIP::Literal << node.Scalar();
  } else {
    out << static_cast<YAML::Node const &>(node);
  }
  return out;
}

xyaml_node::xyaml_node(const YAML::Node &node)
    : YAML::Node(node), location{std::nullopt} {};

xyaml_node xyaml_node::operator[](const std::string &key) const {
  if (portals.find(key) != portals.end())
    return *portals.at(key);
  else
    return xyaml_node(this->YAML::Node::operator[](key), location);
}

xyaml_node xyaml_node::operator[](int key) const {
  return this->operator[](std::to_string(key));
}