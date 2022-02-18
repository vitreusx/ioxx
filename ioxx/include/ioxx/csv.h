#pragma once
#include <ioxx/convert.h>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

class csv_row;

class csv_header {
public:
  csv_header() = default;
  csv_header(csv_row const &row);
  csv_header(std::initializer_list<std::string> col_names);

  void insert(size_t idx, std::string const &name);

  size_t size() const;
  std::string const &operator[](size_t idx) const;
  size_t operator[](std::string const &s) const;

  friend std::ostream &operator<<(std::ostream &os, csv_header const &header);

  std::vector<std::string> const& col_names() const;

private:
  std::vector<std::string> idx_to_name;
  std::unordered_map<std::string, size_t> name_to_idx;
};

class free_schema;

enum class csv_access_mode { READING, WRITING };

class csv_cell {
public:
  explicit csv_cell(std::string &ref, csv_access_mode mode);

  template <typename T> T as() { return convert<T, std::string>(ref); }

  template <typename T> csv_cell &operator<<(T const &value) {
    ref = convert<std::string, T>(value);
    return *this;
  }

  template <typename T> csv_cell &operator>>(T &value) {
    value = convert<T, std::string>(ref);
    return *this;
  }

  template <typename T> csv_cell &operator&(T &value) {
    switch (mode) {
    case csv_access_mode::READING:
      return (*this >> value);
    case csv_access_mode::WRITING:
      return (*this << value);
    }
  }

private:
  std::string &ref;
  csv_access_mode mode;
};

class csv_row {
public:
  csv_row(csv_header const &header, csv_access_mode mode);
  csv_row(csv_header const &header, csv_access_mode mode,
          std::string const &line);

  csv_cell operator[](size_t idx);
  csv_cell operator[](std::string const &name);

  friend std::ostream &operator<<(std::ostream &os, csv_row const &row);
  csv_header const &header;

private:
  csv_access_mode mode;
  std::vector<std::string> raw_columns;

  friend class free_schema;
  friend class csv_header;
};

class free_schema {
public:
  std::vector<std::string> values;
  void connect(csv_row &row);
};

template <typename Schema = free_schema> class csv {
public:
  csv() = default;

  explicit csv(std::string const &source, bool load_header = true) {
    std::stringstream ss{};
    ss << source;
    load(ss, load_header);
  }

  explicit csv(std::istream &file, bool load_header = true) {
    load(file, load_header);
  }

  std::string save(bool save_header = true) const {
    std::stringstream ss{};
    save(ss, save_header);
    return ss.str();
  }

  std::ostream &save(std::ostream &os, bool save_header = true) const {
    bool first = true;

    if (save_header) {
      if (!first)
        os << '\n';
      os << header;
      first = false;
    }

    for (auto const &schema : rows) {
      csv_row row(header, csv_access_mode::WRITING);
      const_cast<Schema &>(schema).connect(row);

      if (!first)
        os << '\n';
      os << row;
      first = false;
    }

    return os;
  }

  csv_header header;
  std::vector<Schema> rows;

private:
  void load(std::istream &is, bool load_header) {
    rows.clear();
    for (std::string line; std::getline(is, line);) {
      auto row = csv_row(this->header, csv_access_mode::READING, line);
      if (load_header) {
        header = csv_header(row);
        load_header = false;
      } else {
        auto &schema = rows.emplace_back();
        schema.connect(row);
      }
    }
  }
};
