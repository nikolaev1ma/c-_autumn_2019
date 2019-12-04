// ссылка на контест:
// https://contest.yandex.ru/contest/14570/run-report/23439957/

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::cin;
using std::cout;
using std::move;
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

  explicit Node(int parent_ = -1, int left_ = 0, int right_ = 0)
      : parent(parent_), str_number(0), left_border_edge(left_),
        right_border_edge(right_), link(-1),
        child(vector<int>(size_alphabet + 2, -1)) {}

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
  void UpdateChild(char letter, int child_) {
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
  static SuffixTree CompletedSuffixTree(string a, string b);

  string Description() const;

private:
  int vertex_count_;           // колво вершин
  const string str_;           // объеденение 2 строк
  const int a_str_size_;       // длина 1 строки
  const int str_size_;         // длина объедениения строк
  InternalVertex current_pos_; // текущая вершина
  vector<Node> node_arr_;      // массив вершин графа

  SuffixTree(string a,
             string b); // храним длину строки а, чтобы могли через mode
  // делать инициализацию

  void BuildTree(); // построение дерева

  void AddInTree(int new_letter); // добавляем в дерево все подстроки,
  // заканчивающие в new_letter

  InternalVertex Go(InternalVertex position, int left,
                    int right)
      const; // идем из position по дереву по подстроке str_[left ... right]

  int SuffLink(int vertex); // возвращаем ссылку на vertex

  int Share(InternalVertex pos); // разделяем при надобности ветки из position

  void DFS(int dfs_point, string &tmp_result_str, vector<int> &dfs_sort,
           int &dfs_counter) const; // проход по вершинам дерева для вывода
};

SuffixTree::SuffixTree(string a, string b)
    : a_str_size_(a.size()), str_(a + move(b)), str_size_(str_.size()),
      current_pos_(InternalVertex(0, 0)), vertex_count_(1), node_arr_(1) {}

void SuffixTree::BuildTree() {
  // строим дерево для каждого префикса строки последовательно
  for (int i = 0; i < str_size_; ++i) {
    AddInTree(i); // обновляем дерево
  }
}

// идем dfs по дереву, пока мы не наткнулись на символ '$'. Тогда обрезаем конец
// дерева. Таким образом у нас получается дерево в котором содержутся только
// суффиксы отдельно строки a и строки b. склеиных строк не будет, тк мы
// обрезаем конец ветки, как толко находим '$'
string SuffixTree::Description() const {
  vector<int> dfs_sort(vertex_count_);
  int dfs_counter = 0;
  string tmp_result_str;
  DFS(0, tmp_result_str, dfs_sort, dfs_counter); // dfs из вершины дерева

  std::stringstream ver_count_str;
  ver_count_str << dfs_counter + 1
                << '\n'; // заметим, что dfs_counter + 1 это и есть колво вершин
  // в нашем дереве
  const string result =
      ver_count_str.str() + tmp_result_str; // объяденяем два вывода
  return result;
}

// записываем информацию по поводу вершины по индексу dfs_point и продолжаем dfs
// при необходимости
void SuffixTree::DFS(int dfs_point, string &tmp_result_str,
                     vector<int> &dfs_sort, int &dfs_counter) const {
  const auto vertex = node_arr_[dfs_point];
  for (int i = 0; i < size_alphabet + 2; ++i) {
    // проходимся по детям
    if (vertex.child[i] == -1) {
      continue;
    }
    ++dfs_counter;
    dfs_sort[vertex.child[i]] =
        dfs_counter; // добавляем в отсортированный массив node
    Node child_vertex = node_arr_[vertex.child[i]]; // ребенок
    child_vertex.parent = dfs_sort[child_vertex.parent]; // меняем родителя
    const bool reach_$ = child_vertex.left_border_edge < a_str_size_ &&
                         child_vertex.right_border_edge >=
                             a_str_size_; // флаг того, что мы дошли до $
    if (reach_$) {
      // обрезаем
      child_vertex.right_border_edge = a_str_size_;
      child_vertex.str_number = 0;
    }
    // проверка на то, что ребро из слова b
    if (child_vertex.left_border_edge >= a_str_size_) {
      child_vertex.left_border_edge -= a_str_size_;
      child_vertex.right_border_edge -= a_str_size_;
      child_vertex.str_number = 1;
    }
    // запись информации
    std::stringstream description_node;
    description_node << child_vertex.parent << " " << child_vertex.str_number
                     << " " << child_vertex.left_border_edge << " "
                     << child_vertex.right_border_edge << '\n';
    tmp_result_str += description_node.str();
    if (!reach_$) {
      // продолжаем dfs
      DFS(vertex.child[i], tmp_result_str, dfs_sort, dfs_counter);
    }
  }
}

void SuffixTree::AddInTree(int new_letter) {
  while (true) {
    const InternalVertex next_pos =
        Go(current_pos_, new_letter,
           new_letter + 1); // переходим в следующую вершину
    // если можем перейти, то просто выходим
    if (next_pos.vertex != -1) {
      current_pos_ = next_pos;
      return;
    }
    const int split_edge_node = Share(current_pos_);
    ++vertex_count_;
    node_arr_.emplace_back(split_edge_node, new_letter,
                           str_size_); // добавляем новую ветку для новой буквы
    node_arr_[split_edge_node].UpdateChild(str_[new_letter], vertex_count_ - 1);
    // если мы уже оказались в root то выходим
    if (split_edge_node == 0) {
      return;
    }
    // переходим по суффиксной ссылки
    current_pos_.vertex = SuffLink(split_edge_node);
    current_pos_.pos_in_edge = node_arr_[current_pos_.vertex].LenEdge();
  }
}

// делим ребро по position при необходимости на два ребра поменьше
int SuffixTree::Share(InternalVertex position) {
  const int vertex = position.vertex;
  const int pos_in_edge = position.pos_in_edge;
  // условие того, что position это конец ребра - тоесть вершина в дереве
  if (pos_in_edge == node_arr_[vertex].LenEdge()) {
    return vertex;
  }
  // условие того, что position это начало ребра - тоесть вершина в дереве
  if (pos_in_edge == 0) {
    return node_arr_[vertex].parent;
  }
  const auto basic_node = node_arr_[vertex];
  ++vertex_count_;
  node_arr_.emplace_back(basic_node.parent, basic_node.left_border_edge,
                         basic_node.left_border_edge +
                             pos_in_edge); //добавляем вершину посреди ребра
  node_arr_[basic_node.parent].UpdateChild(
      str_[basic_node.left_border_edge],
      vertex_count_ - 1); // изменяем ссылку на ребенка у отца basic_node
  node_arr_[vertex_count_ - 1].UpdateChild(
      str_[basic_node.left_border_edge + pos_in_edge],
      vertex); // делаем ребенком у новой вершины basic_node
  node_arr_[vertex].parent = vertex_count_ - 1; // меняем родителя
  node_arr_[vertex].left_border_edge += pos_in_edge;
  return vertex_count_ - 1;
}

InternalVertex SuffixTree::Go(InternalVertex position, int left,
                              int right) const {
  // идем пока не закончится подстрока по которой мы идем
  while (left < right) {
    const Node vertex = node_arr_[position.vertex];
    const int pos_in_edge = position.pos_in_edge;
    // условие того, что ребро закончилось, и надо переходить на новое
    if (pos_in_edge == vertex.LenEdge()) {
      position = InternalVertex(vertex.GetChild(str_[left]), 0);
      // нету такой буквы  - выходим с ошибкой
      if (position.vertex == -1) {
        return position;
      }
      continue;
    }
    // идем по ребру, но следующая буква не та
    if (str_[vertex.left_border_edge + pos_in_edge] != str_[left]) {
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
  return position;
}

int SuffixTree::SuffLink(int vertex) {
  // уже посчитали суффиксную ссыдку раньше
  if (node_arr_[vertex].link != -1) {
    return node_arr_[vertex].link;
  } else if (node_arr_[vertex].parent == -1) {
    // vervex - root
    return 0;
  }
  const int parent_link = SuffLink(node_arr_[vertex].parent);
  if (node_arr_[vertex].parent == 0) {
    // в таком случае просто спускаемся по ребру по которому и поднялись ,
    // только без первого символа и создаем при необходимости новую вершину в
    // ребре
    const auto iv = InternalVertex(0, 0);
    const auto p = Go(iv, node_arr_[vertex].left_border_edge + 1,
                      node_arr_[vertex].right_border_edge);
    return node_arr_[vertex].link = Share(p);
  } else {
    // переходим в суффиксную ссылку отца, а потом спускаемся на то же ребро,
    // что и поднялись до отца и создаем еще одну вершину при необходимости
    const auto iv =
        InternalVertex(parent_link, node_arr_[parent_link].LenEdge());
    const auto p = Go(iv, node_arr_[vertex].left_border_edge,
                      node_arr_[vertex].right_border_edge);
    return node_arr_[vertex].link = Share(p);
  }
}

SuffixTree SuffixTree::CompletedSuffixTree(string a, string b) {
  auto new_suffix_tree = SuffixTree(a, b);
  new_suffix_tree.BuildTree();
  return new_suffix_tree;
}

int main() {
  string str_a;
  string str_b;
  cin >> str_a >> str_b;
  const auto my_class =
      SuffixTree::CompletedSuffixTree(move(str_a), move(str_b));
  const auto result = my_class.Description();
  cout << result;
}
