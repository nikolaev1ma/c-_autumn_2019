#ifndef C__AUTUMN_2019_STRING_H
#define C__AUTUMN_2019_STRING_H

#include <iostream>
#include <vector>
#include <string>
#include <map>


using std::vector;
using std::string;
using std::cin;
using std::cout;
using std::map;

class String {
public:

    String(string str_) {
        str = str_;
        str_size = str.size();
    }

    String(string &&str_) {
        str = std::move(str_);
        str_size = str.size();
    }

    String(int str_size_) {
        str_size = str_size_;
    }

    String() {}

    void get_prefix(const vector<int> &prefix_ ) {
        prefix = prefix_;
        str_size = prefix.size();
    }

    void get_z(const vector<int> &z_) {
        z = z_;
        str_size = prefix.size();
    }

    void cout_str() {
        cout << "str:\n";
        cout << str;
        cout << "\n";
    }

    void cout_prefix() {
        cout << "prefix:\n";
        for(int i = 0; i < sz; ++i) {
            cout << prefix[i] << " ";
        }
        cout << "\n";
    }

    void cout_z() {
        cout << "z:\n";
        for(int i = 0; i < sz; ++i) {
            cout << z[i] << " ";
        }
        cout << "\n";
    }

    //str -> prefix
    void get_prefix_by_str();
    //prefix -> str
    void get_str_by_prefix();
    //str -> z
    void get_z_by_str();
    //z -> str
    void get_str_by_z();
    //z -> prefix
    void get_prefix_by_z();
    // prefix -> z
    void get_z_by_prefix();

private:
    string str;
    vector<int> prefix;
    vector<int> z;
    int str_size;
};

#endif //C__AUTUMN_2019_STRING_H
