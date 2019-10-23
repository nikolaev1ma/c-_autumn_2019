// ссылка на контест:
// https://contest.yandex.ru/contest/13875/standings/

#include <iostream>
#include <string>
#include <vector>

using std::cin;
using std::cout;
using std::string;
using std::vector;

int const size_alphabet = 26;

struct Node {
  vector<int> child;
  vector<int> way; // пройденный путь
  int parent;
  char letter_to_parent;
  int suff_link; // суффиксная ссылка
  int up;        // сжатая суффиксная ссылка
  bool is_leaf;
  vector<int> pattern_leaf_here; // множество шаблонов без маски, заканивающихся
                                 // в этой вершине
};

class ProcessingString {
public:
  ProcessingString(string str_, string temp_);

  // храним здесь всю логику
  void Implementation();

private:
  string str;              // строка
  string temp;             // шаблон
  vector<string> temp_arr; // массив шаблонов без маски
  vector<int> pos_in_temp; // позиции шаблонов без маски в шаблоне
  vector<int>
      count_of_temps_in_this_position; // колво всех шаблонов без масок, которые
                                       // могут начаться в этой месте строки
  vector<Node> bor;
  int sz_bor;

  // вывод результата
  void Result();

  // обработка текста
  void ProcessText();

  // заполнение bor
  void BuildBor();

  // инициализация root в bor
  void InitBor();

  // разделение temp на меньшие осмысленные куски
  void FillTempArr();

  // добавление в bor строки
  void AddInBor(int number);

  // заполняет child, way
  void InitArr(int number);

  // получение ссылки vertex
  int GetLink(int vertex);

  // возвращает путь по бору из вершины vertex по букве letter
  int Way(int vertex, char letter);

  // взвращает краткую суффикную ссылку vertex
  const int GetUp(int vertex);
};

ProcessingString::ProcessingString(string str_, string temp_)
    : str(std::move(str_)), temp(std::move(temp_)) {
  temp_arr.resize(1);
  pos_in_temp.resize(1);
  FillTempArr();
  InitBor();
  count_of_temps_in_this_position.resize(str.size());
}

void ProcessingString::ProcessText() {
  int cur = 0; // место, где мы находимся в bor
  for (int i = 0; i < str.size(); ++i) {
    char letter = str[i] - 'a'; // текущая обрабатываемая буква
    cur = Way(cur, letter); // продвигаемся по бору
    int search_temp = cur;
    // идем по кратким суффиксным ссылкам до корня и запоминаем шаблоны без
    // масок, которые заканчиваются в этой вершине
    while (search_temp != 0) {
      vector<int> tmp =
          bor[search_temp].pattern_leaf_here; // массив всех шаблонов,
      // заканчивающихся в search_temp
      for (int j = 0; j < tmp.size(); ++j) {
        int cur_temp = tmp[j];
        int len_cur_temp =
            pos_in_temp[cur_temp]; // позиция данного шаблона без маски
        if (i - len_cur_temp >= 0) {
          // в позиции i - len_cur_temp строки начинается данный шаблон
          ++count_of_temps_in_this_position[i - len_cur_temp];
        }
      }
      search_temp = bor[search_temp].up;
    }
  }
}

void ProcessingString::Result() {
  for (int i = 0; i < std::min(str.size(), str.size() - temp.size() + 1); ++i) {
    // если в i позиции строки начинаются все шаблоны без маски, то здесь
    // начинаются и наш шаблон
    if (count_of_temps_in_this_position[i] == temp_arr.size()) {
      cout << i << " ";
    }
  }
  cout << std::endl;
}

void ProcessingString::BuildBor() {
  for (int i = 0; i < temp_arr.size(); ++i) {
    // добавление в бор текущий шаблон без маски
    AddInBor(i);
  }
  for (int i = 0; i < bor.size(); ++i) {
    // проводим все суффиксные ссылки
    GetLink(i);
  }
  for (int i = 0; i < bor.size(); ++i) {
    // проводим все краткие суффиксные ссылки
    GetUp(i);
  }
}

void ProcessingString::Implementation() {
  BuildBor();    // построение бора
  ProcessText(); // поиск шаблонов в строке
  Result();      // вывод
}

void ProcessingString::InitBor() {
  bor.resize(1);
  bor[0].parent = -1;
  bor[0].suff_link = -1;
  sz_bor = 1;
  InitArr(0);
}

void ProcessingString::FillTempArr() {
  int count_of_temps = 0;
  bool valid_letter_appeared =
      false; // проверка на то, что мы уже прошли валижную букву
  for (int i = 0; i < temp.size(); ++i) {
    char letter = temp[i];
    if (letter != '?') {
      // начало нового шаблона без маски
      if (i && temp[i - 1] == '?' && valid_letter_appeared) {
        ++count_of_temps;
        temp_arr.resize(count_of_temps + 1);
        pos_in_temp.resize(count_of_temps + 1);
      }
      valid_letter_appeared = true;
      temp_arr[count_of_temps] += letter;
      pos_in_temp[count_of_temps] = i;
    }
  }
}

void ProcessingString::InitArr(int number) {
  bor[number].way.resize(size_alphabet);
  bor[number].child.resize(size_alphabet);
  for (int i = 0; i < size_alphabet; ++i) {
    bor[number].way[i] = -1;
    bor[number].child[i] = -1;
  }
}

void ProcessingString::AddInBor(int number) {
  string tmp_string = temp_arr[number];
  int cur = 0;
  for (int i = 0; i < tmp_string.size(); ++i) {
    char cur_letter = tmp_string[i] - 'a'; // добавляемая буква
    // если нет ребенка у текущей вершины, то создаем новую ноду
    if (bor[cur].child[cur_letter] == -1) {
      bor.resize(sz_bor + 1);
      InitArr(sz_bor);
      bor[sz_bor].suff_link = -1;
      bor[sz_bor].up = -1;
      bor[sz_bor].parent = cur;
      bor[sz_bor].letter_to_parent = cur_letter;
      bor[cur].child[cur_letter] = sz_bor;
      ++sz_bor;
    }
    // рекурсиво спускаемся к сыну
    cur = bor[cur].child[cur_letter];
  }
  bor[cur].is_leaf = true; // отмечаем, что финальная вершина терминальная
  bor[cur].pattern_leaf_here.push_back(
      number); // и запоминаем шаблон, который заканчивается в этой вершине
}

int ProcessingString::GetLink(int vertex) {
  if (bor[vertex].suff_link == -1) {
    if (vertex == 0) {
      bor[vertex].suff_link = 0;
      return 0;
    }
    if (bor[vertex].parent == 0) {
      bor[vertex].suff_link = 0;
      return 0;
    }
    // идем рекурсивно в отца
    bor[vertex].suff_link =
        Way(GetLink(bor[vertex].parent), bor[vertex].letter_to_parent);
    return bor[vertex].suff_link;
  }
  return bor[vertex].suff_link;
}

int ProcessingString::Way(int vertex, char letter) {
  if (bor[vertex].way[letter] == -1) {
    // если есть сын к которому есть путь по букве letter, то по нему и идем
    if (bor[vertex].child[letter] != -1) {
      bor[vertex].way[letter] = bor[vertex].child[letter];
    } else if (vertex == 0) {
      bor[vertex].way[letter] = 0;
    } else {
      // рекурсивно делаем действия с суффиксной ссылкой
      bor[vertex].way[letter] = Way(GetLink(vertex), letter);
    }
  }
  return bor[vertex].way[letter];
}

const int ProcessingString::GetUp(int vertex) {
  if (bor[vertex].up == -1) {
    int suf_parent = bor[vertex].suff_link;
    //если суффиксная ссылка - терминал, то эта же ссылка - краткая
    if (bor[suf_parent].is_leaf) {
      bor[vertex].up = bor[vertex].suff_link;
    } else if (suf_parent == 0) {
      bor[vertex].up = 0;
    } else {
      // рекурсивно ищем через суффиксную ссылку
      bor[vertex].up = GetUp(suf_parent);
    }
    return bor[vertex].up;
  }
}

int main() {
  string inp_template; // шаблон
  cin >> inp_template;
  string inp_str; // строка
  cin >> inp_str;
  ProcessingString my_class(std::move(inp_str), std::move(inp_template));
  my_class.Implementation();
  return 0;
}
