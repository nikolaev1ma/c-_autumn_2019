//ссылка на контест:
// https://contest.yandex.ru/contest/13875/standings/

#include "GetPrefixFromZ.cpp"
#include "GetZFromPrefix.cpp"

// prefix -> str
int main() {
  int input;
  vector<int> prefix;
  while (cin >> input) {
    prefix.emplace_back(input);
  }
  ProcessingStr my_class;
  my_class.PutPrefix(std::move(prefix)); //инициализация префикса
  my_class.GetStrFromPrefix(); // получаем строку по префиксу
  cout << my_class;            //вывод
}
