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
  ProcessingStr(string &str_);

  //вычисление префикс функции строки
  void PrefixFunction();

  //вычисляем префикс на новой позиции, на которой стоит letter
  int PrefixFunctionInNewPosition(char letter) const;

  //вычисляем, добавился ли нам новый шаблон, при добавлении новой буквы letter,
  //если добавился, выводим его позицию
  void FindNextTemp(char letter);

private:
  string str; // обрабатываемая строка, в данном случае это шаблон
  vector<int> prefix; //префикс функция шаблона
  int penultimate_prefix; // при добавлении буквы в строку новый префикс зависит
                          // от предыдущего префикса, поэтому будем его хранить
  int size_of_str; //длина шаблона
  int position_in_added_str; //счетчик по текущему состоянию строки
};

ProcessingStr::ProcessingStr(string &str_)
    : str(std::move(str_) + '#'), penultimate_prefix(0),
      size_of_str(str.size() - 1), position_in_added_str(1) {}

void ProcessingStr::PrefixFunction() {
  prefix.reserve(size_of_str);
  prefix.emplace_back(0);
  for (int i = 1; i < size_of_str; ++i) {
    //если str[prefix[i - 1]] = str[i], то очевидно что
    // prefix[i] = prefix[i - 1] + 1 если же нет, то просто рекурсивно
    // спускаемся
    int tmp_prefix = prefix[i - 1];
    while (tmp_prefix > 0 && str[i] != str[tmp_prefix]) {
      tmp_prefix = prefix[tmp_prefix - 1];
    }
    if (str[i] == str[tmp_prefix]) {
      ++tmp_prefix;
    }
    // если условие не выполнено, то prefix[i] = 0
    prefix.emplace_back(tmp_prefix);
  }
}

int ProcessingStr::PrefixFunctionInNewPosition(char letter) const {
  int tmp_prefix = penultimate_prefix;
  //делаем то же самое, что и в prefix_function, только заметим, что для
  //вычисления нам понадобятся только prefix[0 .. size_of_str - 1]
  while (tmp_prefix > 0 && letter != str[tmp_prefix]) {
    tmp_prefix = prefix[tmp_prefix - 1];
  }
  if (letter == str[tmp_prefix]) {
    ++tmp_prefix;
  }
  return tmp_prefix;
}

void ProcessingStr::FindNextTemp(char letter) {
  // шаблон добавится тогда и только тогда, когда новый префикс будет
  // равняться длине шаблона. Данный префикс не может быть больше size_of_str,
  // из-за символа '#'
  int new_prefix = PrefixFunctionInNewPosition(letter);
  if (new_prefix == size_of_str) {
    //вычитаем длину строки, тк нам нужно начало шаблона, а не конец
    cout << position_in_added_str - size_of_str << " ";
  }
  penultimate_prefix = new_prefix;
  ++position_in_added_str;
}

int main() {
  string temp; //шаблон
  cin >> temp;
  ProcessingStr my_class(temp);
  my_class.PrefixFunction(); //считаем префикс от шаблона с # на конце
  cin.tie(nullptr);
  char letter;
  string str;
  cin >> str;
  for (auto letter : str) {
    my_class.FindNextTemp(
        letter); // для каждой новой буквы проверям наличие шаблона
  }
  return 0;
}
