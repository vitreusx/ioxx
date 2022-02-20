#include "xyaml_types.h"
#include <fstream>
#include <iostream>
using namespace ioxx;

void xyaml_proxy_conn<xyaml_node>::connect(xyaml_node_proxy &proxy, xyaml_node &node) {
  if (proxy.loading()) node = static_cast<xyaml_node&>(proxy);
  else static_cast<xyaml_node&>(proxy) = node;
}

void xyaml_file::connect(xyaml_node_proxy &proxy) {
  if (proxy.loading()) {
    if (proxy.IsScalar()) {
      source = proxy.as<std::string>();
    } else if (proxy["path"]) {
      path = std::filesystem::path(proxy["path"].as<std::string>());

      if (proxy["relative-to"])
        relative_to = proxy["relative-to"].as<std::string>();

      std::filesystem::path full_path;
      if (!relative_to.has_value() || relative_to.value() == "file")
        full_path = proxy.location->parent_path() / path.value();
      else
        full_path = path.value();

      std::ifstream file(full_path);
      std::stringstream ss {};
      ss << file.rdbuf();
      source = ss.str();
      std::cout << source << '\n';
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

      std::filesystem::create_directories(full_path.parent_path());
      std::ofstream file(full_path);
      file << source;
    } else {
      proxy = source;
      proxy.is_file = true;
    }
  }
}

void xyaml_embed::connect(xyaml_node_proxy &proxy) {
  if (proxy.loading()) {
    if (proxy.IsScalar() || proxy["path"]) {
      file = xyaml_file();
      proxy & file.value();
      node = xyaml_node::from_data(YAML::Load(file->source), proxy.location);
    }
    else {
      node = static_cast<xyaml_node&>(proxy);
    }
  }
  else {
    if (file.has_value()) {
      file->source = YAML::Dump(node);
      proxy & file.value();
    }
    else {
      proxy & node;
    }
  }
}

xyaml_node_proxy xyaml_embed::sub_proxy(xyaml_node_proxy const& super_proxy) {
  return xyaml_node_proxy(node, super_proxy.mode);
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

#define SCALAR_PROXY_IMPL(T)                                              \
  void xyaml_proxy_conn<T>::connect(xyaml_node_proxy &proxy, T &value) {       \
    connect_scalar<T>(proxy, value);                                           \
  }

SCALAR_PROXY_IMPL(std::string);
SCALAR_PROXY_IMPL(int);
SCALAR_PROXY_IMPL(double);
SCALAR_PROXY_IMPL(float);
SCALAR_PROXY_IMPL(char);
SCALAR_PROXY_IMPL(size_t);