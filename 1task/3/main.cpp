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
  vector<int> way; //пройденный путь
  int parent;
  char letter_to_parent;
  int suff_link; //суффиксная ссылка
  int up;        // сжатая суффиксная ссылка
  bool is_leaf;
  vector<int> pattern_leaf_here; // множество шаблонов без маски, заканивающихся
                                 // в этой вершине
};

class String {
public:
  String(string str_, string temp_) {
    str = str_;
    temp = temp_;
    temp_arr.resize(1);
    pos_in_temp.resize(1);
    fill_temp_arr();
    init_bor();
    count_of_temps_in_this_position.resize(str.size());
  }

  //заполняет child, way
  void init_arr(int number) {
    bor[number].way.resize(size_alphabet);
    bor[number].child.resize(size_alphabet);
    for (int i = 0; i < size_alphabet; ++i) {
      bor[number].way[i] = -1;
      bor[number].child[i] = -1;
    }
  }

  //инициализация root в bor
  void init_bor() {
    bor.resize(1);
    bor[0].parent = -1;
    bor[0].suff_link = -1;
    sz_bor = 1;
    init_arr(0);
  }

  //разделение temp на меньшие осмысленные куски
  void fill_temp_arr() {
    int count_of_temps = 0;
    bool flag = false;
    for (int i = 0; i < temp.size(); ++i) {
      char letter = temp[i];
      if (letter != '?') {
        //начало нового шаблона без маски
        if (i && temp[i - 1] == '?' && flag) {
          ++count_of_temps;
          temp_arr.resize(count_of_temps + 1);
          pos_in_temp.resize(count_of_temps + 1);
        }
        flag = true;
        temp_arr[count_of_temps] += letter;
        pos_in_temp[count_of_temps] = i;
      }
    }
  }

  //заполнение бор
  void build_bor() {
    for (int i = 0; i < temp_arr.size(); ++i) {
      //добавление в бор текущий шаблон без маски
      add_in_bor(temp_arr[i], i);
    }
    for (int i = 0; i < bor.size(); ++i) {
      //проводим все суффиксные ссылки
      get_link(i);
    }
    for (int i = 0; i < bor.size(); ++i) {
      // проводим все краткие суффиксные ссылки
      get_up(i);
    }
  }

  //добавление в bor строки
  void add_in_bor(string inp_str, int number) {
    int cur = 0;
    for (int i = 0; i < inp_str.size(); ++i) {
      char cur_letter = inp_str[i] - 'a'; //добавляемая буква
      //если нет ребенка у текущей вершины, то создаем новую ноду
      if (bor[cur].child[cur_letter] == -1) {
        bor.resize(sz_bor + 1);
        init_arr(sz_bor);
        bor[sz_bor].suff_link = -1;
        bor[sz_bor].up = -1;
        bor[sz_bor].parent = cur;
        bor[sz_bor].letter_to_parent = cur_letter;
        bor[cur].child[cur_letter] = sz_bor;
        ++sz_bor;
      }
      //рекурсиво спускаемся к сыну
      cur = bor[cur].child[cur_letter];
    }
    bor[cur].is_leaf = true; //отмечаем, что финальная вершина термина
    bor[cur].pattern_leaf_here.push_back(
        number); //и запоминаем шаблон, который щаканчивается в этой вершине
  }

  //получение ссылки ver
  int get_link(int ver) {
    if (bor[ver].suff_link == -1) {
      if (ver == 0) {
        bor[ver].suff_link = 0;
        return 0;
      }
      if (bor[ver].parent == 0) {
        bor[ver].suff_link = 0;
        return 0;
      }
      //идем рекурсивно в отца
      bor[ver].suff_link =
          way(get_link(bor[ver].parent), bor[ver].letter_to_parent);
      return bor[ver].suff_link;
    }
    return bor[ver].suff_link;
  }

  //возвращает путь по бору из вершины ver по букве letter
  int way(int ver, char letter) {
    if (bor[ver].way[letter] == -1) {
      //если есть сын к которому есть путь по букве letter, то по нему и идем
      if (bor[ver].child[letter] != -1) {
        bor[ver].way[letter] = bor[ver].child[letter];
      } else {
        if (ver == 0) {
          bor[ver].way[letter] = 0;
        } else {
          //рекурсивно делаем действия с суффиксной ссылкой
          bor[ver].way[letter] = way(get_link(ver), letter);
        }
      }
    }
    return bor[ver].way[letter];
  }

  //взвращает краткую суффикную ссылку ver
  int get_up(int ver) {
    if (bor[ver].up == -1) {
      int suf_parent = bor[ver].suff_link;
      //если суффиксная ссылка - терминал, то эта же ссылка - краткая
      if (bor[suf_parent].is_leaf) {
        bor[ver].up = bor[ver].suff_link;
        return bor[ver].up;
      }
      if (suf_parent == 0) {
        bor[ver].up = 0;
        return 0;
      }
      //рекурсивно ищем через суффиксную ссылку
      bor[ver].up = get_up(suf_parent);
      return bor[ver].up;
    }
    return bor[ver].up;
  }

  //обработка текста
  void process_text() {
    int cur = 0; //место, где мы находимся в bor
    for (int i = 0; i < str.size(); ++i) {
      char letter = str[i] - 'a'; //текущая обрабатываемая буква
      cur = way(cur, letter); //продвигаемся по бору
      int search_temp = cur;
      //идем по кратким суффиксным ссылкам до корня и запоминаем шаблоны без
      //масок, которые заканчиваются в этой вершине
      while (search_temp != 0) {
        vector<int> tmp =
            bor[search_temp].pattern_leaf_here; //массив всех шаблонов,
                                                //заканчивающихся в search_temp
        for (int j = 0; j < tmp.size(); ++j) {
          int cur_temp = tmp[j];
          int len_cur_temp =
              pos_in_temp[cur_temp]; // позиция данного шаблона без маски
          if (i - len_cur_temp >= 0) {
            // в позиции i - len_cur_temp строки начинается данный шаблон
            count_of_temps_in_this_position[i - len_cur_temp] += 1;
          }
        }
        search_temp = bor[search_temp].up;
      }
    }
  }

  //вывод результата
  void result() {
    for (int i = 0; i < std::min(str.size(), str.size() - temp.size() + 1);
         ++i) {
      //если в i позиции строки начинаются все шаблоны без маски, то здесь
      //начинаются и наш шаблон
      if (count_of_temps_in_this_position[i] == temp_arr.size()) {
        cout << i << " ";
      }
    }
  }

private:
  string str;              //строка
  string temp;             //шаблон
  vector<string> temp_arr; //массив шаблонов без маски
  vector<int> pos_in_temp; // позиции шаблонов без маски в шаблоне
  vector<int>
      count_of_temps_in_this_position; // колво всех шаблонов без масок, которые
                                       // могут начаться в этой месте строки
  vector<Node> bor;
  int sz_bor;
};

int main() {
  string inp_template; //шаблон
  cin >> inp_template;
  string inp_str; //строка
  cin >> inp_str;
  String my_class(inp_str, inp_template);
  my_class.build_bor();    //построение бора
  my_class.process_text(); // поиск шаблонов в строке
  my_class.result();       //вывод
  return 0;
}
