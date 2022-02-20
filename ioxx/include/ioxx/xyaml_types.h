#pragma once
#include "xyaml_def.h"
#include "csv.h"

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

  void connect(xyaml_node_proxy& proxy);
};

template <typename Row> struct xyaml_csv_node {
  xyaml_file file_node;
  csv<Row> csv_file;

  void connect(xyaml_node_proxy &proxy) {
    if (proxy.loading()) {
      proxy &file_node;
      csv_file = csv<Row>(file_node.source);
    } else {
      file_node.source = csv_file.save();
      proxy &file_node;
    }
  }
};

template<> struct xyaml_proxy_conn<std::filesystem::path> {
  static void connect(xyaml_node_proxy& proxy, std::filesystem::path& path);
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