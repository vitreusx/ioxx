#pragma once
#include <experimental/type_traits>
#include <filesystem>
#include <ioxx/convert.h>
#include <memory>
#include <optional>
#include <string>
#include <yaml-cpp/yaml.h>

namespace ioxx {
class xyaml_node;
class xyaml_proxy;
template <typename T> struct xyaml_connection;

class xyaml_node : public YAML::Node {
public:
  xyaml_node() = default;
  xyaml_node(YAML::Node const &node);
  xyaml_node(xyaml_node const &other) = default;

  static xyaml_node from_path(std::filesystem::path const &path);
  static xyaml_node
  from_data(const YAML::Node &node,
            std::optional<std::filesystem::path> location = std::nullopt);

  template <typename T> xyaml_node(T const &value);

  template <typename Key> xyaml_node operator[](Key key) {
    return xyaml_node(this->YAML::Node::operator[](key), location);
  }

  template <typename T> T as() const;

  xyaml_node &operator=(xyaml_node const &other);
  template <typename T> xyaml_node &operator=(T const &value);

  friend YAML::Emitter &operator<<(YAML::Emitter &out, xyaml_node const &node);

  std::optional<std::filesystem::path> location;
  bool is_file = false;

private:
  explicit xyaml_node(const YAML::Node &node,
                      std::optional<std::filesystem::path> location);
};

enum class node_proxy_mode { LOAD, SAVE };

class xyaml_proxy : public xyaml_node {
public:
  xyaml_proxy() = default;
  explicit xyaml_proxy(xyaml_node const &data, node_proxy_mode mode);

  template <typename Key> xyaml_proxy operator[](Key key) {
    auto node = this->xyaml_node::operator[](key);
    return xyaml_proxy(node, mode);
  }

  template <typename T> xyaml_proxy &operator&(T &value) {
    xyaml_connection<T>()(*this, value);
    return *this;
  }

  template <typename T> xyaml_proxy &operator=(T const &value) {
    this->xyaml_node::operator=(value);
    return *this;
  }

  void connect(xyaml_proxy &proxy) const;
  xyaml_proxy operator()(xyaml_node const &node) const;

  node_proxy_mode mode;
  bool saving() const;
  bool loading() const;
};

template <typename T> T xyaml_node::as() const {
  auto proxy = xyaml_proxy(*this, node_proxy_mode::LOAD);
  T res;
  xyaml_connection<T>()(proxy, res);
  return res;
}

template <typename T> xyaml_node::xyaml_node(const T &value) { *this = value; }

template <typename T> xyaml_node &xyaml_node::operator=(const T &value) {
  reset(YAML::Node(YAML::NodeType::Map));
  auto proxy = xyaml_proxy(*this, node_proxy_mode::SAVE);
  xyaml_connection<T>()(proxy, const_cast<T &>(value));
  return *this;
}

template <typename T>
using xyaml_connect_t =
    decltype(std::declval<T &>().connect(std::declval<xyaml_proxy &>()));

template <typename T> struct xyaml_connection {
  void operator()(xyaml_proxy &proxy, T &value) const {
    using namespace std::experimental;

    if constexpr (is_detected_exact_v<void, xyaml_connect_t, T>) {
      value.connect(proxy);
    } else {
      if (proxy.loading()) {
        value = proxy.YAML::Node::as<T>();
      } else {
        proxy.YAML::Node::operator=(value);
      }
    }
  }
};

} // namespace ioxx
