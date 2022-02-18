#include "csv.h"
#include "read_csv_row.h"

csv_header::csv_header(std::initializer_list<std::string> col_names) {
  size_t col_idx = 0;
  for (auto const &col_name : col_names)
    insert(col_idx++, col_name);
}

csv_header::csv_header(const csv_row &row) {
  size_t col_idx = 0;
  for (auto const &col_name : row.raw_columns)
    insert(col_idx++, col_name);
}

void csv_header::insert(size_t idx, std::string const &name) {
  if (idx + 1 > idx_to_name.size())
    idx_to_name.resize(idx + 1);

  idx_to_name[idx] = name;
  name_to_idx[name] = idx;
}

size_t csv_header::size() const { return idx_to_name.size(); }

std::string const &csv_header::operator[](size_t idx) const {
  return idx_to_name[idx];
}

size_t csv_header::operator[](std::string const &s) const {
  return name_to_idx.at(s);
}

std::ostream &operator<<(std::ostream &os, csv_header const &header) {
  for (size_t idx = 0; idx < header.idx_to_name.size(); ++idx) {
    if (idx > 0)
      os << ',';
    os << header.idx_to_name[idx];
  }
  return os;
}
std::vector<std::string> const &csv_header::col_names() const {
  return idx_to_name;
}

csv_cell::csv_cell(std::string &ref, csv_access_mode mode)
    : ref{ref}, mode{mode} {};

csv_row::csv_row(csv_header const &header, csv_access_mode mode)
    : header{header}, mode{mode} {
  raw_columns = std::vector<std::string>(header.size());
}

csv_row::csv_row(csv_header const &header, csv_access_mode mode,
                 const std::string &line)
    : header{header}, mode{mode} {
  raw_columns = readCSVRow(line);
}

csv_cell csv_row::operator[](size_t idx) {
  return csv_cell(raw_columns[idx], mode);
}

csv_cell csv_row::operator[](const std::string &name) {
  return (*this)[header[name]];
}

std::ostream &operator<<(std::ostream &os, csv_row const &row) {
  for (size_t idx = 0; idx < row.raw_columns.size(); ++idx) {
    if (idx > 0)
      os << ',';
    os << row.raw_columns[idx];
  }
  return os;
}

void free_schema::connect(csv_row &row) {
  switch (row.mode) {
  case csv_access_mode::READING:
    values = row.raw_columns;
    break;
  case csv_access_mode::WRITING:
    row.raw_columns = values;
    break;
  }
}
