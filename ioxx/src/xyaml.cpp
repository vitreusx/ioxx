#include "xyaml.h"
#include <utility>

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

xyaml_node_proxy::xyaml_node_proxy(const xyaml_node &data, node_proxy_mode mode)
    : xyaml_node(data), mode{mode} {};

template <typename T>
static void scalar_conn(xyaml_node_proxy &proxy, T &value) {
  switch (proxy.mode) {
  case node_proxy_mode::LOAD: {
    auto node_str = proxy.YAML::Node::as<std::string>();
    value = convert<T, std::string>(node_str);
    break;
  }
  case node_proxy_mode::SAVE: {
    proxy.YAML::Node::operator=(convert<std::string, T>(value));
    break;
  }
  }
}

#define SCALAR_PROXY_CONN_IMPL(T)                                              \
  void xyaml_proxy_conn<T>::connect(xyaml_node_proxy &proxy, T &value) {       \
    scalar_conn<T>(proxy, value);                                              \
  }

APPLY_OVER_SCALARS(SCALAR_PROXY_CONN_IMPL);