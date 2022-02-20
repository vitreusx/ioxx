#include "xyaml.h"
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

xyaml_node_proxy::xyaml_node_proxy(const xyaml_node &data, node_proxy_mode mode)
    : xyaml_node(data), mode{mode} {}

bool xyaml_node_proxy::loading() const {
  return mode == node_proxy_mode::LOAD;
};

void xyaml_file::connect(xyaml_node_proxy &proxy) {
  if (proxy.loading()) {
    if (proxy.IsScalar()) {
      source = proxy.as<std::string>();
    } else if (proxy["path"]) {
      path = std::filesystem::path(proxy["path"].as<std::string>());

      if (proxy["relative-to"])
        relative_to = proxy["relative-to"].as<std::string>();

      std::filesystem::path full_path;
      if (relative_to.has_value() && relative_to.value() == "file")
        full_path = proxy.location.value_or("") / path.value();
      else
        full_path = path.value();

      std::ifstream file(full_path);
      std::stringstream ss {};
      ss << file.rdbuf();
      source = ss.str();
    }
  } else {
    if (path.has_value()) {
      proxy["path"] = path.value().string();
      if (relative_to.has_value())
        proxy["relative-to"] = relative_to.value();

      std::filesystem::path full_path;
      if (relative_to.has_value() && relative_to.value() == "file")
        full_path = proxy.location.value_or("") / path.value();
      else
        full_path = path.value();

      std::ofstream file(full_path);
      file << source;
    } else {
      proxy = source;
      proxy.is_file = true;
    }
  }
}

template <typename T>
static void connect_scalar(xyaml_node_proxy &proxy, T &value) {
  if (proxy.loading()) {
    auto node_str = proxy.YAML::Node::as<std::string>();
    value = convert<T, std::string>(node_str);
  } else {
    proxy.YAML::Node::operator=(convert<std::string, T>(value));
  }
}

#define SCALAR_PROXY_CONN_IMPL(T)                                              \
  void xyaml_proxy_conn<T>::connect(xyaml_node_proxy &proxy, T &value) {       \
    connect_scalar<T>(proxy, value);                                           \
  }

APPLY_OVER_SCALARS(SCALAR_PROXY_CONN_IMPL);