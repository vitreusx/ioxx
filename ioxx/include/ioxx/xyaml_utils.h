#pragma once
#include "csv.h"
#include "xyaml_def.h"

namespace ioxx {

struct xyaml_file {
  std::string source;
  std::optional<std::filesystem::path> path;

  xyaml_file() = default;
  explicit xyaml_file(std::filesystem::path const &path);

  bool connect(xyaml_proxy &proxy);
};

struct xyaml_embedded {
  std::optional<xyaml_file> file;
  xyaml_node node;

  xyaml_embedded() = default;
  explicit xyaml_embedded(std::filesystem::path const &path);

  bool connect(xyaml_proxy &proxy);
};

template <> struct xyaml_connection<xyaml_node> {
  bool operator()(xyaml_proxy &proxy, xyaml_node &node) const;
};

template <typename Row> struct xyaml_csv {
  xyaml_file file_node;
  csv<Row> csv_file;

  bool connect(xyaml_proxy &proxy) {
    if (proxy.loading()) {
      proxy &file_node;
      csv_file = csv<Row>(file_node.source);
    } else {
      file_node.source = csv_file.save();
      proxy &file_node;
    }
    return true;
  }
};

template <typename Row> struct xyaml_connection<csv<Row>> {
  bool operator()(xyaml_proxy &proxy, csv<Row> &csv_file) const {
    xyaml_csv<Row> x_csv;
    if (proxy.saving())
      x_csv.csv_file = csv_file;

    if (proxy & x_csv) {
      if (proxy.loading())
        csv_file = x_csv.csv_file;
      return true;
    } else {
      return false;
    }
  }
};

template <> struct xyaml_connection<std::filesystem::path> {
  bool operator()(xyaml_proxy &proxy, std::filesystem::path &path) const;
};

struct xyaml_import {
  xyaml_node merged, overrides;
  std::vector<xyaml_embedded> imports;

  bool connect(xyaml_proxy &proxy);
};
} // namespace ioxx