#pragma once
#include <ioxx/csv.h>
#include <ioxx/xyaml.h>

namespace ioxx {
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

}
