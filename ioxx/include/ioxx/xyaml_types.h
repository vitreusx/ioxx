#pragma once
#include "xyaml_def.h"

namespace ioxx {

template <> struct xyaml_proxy_conn<xyaml_node> {
  static void connect(xyaml_node_proxy& proxy, xyaml_node& node);
};

struct xyaml_file {
  std::string source;
  std::optional<std::string> relative_to;
  std::optional<std::filesystem::path> path;

  void connect(xyaml_node_proxy &proxy);
};

struct xyaml_embed {
  std::optional<xyaml_file> file;
  xyaml_node node;

  xyaml_node_proxy sub_proxy(xyaml_node_proxy const& super_proxy);

  void connect(xyaml_node_proxy& proxy);
};

#define SCALAR_PROXY(T)                                                   \
  template <> struct xyaml_proxy_conn<T> {                                     \
    static void connect(xyaml_node_proxy &proxy, T &value);                    \
  }

SCALAR_PROXY(std::string);
SCALAR_PROXY(int);
SCALAR_PROXY(double);
SCALAR_PROXY(float);
SCALAR_PROXY(char);
SCALAR_PROXY(size_t);
}