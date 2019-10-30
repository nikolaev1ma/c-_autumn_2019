#include "ProcessingStr.h"

int const size_alphabet = 26;

class GetStrByPrefix {
public:
  GetStrByPrefix(vector<int> prefix_) : prefix(std::move(prefix_)) {
    Recovery();  // находим для него строку
  }

  string Str() const { return str; }

private:
  vector<int> prefix;
  string str;

  // заполнение строки
  void Recovery() {
    str = 'a'; //первая буква вседа 'a'
    for (int i = 1; i < prefix.size(); ++i) {
      AddLetter(i); // заполняем побуквенно строку
    }
  }

  // добавляем букву на n место
  void AddLetter(int n) {
    // если префикс в этом знаении не 0, то буква однозначно востанавливается
    if (prefix[n] != 0) {
      str += str[prefix[n] - 1];
      return;
    }
    vector<bool> can_use_letter(
            size_alphabet); // can_use_letter[l] - может ли находиться буква
    // l на позиции n
    for (char letter = 'a'; letter <= 'z'; ++letter) {
      // изначально каждая буква может находиться на позиции n
      can_use_letter[letter - 'a'] = true;
    }
    // 'a' не может быть использованна, тк prefix == 0
    can_use_letter[0] = false;
    int tmp_prefix = prefix[n - 1];
    // прохдимся по префиксам, и узнаем, какие буквы мы не можем исплзвать
    while (tmp_prefix > 0) {
      can_use_letter[str[tmp_prefix] - 'a'] = false;
      tmp_prefix = prefix[tmp_prefix - 1];
    }
    // первую букву, которую мы можем использовать - используем
    for (char letter = 'b'; letter <= 'z'; ++letter) {
      if (can_use_letter[letter - 'a']) {
        str += letter;
        return;
      }
      // подходящей буквы нет
      str += '?';
    }
  }
};

void ProcessingStr::GetStrFromPrefix() {
  if (!initialized_str) {
    initialized_str = true;
    GetStrByPrefix auxiliary_class(
            std::move(prefix)); // создаем вспомогательный класс
    str = auxiliary_class.Str(); // присваиваем
  }
}
