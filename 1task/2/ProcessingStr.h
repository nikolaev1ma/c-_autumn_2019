#pragma once

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

using std::cin;
using std::cout;
using std::map;
using std::pair;
using std::set;
using std::string;
using std::vector;

class ProcessingStr {
public:
  ProcessingStr(string str_)
      : str(std::move(str_)), str_size(str.size()), initialized_str(true) {}

  ProcessingStr(int str_size_) : str_size(str_size_) {}

  ProcessingStr() {}

  // инициализация
  void PutPrefix(const vector<int> &prefix_) {
    prefix = prefix_;
    str_size = prefix.size();
    initialized_prefix = true;
  }

  void PutZ(const vector<int> &z_) {
    z = z_;
    str_size = prefix.size();
    initialized_z = true;
  }

  //вывод
  friend std::ostream &operator<<(std::ostream &out, const ProcessingStr &c) {
    out << "str:\n" << c.str << "\n";
    return out;
  }

  // str -> prefix
  void GetPrefixFromStr();

  // prefix -> str
  void GetStrFromPrefix();

  // str -> z
  void GetZFromStr();

  // z -> str
  void GetStrFromZ();

  // z -> prefix
  void GetPrefixFromZ();

  // prefix -> z
  void GetZFromPrefix();

private:
  string str;
  vector<int> prefix;
  vector<int> z;
  int str_size;

  bool initialized_prefix;
  bool initialized_z;
  bool initialized_str;
};
