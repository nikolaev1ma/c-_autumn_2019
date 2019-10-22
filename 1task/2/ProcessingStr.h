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
  ProcessingStr(string str_) : str(std::move(str_)), str_size(str.size()) {}

  ProcessingStr(int str_size_) : str_size(str_size_) {}

  ProcessingStr() {}

  // инициализация
  void PutPrefix(const vector<int> &prefix_) {
    prefix = prefix_;
    str_size = prefix.size();
  }

  void PutZ(const vector<int> &z_) {
    z = z_;
    str_size = prefix.size();
  }

  //вывод
  void CoutStr() { cout << "str:\n" << str << "\n"; }

  void CoutPrefix() {
    cout << "prefix:\n";
    for (int i = 0; i < str_size; ++i) {
      cout << prefix[i] << " ";
    }
    cout << "\n";
  }

  void CoutZ() {
    cout << "z:\n";
    for (int i = 0; i < str_size; ++i) {
      cout << z[i] << " ";
    }
    cout << "\n";
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
};
