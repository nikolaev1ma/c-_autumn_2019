#include "String.h"

typedef std::map<char, bool> mapPrefixT;

class GetStrByPrefix {
public:
  GetStrByPrefix(const vector<int> prefix_) { prefix = prefix_; }

  //добавляем букву на n место
  void AddLetter(int n) {
    //если префикс в этом знаении не 0, то буква однозначно востанавливается
    if (prefix[n] != 0) {
      str += str[prefix[n] - 1];
      return;
    }
    mapPrefixT can_use_letter; // can_use_letter[l] - может ли находиться буква
                               // l на позиции n
    for (char a = 'a'; a <= 'z'; ++a) {
      //изначально каждая буква может находиться на позиции n
      can_use_letter[a] = true;
    }
    //'a' не может быть использованна, тк prefix == 0
    can_use_letter['a'] = false;
    int temp_pref = prefix[n - 1];
    // прохдимся по префиксам, и узнаем, какие буквы мы не можем исплзвать
    while (temp_pref > 0) {
      can_use_letter[str[temp_pref]] = false;
      temp_pref = prefix[temp_pref - 1];
    }
    // первую букву, которую мы можем использовать - используем
    for (char a = 'b'; a <= 'z'; ++a) {
      if (can_use_letter[a] == true) {
        str += a;
        return;
      }
    }
  }

  //заполнение строки
  void Recovery() {
    str = 'a'; //первая буква вседа 'a'
    for (int i = 1; i < prefix.size(); ++i) {
      AddLetter(i); // заполняем побуквенно строку
    }
  }

  string Str() { return str; }

private:
  vector<int> prefix;
  string str;
};

void String::get_str_by_prefix() {
  GetStrByPrefix auxiliary_class(prefix); //создаем вспомоательный класс
  auxiliary_class.Recovery();  //находим для него строку
  str = auxiliary_class.Str(); //присваиваем
}