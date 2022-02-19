#pragma once
#include <filesystem>
#include <ioxx/convert.h>
#include <optional>
#include <string>
#include <yaml-cpp/yaml.h>

class xyaml_node_proxy;
template <typename T> struct xyaml_proxy_conn {
  static void connect(xyaml_node_proxy &proxy, T &value) {
    value.connect(proxy);
  }
};

class xyaml_node : public YAML::Node {
public:
  xyaml_node() = default;

  static xyaml_node from_path(std::filesystem::path const &path);
  static xyaml_node
  from_data(const YAML::Node &node,
            std::optional<std::filesystem::path> location = std::nullopt);

  template <typename T> xyaml_node(T const &value);

  template <typename Key> xyaml_node operator[](Key key) {
    return xyaml_node(this->YAML::Node::operator[](key), location);
  }

  template <typename T> T as() const;

  template <typename T> xyaml_node &operator=(T const &value);

  std::optional<std::filesystem::path> location;

private:
  explicit xyaml_node(const YAML::Node &node,
                      std::optional<std::filesystem::path> location);
};

enum class node_proxy_mode { LOAD, SAVE };

class xyaml_node_proxy : public xyaml_node {
public:
  xyaml_node_proxy(xyaml_node const &data, node_proxy_mode mode);

  template <typename Key> xyaml_node_proxy operator[](Key key) {
    auto node = this->xyaml_node::operator[](key);
    return xyaml_node_proxy(node, mode);
  }

  template <typename T> xyaml_node_proxy &operator&(T &value) {
    xyaml_proxy_conn<T>::connect(*this, value);
    return *this;
  }

  node_proxy_mode mode;
};

template <typename T> T xyaml_node::as() const {
  auto proxy = xyaml_node_proxy(*this, node_proxy_mode::LOAD);
  T res;
  xyaml_proxy_conn<T>::connect(proxy, res);
  return res;
}

template <typename T> xyaml_node::xyaml_node(const T &value) { *this = value; }

template <typename T> xyaml_node &xyaml_node::operator=(const T &value) {
  reset(YAML::Node(YAML::NodeType::Map));
  auto proxy = xyaml_node_proxy(*this, node_proxy_mode::SAVE);
  xyaml_proxy_conn<T>::connect(proxy, const_cast<T &>(value));
  return *this;
}

#define SCALAR_PROXY_CONN(T)                                                   \
  template <> struct xyaml_proxy_conn<T> {                                     \
    static void connect(xyaml_node_proxy &proxy, T &value);                    \
  };

APPLY_OVER_SCALARS(SCALAR_PROXY_CONN);