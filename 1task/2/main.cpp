//ссылка на контест:
// https://contest.yandex.ru/contest/13875/standings/

#include "GetPrefixByZ.cpp"
#include "GetZByPrefix.cpp"

// prefix -> str
int main() {
  int input;
  vector<int> prefix;
  while (cin >> input) {
    if (input == '\n') {
      break;
    }
    prefix.push_back(input);
  }
  String my_class;
  my_class.get_prefix(prefix); //инициализация префикса
  my_class.get_str_by_prefix(); // получаем строку по префиксу
  my_class.cout_str();          //вывод
}
