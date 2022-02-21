#include "xyaml_utils.h"
#include "merge_yaml.h"
#include <fstream>
#include <iostream>
using namespace ioxx;

xyaml_file::xyaml_file(const std::filesystem::path &path) {
  this->path = path;

  std::ifstream file(path);
  std::stringstream ss{};
  ss << file.rdbuf();
  source = ss.str();
}

void xyaml_file::connect(xyaml_proxy &proxy) {
  if (proxy.loading()) {
    if (proxy.IsScalar()) {
      source = proxy.as<std::string>();
    } else if (proxy["__path"]) {
      path = std::filesystem::path(proxy["__path"].as<std::string>());
      auto full_path = proxy.location->parent_path() / path.value();

      std::ifstream file(full_path);
      std::stringstream ss{};
      ss << file.rdbuf();
      source = ss.str();
    } else {
      throw;
    }
  } else {
    if (path.has_value()) {
      proxy["__path"] = path.value().string();
      auto full_path = proxy.location.value_or("") / path.value();
      std::filesystem::create_directories(full_path.parent_path());
      std::ofstream file(full_path);
      file << source;
    } else {
      proxy = source;
      proxy.is_file = true;
    }
  }
}

xyaml_embedded::xyaml_embedded(const std::filesystem::path &path) {
  file = xyaml_file(path);
  auto data = YAML::Load(file->source);
  node = xyaml_node::from_data(data, path);
}

void xyaml_embedded::connect(xyaml_proxy &proxy) {
  if (proxy.loading()) {
    auto node_file = xyaml_file();
    if (proxy & node_file) {
      file = node_file;
      auto path = proxy.location.value_or("") / node_file.path.value_or("");
      auto data = YAML::Load(node_file.source);
      node = xyaml_node::from_data(data, path);
    } else {
      node = static_cast<xyaml_node &>(proxy);
    }
  } else {
    if (file.has_value()) {
      file->source = YAML::Dump(node);
      proxy &file.value();
    } else {
      static_cast<xyaml_node &>(proxy) = node;
    }
  }
}

void xyaml_connection<xyaml_node>::operator()(xyaml_proxy &proxy,
                                              xyaml_node &node) const {
  xyaml_embedded embedded;
  if (proxy.saving())
    embedded.node = node;

  proxy &embedded;

  if (proxy.loading())
    node = embedded.node;
}

void xyaml_connection<std::filesystem::path>::operator()(
    xyaml_proxy &proxy, std::filesystem::path &path) const {
  if (proxy.loading()) {
    std::string path_str;
    proxy &path_str;
    path = path_str;
  } else {
    auto path_str = path.string();
    proxy &path_str;
  }
}

void xyaml_import::connect(xyaml_proxy &proxy) {
  if (proxy.loading()) {
    if (proxy["__import"]) {
      auto paths = proxy["__import"].as<std::vector<std::string>>();
      imports = {};
      for (auto const &path_str : paths) {
        imports.emplace_back(path_str);
      }

      overrides = xyaml_node::from_data(YAML::Node(), proxy.location);
      if (proxy["overrides"])
        proxy["overrides"] & overrides;

      merged = xyaml_node::from_data(YAML::Node(), proxy.location);
      for (auto const &import : imports)
        merged = merge_yaml(merged, import.node);
      merged = merge_yaml(merged, overrides);
    } else {
      proxy &merged;
    }
  }
}