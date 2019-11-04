// ссылка на контест:
// https://contest.yandex.ru/contest/14570/run-report/23439957/

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::cin;
using std::cout;
using std::string;
using std::vector;

const int size_alphabet = 26;

// вершина в суффиксном дереве
struct Node {
  int parent;     // родитель
  int str_number; // номер строки, который соответствует ребру из node в parent
  int left_border_edge; // начало подстроки соответствующей ребру из node в
                        // parent
  int right_border_edge; // конец подстроки соответствующей ребру из node в
                         // parent
  int link;              // ссылка
  vector<int>
      child; // массив детей. child[i] соответствует ребенку, до которого есть
             // путь по букве i + 'a', ну или по '$' или по '#'

  Node(int parent_ = -1, int left_ = 0, int right_ = 0)
      : parent(parent_), str_number(0), left_border_edge(left_),
        right_border_edge(right_), link(-1),
        child(vector<int>(size_alphabet + 2)) {
    for (char letter = 'a'; letter <= 'z'; ++letter) {
      child[letter - 'a' + 2] = -1;
    }
    // '#' имеет самый большой приортитет
    child[0] = -1;
    // '$' имеет самый большой приортитет после '#'
    child[1] = -1;
  }

  // длина ребра
  int LenEdge() const { return right_border_edge - left_border_edge; }

  // получение ребенка по char
  int GetChild(char letter) const {
    if (letter == '#') {
      return child[0];
    }
    if (letter == '$') {
      return child[1];
    }
    return child[letter - 'a' + 2];
  }

  // изменение реберка по букве letter
  void ChangeChild(char letter, int child_) {
    if (letter == '#') {
      child[0] = child_;
      return;
    }
    if (letter == '$') {
      child[1] = child_;
      return;
    }
    child[letter - 'a' + 2] = child_;
  }
};

// промежуточная вершина в дереве, может лежать внутри ребра
struct InternalVertex {
  int vertex; // вершина к которой internal_vertex прикреплен
  int pos_in_edge; // глубина внутри ребра

  InternalVertex(int vertex_, int pos_) : vertex(vertex_), pos_in_edge(pos_) {}
};

class SuffixTree {
public:
  SuffixTree(string a, string b,
             int a_size_); // храним длину строки а, чтобы могли через mode
                           // делать инициализацию

  void BuildTree(); // построение дерева

  string Result();

private:
  int vertex_count;           // колво вершин
  string str;                 // объеденение 2 строк
  int a_str_size;             // длина 1 строки
  int str_size;               // длина объедениения строк
  InternalVertex current_pos; // текущая вершина
  vector<Node> node_arr;      // массив вершин графа

  vector<int> dfs_sort; // порядок обхода вершин в графе при финальном dfs
  int dfs_counter;      // счетчик к поличеству вершин
  std::stringstream
      tmp_result_str; // строка в которую будет записываться информация о ребрах

  void AddInTree(int new_letter); // добавляем в дерево все подстроки,
                                  // заканчивающие в new_letter

  InternalVertex
  Go(InternalVertex position, int left,
     int right); // идем из position по дереву по подстроке str[left ... right]

  int SuffLink(int vertex); // возвращаем ссылку на vertex

  int Share(InternalVertex pos); // разделяем при надобности ветки из position

  void DFS(int dfs_counter); // проход по вершинам дерева для вывода
};

SuffixTree::SuffixTree(string a, string b, int a_size_)
    : a_str_size(a_size_), str(std::move(a) + std::move(b)),
      str_size(str.size()), current_pos(InternalVertex(0, 0)), vertex_count(1),
      dfs_counter(0) {
  node_arr.emplace_back();
}

void SuffixTree::BuildTree() {
  // строим дерево для каждого префикса строки последовательно
  for (int i = 0; i < str_size; ++i) {
    AddInTree(i); // обновляем дерево
  }
}

// идем dfs по дереву, пока мы не наткнулись на символ '$'. Тогда обрезаем конец
// дерева. Таким образом у нас получается дерево в котором содержутся только
// суффиксы отдельно строки a и строки b. склеиных строк не будет, тк мы
// обрезаем конец ветки, как толко находим '$'
string SuffixTree::Result() {
  dfs_sort.resize(vertex_count);
  DFS(0); // dfs из вершины дерева
  string result;
  std::stringstream ver_count_str;
  ver_count_str << dfs_counter + 1
                << '\n'; // заметим, что dfs_counter + 1 это и есть колво вершин
                         // в нашем дереве
  result = ver_count_str.str() + tmp_result_str.str(); // объяденяем два вывода
  return result;
}

// записываем информацию по поводу вершины по индексу dfs_point и продолжаем dfs
// при необходимости
void SuffixTree::DFS(int dfs_point) {
  Node vertex = node_arr[dfs_point];
  for (int i = 0; i < size_alphabet + 2; ++i) {
    // проходимся по детям
    if (vertex.child[i] != -1) {
      ++dfs_counter;
      dfs_sort[vertex.child[i]] =
          dfs_counter; // добавляем в отсортированный массив node
      Node child_vertex = node_arr[vertex.child[i]]; // ребенок
      child_vertex.parent = dfs_sort[child_vertex.parent]; // меняем родителя
      bool reach_$ = child_vertex.left_border_edge < a_str_size &&
                     child_vertex.right_border_edge >=
                         a_str_size; // флаг того, что мы дошли до $
      if (reach_$) {
        // обрезаем
        child_vertex.right_border_edge = a_str_size;
        child_vertex.str_number = 0;
      }
      // проверка на то, что ребро из слова b
      if (child_vertex.left_border_edge >= a_str_size) {
        child_vertex.left_border_edge -= a_str_size;
        child_vertex.right_border_edge -= a_str_size;
        child_vertex.str_number = 1;
      }
      // запись информации
      tmp_result_str << child_vertex.parent << " " << child_vertex.str_number
                     << " " << child_vertex.left_border_edge << " "
                     << child_vertex.right_border_edge << '\n';
      if (!reach_$) {
        // продолжаем dfs
        DFS(vertex.child[i]);
      }
    }
  }
}

void SuffixTree::AddInTree(int new_letter) {
  while (true) {
    InternalVertex next_pos =
        Go(current_pos, new_letter,
           new_letter + 1); // переходим в следующую вершину
    // если можем перейти, то просто выходим
    if (next_pos.vertex != -1) {
      current_pos = next_pos;
      return;
    }
    int split_edge_node = Share(current_pos);
    ++vertex_count;
    node_arr.push_back(Node(split_edge_node, new_letter,
                            str_size)); // добавляем новую ветку для новой буквы
    node_arr[split_edge_node].ChangeChild(str[new_letter], vertex_count - 1);
    // если мы уже оказались в root то выходим
    if (split_edge_node == 0) {
      return;
    }
    // переходим по суффиксной ссылки
    current_pos.vertex = SuffLink(split_edge_node);
    current_pos.pos_in_edge = node_arr[current_pos.vertex].LenEdge();
  }
}

// делим ребро по position при необходимости на два ребра поменьше
int SuffixTree::Share(InternalVertex position) {
  int vertex = position.vertex;
  int pos_in_edge = position.pos_in_edge;
  // условие того, что position это конец ребра - тоесть вершина в дереве
  if (pos_in_edge == node_arr[vertex].LenEdge()) {
    return vertex;
  }
  // условие того, что position это начало ребра - тоесть вершина в дереве
  if (pos_in_edge == 0) {
    return node_arr[vertex].parent;
  }
  Node basic_node = node_arr[vertex];
  ++vertex_count;
  node_arr.push_back(Node(basic_node.parent, basic_node.left_border_edge,
                          basic_node.left_border_edge +
                              pos_in_edge)); //добавляем вершину посреди ребра
  node_arr[basic_node.parent].ChangeChild(
      str[basic_node.left_border_edge],
      vertex_count - 1); // изменяем ссылку на ребенка у отца basic_node
  node_arr[vertex_count - 1].ChangeChild(
      str[basic_node.left_border_edge + pos_in_edge],
      vertex); // делаем ребенком у новой вершины basic_node
  node_arr[vertex].parent = vertex_count - 1; // меняем родителя
  node_arr[vertex].left_border_edge += pos_in_edge;
  return vertex_count - 1;
}

InternalVertex SuffixTree::Go(InternalVertex position, int left, int right) {
  // идем пока не закончится подстрока по которой мы идем
  while (left < right) {
    Node vertex = node_arr[position.vertex];
    int pos_in_edge = position.pos_in_edge;
    // условие того, что ребро закончилось, и надо переходить на новое
    if (pos_in_edge == vertex.LenEdge()) {
      position = InternalVertex(vertex.GetChild(str[left]), 0);
      // нету такой буквы  - выходим с ошибкой
      if (vertex.GetChild(str[left]) == -1) {
        return position;
      }
    } else {
      // идем по ребру, но следующая буква не та
      if (str[vertex.left_border_edge + pos_in_edge] != str[left]) {
        return InternalVertex(-1, -1);
      }
      // если остатот пути меньше чем остаток ребра, то сразу возвращаем нужную
      // нам позицию
      if (right - left < vertex.LenEdge() - pos_in_edge) {
        return InternalVertex(position.vertex, pos_in_edge + right - left);
      } else {
        // если нет, то идем в конец ребра и идем в следующую итерацию
        left += vertex.LenEdge() - pos_in_edge;
        position.pos_in_edge = vertex.LenEdge();
      }
    }
  }
  return position;
}

int SuffixTree::SuffLink(int vertex) {
  // уже посчитали суффиксную ссыдку раньше
  if (node_arr[vertex].link != -1) {
    return node_arr[vertex].link;
  } else if (node_arr[vertex].parent == -1) {
    // vervex - root
    return 0;
  }
  int parent_link = SuffLink(node_arr[vertex].parent);
  if (node_arr[vertex].parent == 0) {
    // в таком случае просто спускаемся по ребру по которому и поднялись ,
    // только без первого символа и создаем при необходимости новую вершину в
    // ребре
    InternalVertex iv = InternalVertex(0, 0);
    InternalVertex p = Go(iv, node_arr[vertex].left_border_edge + 1,
                          node_arr[vertex].right_border_edge);
    return node_arr[vertex].link = Share(p);
  } else {
    // переходим в суффиксную ссылку отца, а потом спускаемся на то же ребро,
    // что и поднялись до отца и создаем еще одну вершину при необходимости
    InternalVertex iv =
        InternalVertex(parent_link, node_arr[parent_link].LenEdge());
    InternalVertex p = Go(iv, node_arr[vertex].left_border_edge,
                          node_arr[vertex].right_border_edge);
    return node_arr[vertex].link = Share(p);
  }
}

int main() {
  string str_a;
  string str_b;
  cin >> str_a >> str_b;
  int a_size_ = str_a.size(); // сохраняем длину строки a, потому что хотим
                              // передавать строки через move
  SuffixTree my_class(std::move(str_a), std::move(str_b), a_size_);
  my_class.BuildTree();
  string result = my_class.Result();
  cout << result;
}
