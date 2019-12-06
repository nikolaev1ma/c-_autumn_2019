#include <algorithm>
#include <cmath>
#include <iostream>
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
public:
  CPoint() {}

  CPoint(double x, double y, int counter) : x_(x), y_(y), counter_(counter) {}

  double GetX() const { return x_; }

  double GetY() const { return y_; }

  double Length() const { return sqrt(x_ * x_ + y_ * y_); }

  int GetCounter() const { return counter_; }

private:
  double x_; // координаты
  double y_;
  int counter_; // индекс вершины в множестве
};

// векторная разность
CPoint operator-(const CPoint &a, const CPoint &b) {
  return CPoint(a.GetX() - b.GetX(), a.GetY() - b.GetY(), -1);
}

// равернство вершин
bool operator==(const CPoint &a, const CPoint &b) {
  return ((a.GetX() == b.GetX()) && (a.GetY() == b.GetY()));
}

// растояние между точками
double Distance(const CPoint &a, const CPoint &b) {
  const auto v = b - a;
  return v.Length();
}

// сравнение по координате x
struct compare {
  bool operator()(const CPoint &a, const CPoint &b) {
    return (a.GetX() < b.GetX());
  }
};

// считаем косиус угла bac
double Cos(const CPoint &b, const CPoint &a, const CPoint &c) {
  const auto ab = Distance(a, b);
  const auto ac = Distance(a, c);
  const auto bc = Distance(b, c);
  return (ab * ab + ac * ac - bc * bc) / (2 * ab * ac);
}

// считаем синус угла bac
double Sin(const CPoint &b, const CPoint &a, const CPoint &c) {
  const auto cos = Cos(b, a, c);
  return abs(sqrt(1 - cos * cos));
}

// есть 2 треугольника abc и abd.  мы хотим проверить, что они удоволетворяют
// условию Делоне
bool GoodEdge(const CPoint &a, const CPoint &b, const CPoint &c,
              const CPoint &d) {
  if (d.GetCounter() == -1 || c.GetCounter() == -1) {
    return true;
  }
  return (Sin(a, c, b) * Cos(a, d, b) + Sin(a, d, b) * Cos(a, c, b) > -eps);
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
namespace std {
template <> struct hash<CPointPair> {
  std::size_t operator()(const CPointPair &k) const {
    return hash<double>()(k.a.GetX()) ^ hash<double>()(k.a.GetY()) ^
           hash<double>()(k.b.GetX()) ^ hash<double>()(k.b.GetY());
  }
};
} // namespace std

class Triangulation {
public:
  Triangulation();

  void AddPoint(double x, double y); // добавление вершины в наше множестов

  void Construction(); // построение триангуляции Делоне

  double AverageDig()
      const; // среднее значение степеней вершин внутри выкуплой оболочки

private:
  vector<CPoint> point_arr; // массив вершин
  // edge - множество ребер триангуляции , где каждому ребру сопоставляется 2
  // вершины, которые образуют смежные треугольники триангуляции
  unordered_map<CPointPair, CPointPair> edge;
  vector<CPoint> shell; // вершины текущей выкуплой оболочки
  int ver_count; // колво вершин

  void Step(const int counter); // шаг построения. Обновление триангуляции для
                                // нового множества вершин

  // обновление выпуклой оболочки на очередном шаге. Возвращает множесто видимых
  // вершин
  vector<CPoint> ShellUpdate(const int counter);

  // обновление ребер, смежных с добавляемой вершиной
  void EdgeUpdate(const vector<CPoint> &visible, const int counter);

  void Stack(const vector<CPoint> &visible, const int counter); // стек
};

Triangulation::Triangulation()
    : ver_count(0), point_arr(0), edge(0), shell(0) {}

void Triangulation::AddPoint(double x, double y) {
  point_arr.emplace_back(x, y, ver_count);
  ++ver_count;
}

void Triangulation::Construction() {
  // для начала сортируем все вершины по координтате x
  std::sort(point_arr.begin(), point_arr.end(), compare());
  // далее делаем инициализацию для начального треугольника
  // в edge будем поддерживать инвариант, что если у нас есть infinity, то мы
  // будем его закидывать во второй аргумент value
  const auto infinity =
      CPoint(0, 0, -1); // должны остлеживать, что ребро граничное
  const CPoint first = point_arr[0];
  const CPoint second = point_arr[1];
  const CPoint third = point_arr[2];
  edge[{first, second}] = {third, infinity};
  edge[{second, third}] = {first, infinity};
  edge[{third, first}] = {second, infinity};

  shell.emplace_back(third);
  shell.emplace_back(second);
  shell.emplace_back(first);
  // дедаем пошаговое обновление триангуляции, каждый раз добавляя по одной
  // вершине в триангуляцию
  for (int counter = 3; counter < ver_count; ++counter) {
    Step(counter);
  }
}

// добавляем в множесто point[counter]
void Triangulation::Step(const int counter) {
  // сначала обновляем выпуклую оболчку и нахдим видимые вершины для
  // point_arr[counter]
  vector<CPoint> visible = ShellUpdate(counter);
  // далее обноляем триангуляцию
  EdgeUpdate(visible, counter);
  Stack(visible, counter);
  return;
}

vector<CPoint> Triangulation::ShellUpdate(const int counter) {
  const auto added = point_arr[counter]; // добавляемая вершина
  int i = 0;
  // хотим поддерживать, что shell[0] это всегда point_arr[counter - 1]
  // также хотим поддерживать, что в shell вершины лежат в порядке обходе вершин
  // по выпуклой оболочке очевидно, что point_arr[counter - 1] видима для
  // point_arr[counter]
  while (true) {
    const auto current_point = shell[i];
    const auto next_point = shell[(i + 1) % shell.size()];
    const auto in_shell = shell[(i + 2) % shell.size()];
    const auto v = next_point - current_point;
    const auto w = in_shell - current_point;
    const auto p = added - current_point;
    // пока прямая (current, next) разделяет плоскость на 2 полуплоскости, где
    // added и все остальные точки лежат по разные стороны, current видима для
    // этого посчитаем скаляр (v, w) и (v, p). Они должны быть разного знака
    const auto scalar1 = v.GetX() * w.GetY() - v.GetY() * w.GetX();
    const auto scalar2 = v.GetX() * p.GetY() - v.GetY() * p.GetX();
    if (scalar1 * scalar2 > 0) {
      break;
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
    const auto current_point = shell[i];
    const auto next_point = shell[(i - 1 + shell.size()) % shell.size()];
    const auto in_shell = shell[(i - 2 + shell.size()) % shell.size()];
    visible.emplace_back(shell[i]);
    const auto v = next_point - current_point;
    const auto w = in_shell - current_point;
    const auto p = added - current_point;
    const double scalar1 = v.GetX() * w.GetY() - v.GetY() * w.GetX();
    const double scalar2 = v.GetX() * p.GetY() - v.GetY() * p.GetX();
    if (scalar1 * scalar2 > 0) {
      break;
    }
    i = (i - 1 + shell.size()) % shell.size();
    right_border = i;
  }
  // удаляем из выпуклой оболочке ненужные вершины, поддерживая принцип,
  // описанный выше
  if (right_border == 0) {
    const auto begin = shell[0];
    shell.erase(shell.begin(), shell.begin() + left_border);
    shell.push_back(begin);
  } else {
    if (right_border != shell.size() - 1) {
      shell.erase(shell.begin() + right_border + 1, shell.end());
    }
    shell.erase(shell.begin(), shell.begin() + left_border);
  }
  shell.insert(shell.begin(), added);
  return visible;
}

//
void Triangulation::EdgeUpdate(const vector<CPoint> &visible,
                               const int counter) {
  const auto added = point_arr[counter];
  // проходимся по видимым вершинам и извеняем ребра между этими вершинами
  // раньше эти вершины имели вторым аргументов в value - infinity - теперь
  // added
  for (int i = 1; i < visible.size(); ++i) {
    const auto current_point = visible[i - 1];
    const auto next_point = visible[i];
    CPointPair cp = edge[{current_point, next_point}];
    cp.b = added;
    edge[{current_point, next_point}] = cp;
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

void Triangulation::Stack(const vector<CPoint> &visible, const int counter) {
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
      if (edge[{c, a}].b == b) {
        edge[{c, a}].b = d;
      } else {
        const auto tmp = edge[{c, a}].b;
        edge[{c, a}].b = d;
        edge[{c, a}].a = tmp;
      }
      if (edge[{c, b}].b == a) {
        edge[{c, b}].b = d;
      } else {
        const auto tmp = edge[{c, b}].b;
        edge[{c, b}].b = d;
        edge[{c, b}].a = tmp;
      }
      if (edge[{a, d}].a == b) {
        edge[{a, d}].a = c;
      } else {
        edge[{a, d}].b = c;
      }
      if (edge[{b, d}].a == a) {
        edge[{b, d}].a = c;
      } else {
        edge[{b, d}].b = c;
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
double Triangulation::AverageDig() const {
  // проверка на то, что вершина лежит в выпуклой оболчке
  vector<bool> is_shell(ver_count);
  for (const auto e : shell) {
    is_shell[e.GetCounter()] = true;
  }
  // суммарная степень "нужных" вершин
  int deg_sum = 0;
  for (const auto e : edge) {
    // если конец ребра лежит внутри выпуклой оболочки, то увеличим сумму
    if (!is_shell[e.first.a.GetCounter()]) {
      ++deg_sum;
    }
    if (!is_shell[e.first.b.GetCounter()]) {
      ++deg_sum;
    }
  }
  // колво вершин внутри выпуклой оболочки
  double norm_counter = ver_count - shell.size();
  if (norm_counter == 0) {
    return 0;
  }
  double result = deg_sum / norm_counter;
  return result;
}

int main() {
  std::cout.precision(12);
  double x, y;
  Triangulation my_class;
  while (cin >> x) {
    cin >> y;
    my_class.AddPoint(x, y);
  }
  my_class.Construction(); // строим триангуляцию
  cout << my_class.AverageDig() << '\n';
}
