// ссылка на контест:
// https://contest.yandex.ru/contest/14982/run-report/29027112/
#include <algorithm>
#include <cmath>
#include <iostream>
#include <list>
#include <stack>
#include <unordered_map>
#include <vector>

using std::cin;
using std::cout;
using std::hash;
using std::pair;
using std::stack;
using std::unordered_map;
using std::vector;

const double eps = std::numeric_limits<double>::epsilon();

struct CPoint {
  CPoint() = default;

  CPoint(double x_, double y_, int counter_)
      : x(x_), y(y_), counter(counter_) {}

  double Length() const { return std::sqrt(x * x + y * y); }

  // равенство вершин
  bool operator==(const CPoint &other) const {
    return ((x == other.x) && (y == other.y));
  }

  double x; // координаты
  double y;
  int counter; // индекс вершины в множестве
};

// векторная разность
CPoint operator-(const CPoint &a, const CPoint &b) {
  return CPoint(a.x - b.x, a.y - b.y, -1);
}

// растояние между точками
double Distance(const CPoint &a, const CPoint &b) {
  const auto v = b - a;
  return v.Length();
}

// сравнение по координате x
struct PointCompare {
  bool operator()(const CPoint &a, const CPoint &b) const {
    return (a.x < b.x);
  }
};

// считаем не нормированный косиус угла bac
double NotRationedCos(const CPoint &b, const CPoint &a, const CPoint &c) {
  const auto v = b - a;
  const auto w = c - a;
  return (v.x * w.x + v.y * w.y);
}

// считаем не нормированный синус угла bac
double NotRationedSin(const CPoint &b, const CPoint &a, const CPoint &c) {
  const auto v = b - a;
  const auto w = c - a;
  return std::abs(v.x * w.y - v.y * w.x);
}

// есть 2 треугольника abc и abd.  мы хотим проверить, что они удоволетворяют
// условию Делоне
bool GoodEdge(const CPoint &a, const CPoint &b, const CPoint &c,
              const CPoint &d) {
  if (d.counter == -1 || c.counter == -1) {
    return true;
  }
  return (NotRationedSin(a, c, b) * NotRationedCos(a, d, b) +
              NotRationedSin(a, d, b) * NotRationedCos(a, c, b) >
          -eps);
}

// ребро
struct CPointPair {
  CPoint a;
  CPoint b;

  // ребра равны, если у них совпадают концы
  bool operator==(const CPointPair &other) const {
    return (((a == other.a) && (b == other.b)) ||
            ((b == other.a) && (a == other.b)));
  }
};

// хеш для пары вершин
struct HahCpointPair {
  hash<double> hash_fn;
  std::size_t operator()(const CPointPair &k) const {
    return hash_fn(k.a.x) ^ hash_fn(k.a.y) ^ hash_fn(k.b.x) ^ hash_fn(k.b.y);
  }
};

class Triangulation {
public:
  Triangulation();

  void AddPoint(const CPoint &point); // добавление вершины в наше множестов

  void Build(); // построение триангуляции Делоне

  double AverageDeg()
      const; // среднее значение степеней вершин внутри выкуплой оболочки

private:
  vector<CPoint> point_arr; // массив вершин
  // edge - множество ребер триангуляции , где каждому ребру сопоставляется 2
  // вершины, которые образуют смежные треугольники триангуляции
  unordered_map<CPointPair, CPointPair, HahCpointPair> edge;
  std::list<CPoint> shell; // вершины текущей выкуплой оболочки

  void Step(const int &counter); // шаг построения. Обновление триангуляции для
                                 // нового множества вершин

  // обновление выпуклой оболочки на очередном шаге. Возвращает множесто видимых
  // вершин
  vector<CPoint> ShellUpdate(const int counter);

  // обновление ребер, смежных с добавляемой вершиной
  void EdgeUpdate(const vector<CPoint> &visible, const int counter);

  // проходимся вглубь ранее построенной триангуляции, изменяя при необходимости
  // ребра, если они "плохие"
  void UpdateEdgeInTriangulation(const vector<CPoint> &visible,
                                 const int counter);
};

Triangulation::Triangulation() : point_arr(0), edge(0), shell(0) {}

void Triangulation::AddPoint(const CPoint &point) {
  point_arr.emplace_back(point);
}

void Triangulation::Build() {
  // для начала сортируем все вершины по координтате x
  std::sort(point_arr.begin(), point_arr.end(), PointCompare());
  // далее делаем инициализацию для начального треугольника
  // в edge будем поддерживать инвариант, что если у нас есть infinity, то мы
  // будем его закидывать во второй аргумент value
  const auto infinity =
      CPoint(0, 0, -1); // должны остлеживать, что ребро граничное
  const CPoint &first = point_arr[0];
  const CPoint &second = point_arr[1];
  const CPoint &third = point_arr[2];
  edge[{first, second}] = {third, infinity};
  edge[{second, third}] = {first, infinity};
  edge[{third, first}] = {second, infinity};

  shell.push_back(third);
  shell.push_back(second);
  shell.push_back(first);
  // дедаем пошаговое обновление триангуляции, каждый раз добавляя по одной
  // вершине в триангуляцию
  for (int counter = 3; counter < point_arr.size(); ++counter) {
    Step(counter);
  }
}

// добавляем в множесто point[counter]
void Triangulation::Step(const int &counter) {
  // сначала обновляем выпуклую оболчку и нахдим видимые вершины для
  // point_arr[counter]
  const auto visible = ShellUpdate(counter);
  // далее обноляем триангуляцию
  EdgeUpdate(visible, counter);
  UpdateEdgeInTriangulation(visible, counter);
}

vector<CPoint> Triangulation::ShellUpdate(const int counter) {
  const auto added = point_arr[counter]; // добавляемая вершина
  auto it_shell = shell.begin();
  int i = 0;
  // хотим поддерживать, что shell[0] это всегда point_arr[counter - 1]
  // также хотим поддерживать, что в shell вершины лежат в порядке обходе вершин
  // по выпуклой оболочке очевидно, что point_arr[counter - 1] видима для
  // point_arr[counter]
  while (true) {
    auto tmp_it = it_shell;
    const auto current_point = *tmp_it;
    ++tmp_it;
    if (tmp_it == shell.end()) {
      ++tmp_it; // замкнулись
    }
    const auto next_point = *tmp_it;
    ++tmp_it;
    if (tmp_it == shell.end()) {
      ++tmp_it;
    }
    const auto in_shell = *tmp_it;
    const auto v = next_point - current_point;
    const auto w = in_shell - current_point;
    const auto p = added - current_point;
    // пока прямая (current, next) разделяет плоскость на 2 полуплоскости, где
    // added и все остальные точки лежат по разные стороны, current видима для
    // этого посчитаем скаляр (v, w) и (v, p). Они должны быть разного знака
    const auto scalar1 = v.x * w.y - v.y * w.x;
    const auto scalar2 = v.x * p.y - v.y * p.x;
    if (scalar1 * scalar2 > 0) {
      break;
    }
    ++it_shell;
    if (it_shell == shell.end()) {
      ++it_shell;
    }
    i = (i + 1) % shell.size();
  }
  // обозначим границы видимого куска
  int left_border = i;
  int right_border = i;
  vector<CPoint> visible; // будем сюда складывать видимые вершины
  // теперь идем в противоположную сторону, по ходу добавляя вершины в visible
  while (true) {
    // то же самое
    auto tmp_it = it_shell;
    const auto current_point = *tmp_it;
    --tmp_it;
    if (tmp_it == shell.end()) {
      --tmp_it;
    }
    const auto next_point = *tmp_it;
    --tmp_it;
    if (tmp_it == shell.end()) {
      --tmp_it;
    }
    const auto in_shell = *tmp_it;
    visible.emplace_back(current_point);
    const auto v = next_point - current_point;
    const auto w = in_shell - current_point;
    const auto p = added - current_point;
    const double scalar1 = v.x * w.y - v.y * w.x;
    const double scalar2 = v.x * p.y - v.y * p.x;
    if (scalar1 * scalar2 > 0) {
      break;
    }
    i = (i - 1 + shell.size()) % shell.size();
    right_border = i;
    --it_shell;
    if (it_shell == shell.end()) {
      --it_shell;
    }
  }
  // удаляем из выпуклой оболочке ненужные вершины, поддерживая принцип,
  // описанный выше
  if (right_border == 0) {
    const auto begin = *(shell.begin());
    auto it = shell.begin();
    std::advance(it, left_border);
    shell.erase(shell.begin(), it);
    shell.push_back(begin);
  } else {
    if (right_border != shell.size() - 1) {
      auto it = shell.begin();
      std::advance(it, right_border + 1);
      shell.erase(it, shell.end());
    }
    auto it = shell.begin();
    std::advance(it, left_border);
    shell.erase(shell.begin(), it);
  }
  shell.insert(shell.begin(), added);
  return visible;
}

void Triangulation::EdgeUpdate(const vector<CPoint> &visible,
                               const int counter) {
  const auto added = point_arr[counter];
  // проходимся по видимым вершинам и изменяем ребра между этими вершинами
  // раньше эти вершины имели вторым аргументов в value - infinity - теперь
  // added
  for (int i = 1; i < visible.size(); ++i) {
    const auto current_point = visible[i - 1];
    const auto next_point = visible[i];
    edge[{current_point, next_point}].b = added;
  }
  // теперь добавляем смежные с added ребра
  const auto infinity = CPoint(0, 0, -1);
  edge[{added, visible[0]}] = {visible[1], infinity};
  edge[{added, visible[visible.size() - 1]}] = {visible[visible.size() - 2],
                                                infinity};
  for (int i = 1; i < visible.size() - 1; ++i) {
    const auto current_point = visible[i];
    const auto next_point = visible[i + 1];
    const auto prev_point = visible[i - 1];
    edge[{added, current_point}] = {prev_point, next_point};
  }
}

void Triangulation::UpdateEdgeInTriangulation(const vector<CPoint> &visible,
                                              const int counter) {
  stack<CPointPair> s; // стек ребер
  // закидываем в стек ребра из visible
  // будем поддерживать инвариант, что, edge[].a "направлен внутрь" множества
  // вершин, тоесть стек будет идти вглубь именно по edge[].a
  for (int i = 1; i < visible.size(); ++i) {
    const auto current_point = visible[i - 1];
    const auto next_point = visible[i];
    s.push({current_point, next_point});
  }
  // пока стек не пустой - есть ребра, которые надо проверить и при случае
  // обновить
  while (!s.empty()) {
    const auto current_edge = s.top();
    s.pop();
    const auto neighbors = edge[current_edge];
    const auto a = current_edge.a;
    const auto b = current_edge.b;
    const auto c = neighbors.a;
    const auto d = neighbors.b;
    // проверяем на условие делоне
    if (!GoodEdge(a, b, c, d)) {
      // надо сделать swap ребер в четырехугольнике
      // обновляем ребра в четырехугольнике
      edge.erase({a, b});
      edge[{c, d}] = {a, b};
      auto &ca = edge[{c, a}];
      if (ca.b == b) {
        ca.b = d;
      } else {
        const auto tmp = ca.b;
        ca.b = d;
        ca.a = tmp;
      }
      auto &cb = edge[{c, b}];
      if (cb.b == a) {
        cb.b = d;
      } else {
        const auto tmp = cb.b;
        cb.b = d;
        cb.a = tmp;
      }
      auto &ad = edge[{a, d}];
      if (ad.a == b) {
        ad.a = c;
      } else {
        ad.b = c;
      }
      auto &bd = edge[{b, d}];
      if (bd.a == a) {
        bd.a = c;
      } else {
        bd.b = c;
      }
      // добавление в стек
      s.push({b, c});
      s.push({c, a});
    }
  }
}

// нужно найти среднее значение степеней вершин внутри выпуклой оболочке
// тк это двойственно среднему значению колву ребер в ограниченных гранях в
// диаграмме воронова
double Triangulation::AverageDeg() const {
  // проверка на то, что вершина лежит в выпуклой оболчке
  vector<bool> is_shell(point_arr.size());
  for (const auto &e : shell) {
    is_shell[e.counter] = true;
  }
  // суммарная степень "нужных" вершин
  int deg_sum = 0;
  for (const auto &e : edge) {
    // если конец ребра лежит внутри выпуклой оболочки, то увеличим сумму
    if (!is_shell[e.first.a.counter]) {
      ++deg_sum;
    }
    if (!is_shell[e.first.b.counter]) {
      ++deg_sum;
    }
  }
  // колво вершин внутри выпуклой оболочки
  double norm_counter = point_arr.size() - shell.size();
  if (norm_counter == 0) {
    return 0;
  }
  return deg_sum / norm_counter;
}

int main() {
  std::cout.precision(7);
  cout << std::fixed;
  double x, y;
  Triangulation triangulation_build;
  int i = 0;
  while (cin >> x) {
    cin >> y;
    const auto point = CPoint(x, y, i);
    triangulation_build.AddPoint(point);
    ++i;
  }
  triangulation_build.Build(); // строим триангуляцию
  cout << triangulation_build.AverageDeg() << '\n';
}
