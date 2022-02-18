#include <fstream>
#include <iostream>
#include <ioxx/csv.h>

struct entry {
  std::string type;
  std::unordered_map<std::string, float> distances;

  void connect(csv_row &row) {
    row["type"] & type;
    for (auto const &col_name : row.header.col_names()) {
      if (col_name != "type")
        row[col_name] & distances[col_name];
    }
  }
};

int main() {
  std::ifstream example("data/example.csv");
  csv<entry> csv_file(example);
  return 0;
}