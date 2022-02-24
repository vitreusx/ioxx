#include "xyaml_merge.h"
using namespace ioxx;

xyaml_node ioxx::merge(xyaml_node const &source, xyaml_node const &overrides) {
  if (!overrides.IsMap()) {
    if (overrides.IsNull())
      return source;
    else
      return overrides;
  } else if (!source.IsMap() || !source.size()) {
    return overrides;
  } else {
    auto copy_data = YAML::Node(YAML::NodeType::Map);
    auto copy = xyaml_node::from_data(copy_data, source.location);

    for (auto const &child : source) {
      if (child.first.IsScalar()) {
        auto key = child.first.Scalar();
        if (overrides[key]) {
          copy.portals[key] =
              std::make_shared<xyaml_node>(merge(source[key], overrides[key]));
        } else {
          copy[key] = source[key];
        }
      }
    }

    for (auto const &child : overrides) {
      if (child.first.IsScalar()) {
        auto key = child.first.Scalar();
        if (!copy[key]) {
          copy.portals[key] = std::make_shared<xyaml_node>(overrides[key]);
        }
      }
    }

    return copy;
  }
}