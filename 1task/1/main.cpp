// ссылка на контест:
// https://contest.yandex.ru/contest/13875/standings/

#include <iostream>
#include <string>
#include <vector>

using std::cin;
using std::cout;
using std::string;
using std::vector;

class ProcessingStr {
public:
  //сразу добавляем в конец строки знак #
  // prefix[size_of_str] = 0, тк str[size_of_str] = '#', именно этот префикс мы
  // и запоминаем изначально
  ProcessingStr(string str_)
      : str(str_ + '#'), penultimate_prefix(0), size_of_str(str_.size()),
        position_in_added_str(1) {}

  ProcessingStr(string &&str_)
      : str(std::move(str_) + '#'), penultimate_prefix(0),
        size_of_str(std::move(str_).size()), position_in_added_str(1) {}

  //вычисление префикс функции строки
  void prefix_function() {
    prefix.resize(size_of_str);
    for (int i = 1; i < size_of_str; ++i) {
      //если str[prefix[i - 1]] = str[i], то очевидно что prefix[i] = prefix[i -
      //1] + 1 если же нет, то просто рекурсивно спускаемся
      int temp_pref = prefix[i - 1];
      while (temp_pref > 0 && str[i] != str[temp_pref]) {
        temp_pref = prefix[temp_pref - 1];
      }
      if (str[i] == str[temp_pref]) {
        ++temp_pref;
      }
      // если условие не выполнено, то prefix[i] = 0
      prefix[i] = temp_pref;
    }
  }

  //вычисляем префикс на новой позиции, на которой стоит letter
  int prefix_function_in_new_position(char letter) const {
    int temp_pref = penultimate_prefix;
    //делаем то же самое, что и в prefix_function, только заметим, что для
    //вычисления нам понадобятся только prefix[0 .. size_of_str - 1]
    while (temp_pref > 0 && letter != str[temp_pref]) {
      temp_pref = prefix[temp_pref - 1];
    }
    if (letter == str[temp_pref]) {
      ++temp_pref;
    }
    return temp_pref;
  }

  //вычисляем, добавился ли нам новый шаблон, при добавлении новой буквы letter,
  //если добавился, выводим его позицию
  void find_new_temp_in_new_position(char letter) {
    // шаблон добавится тогда и только тогда, когда новый префикс будет
    // равняться длине шаблона. Данный префикс не может быть больше size_of_str,
    // из-за символа '#'
    int new_prefix = prefix_function_in_new_position(letter);
    if (new_prefix == size_of_str) {
      //вычитаем длину строки, тк нам нужно начало шаблона, а не конец
      cout << position_in_added_str - size_of_str << " ";
    }
    penultimate_prefix = new_prefix;
    ++position_in_added_str;
  }

private:
  string str; // обрабатываемая строка, в данном случае это шаблон
  vector<int> prefix; //префикс функция шаблона
  int penultimate_prefix; // при добавлении буквы в строку новый префикс зависит
                          // от предыдущего префикса, поэтому будем его хранить
  int size_of_str; //длина шаблона
  int position_in_added_str; //счетчик по текущему состоянию строки
};

int main() {
  string temp; //шаблон
  cin >> temp;
  ProcessingStr my_class(temp);
  my_class.prefix_function(); //считаем префикс от шаблона с # на конце
  cin.tie(nullptr);
  char letter;
  string str;
  cin >> str;
  for (int i = 0; i < str.size(); ++i) {
    my_class.find_new_temp_in_new_position(
        str[i]); // для каждой новой буквы проверям наличие шаблона
  }
  return 0;
}
