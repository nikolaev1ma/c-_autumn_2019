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
  ProcessingStr(string str_) {
    str = str_ + '#'; //сразу добавляем в конец строки знак #
    last_prefix = 0; // prefix[sz] = 0, тк str[sz] = '#', именно этот префикс мы
                     // и запоминаем изначально
    sz = str_.size();
    counter = str.size();
  }

  //вычисление префикс функции строки
  void prefix_function() {
    prefix.resize(sz);
    for (int i = 1; i < sz; ++i) {
      //если str[prefix[i - 1]] = str[i], то очевидно что prefix[i] = prefix[i -
      //1] + 1 если же нет, то просто рекурсивно спускаемся
      int j = prefix[i - 1];
      while (j > 0 && str[i] != str[j]) {
        j = prefix[j - 1];
      }
      if (str[i] == str[j]) {
        ++j;
      }
      // если условие не выполнено, то prefix[i] = 0
      prefix[i] = j;
    }
  }

  //вычисляем префикс на новой позиции, на которой стоит letter
  int prefix_function_in_new_pos(char letter) {
    int temp_pref = last_prefix;
    //делаем то же самое, что и в prefix_function, только заметим, что для
    //вычисления нам понадобятся только prefix[0 .. sz - 1]
    while (temp_pref > 0 && letter != str[temp_pref]) {
      temp_pref = prefix[temp_pref - 1];
    }

    if (letter == str[temp_pref]) {
      ++temp_pref;
    }
    last_prefix =
        temp_pref; // запоминает новый префикс, для вычисление следующего
    return last_prefix;
  }

  //вычисляем, добавился ли нам новый шаблон, при добавлении новой буквы letter,
  //если добавился, выводим его позицию
  void find_new_temp_pos(char letter) {
    // шаблон добавится тогда и только тогда, когда новый префикс будет
    // равняться длине шаблона. Данный префикс не может быть больше sz, из-за
    // символа '#'
    if (prefix_function_in_new_pos(letter) == sz) {
      //вычитаем дважды длину строки, тк нам нужно начало шаблона, а не конец, и
      //counter начинается с sz
      cout << counter - 2 * sz << " ";
    }
    ++counter;
  }

private:
  string str; // обрабатываемая строка, в данном случае это шаблон
  vector<int> prefix; //префикс функция шаблона
  int last_prefix; // при добавлении буквы в строку новый префикс зависит от
                   // предыдущего префикса, поэтому будем его хранить
  int sz;          //длина шаблона
  int counter; //счетчик по текущему состоянию строки
};

int main() {
  string temp; //шаблон
  cin >> temp;
  ProcessingStr my_class(temp);
  my_class.prefix_function(); //считаем префикс от шаблона с # на конце
  cin.tie(nullptr);
  char letter;
  //побуквенно считываем строку , в которой будем искать шаблон
  while (cin >> letter) {
    my_class.find_new_temp_pos(
        letter); // для каждой новой буквы проверям наличие шаблона
  }
  return 0;
}