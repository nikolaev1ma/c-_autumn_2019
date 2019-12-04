#include <algorithm>
#include <iostream>
#include <limits>
#include <math.h>
#include <vector>

using std::cin;
using std::cout;
using std::vector;

const double eps = 1e-9999;

// вершина
struct CPoint {
public:
  CPoint() {}

  CPoint(int x, int y, int z, int counter)
      : x_(x), y_(y), z_(z), counter_(counter) {}

  int GetX() const { return x_; }

  int GetY() const { return y_; }

  int GetZ() const { return z_; }

  int GetCounter() const { return counter_; }

  double Length() const { return sqrt(x_ * x_ + y_ * y_ + z_ * z_); }

private:
  int x_; // координаты
  int y_;
  int z_;
  int counter_; // индекс точки в массиве вершин с которыми мы работаем
  // если точка вспомогательная, будем выставлять counter = -1
  // храним, потому что хотим за О(1) находить место в массиве
};

// векторная разность
CPoint operator-(const CPoint &a, const CPoint &b) {
  return CPoint(a.GetX() - b.GetX(), a.GetY() - b.GetY(), a.GetZ() - b.GetZ(),
                -1);
}

// равенство вершин
bool operator==(const CPoint &a, const CPoint &b) {
  return (a.GetX() == b.GetX() && a.GetY() == b.GetY() && a.GetZ() == b.GetZ());
}

// будем хранить ребро как 3 числа: индекс вершины одного конца ребра,
//                                  индекс вершины другого конца ребра,
//                                  индекс вершины, такой, что start, end,
//                                  face_point образуют грань, которую мы уже
//                                  построили
struct Edge {
  Edge(int s, int e, int f) : start(s), end(e), face_point(f) {}

  Edge() {}

  int start;
  int end;
  int face_point;
};

// плоскость
struct Plane {
  // A*x + B*y + C*z + D = 0
  double A;
  double B;
  double C;
  double D;

  // вершины, на которых мы строим нашу плоскость
  CPoint a;
  CPoint b;
  CPoint c;

  Plane() {}

  Plane(double A_, double B_, double C_, double D_)
      : A(A_), B(B_), C(C_), D(D_) {}

  Plane(CPoint a_, CPoint b_, CPoint c_) : a(a_), b(b_), c(c_) {
    const auto v = b - a;
    const auto w = c - a;
    // вычисляем нормаль
    A = v.GetY() * w.GetZ() - v.GetZ() * w.GetY();
    B = v.GetZ() * w.GetX() - v.GetX() * w.GetZ();
    C = v.GetX() * w.GetY() - v.GetY() * w.GetX();
    D = -A * a.GetX() - B * a.GetY() - C * a.GetZ();
  }

  double Length() { return sqrt(A * A + B * B + C * C); }
};

// Есть плоскость  A*x + B*y + C*z + D = 0, и есть отрезок ab. Хотим найти синус
// угла между ними
double SinFromLineAndPlane(const CPoint a, const CPoint b, double A, double B,
                           double C) {
  CPoint v = b - a;
  const auto len = sqrt(A * A + B * B + C * C);
  const auto sin =
      (A * v.GetX() + B * v.GetY() + C * v.GetZ()) / (len * v.Length());
  return sin;
}

// Находим косинус между плскостями, построенными на вершинах a, b, c и на a, b,
// d
double CosFromPlaneAndPlane(CPoint a, CPoint b, CPoint c, CPoint d) {
  // надо найти угол между p1 и p2
  Plane p1 = Plane(a, b, c);
  Plane p2 = Plane(a, b, d);
  if (abs(p1.Length()) < eps || abs(p2.Length()) < eps) {
    return 1;
  }
  double cos =
      (p1.A * p2.A + p1.B * p2.B + p1.C * p2.C) / (p1.Length() * p2.Length());
  return cos;
}

// Лексиграфический порядок
struct CompareVector {
  bool operator()(const vector<int> &a, const vector<int> &b) {
    if (a[0] != b[0]) {
      return (a[0] < b[0]);
    }
    return (a[1] < b[1]);
  }
};

class ConvexHull {
public:
  ConvexHull() {}

  ConvexHull(int ver_count);

  void AddPoint(double x, double y, double z); // добавление вершины в множество

  void Wrapping(); // заворачивание подарка

  void SortPlate(); // сортировка граней

private:
  vector<CPoint> point_arr; // массив вершин
  int vertex_count;         // колво вершин
  vector<Edge> edge_stek; // стек ребер, описание каждого ребра написано выше
  vector<vector<int>>
      plane; // массив граней. plane[i] это просто vector из 3 вершин

  Plane SearchLowerPlane(); // нахождение нижней грани

  void Stek(const Plane &lower_plane); // стек
};

void ConvexHull::AddPoint(double x, double y, double z) {
  point_arr.emplace_back(x, y, z, vertex_count);
  ++vertex_count;
}

ConvexHull::ConvexHull(int ver_count)
    : vertex_count(0), point_arr(0), plane(0), edge_stek(0) {}

void ConvexHull::Wrapping() {
  Plane lower_plane = SearchLowerPlane();
  Stek(lower_plane);
}

Plane ConvexHull::SearchLowerPlane() {
  // находим 1 вершину, которая точно пренадлежит нижней грани
  // сначала просто находим вершину с минимальной z координатой
  auto first = point_arr[0];
  for (const auto point : point_arr) {
    if (first.GetZ() > point.GetZ()) {
      first = point;
    }
  }
  // далее находим 2 вершину, которая точно пренадлежит нижней грани
  // для этого находим минимальный нормаль-вектор между first и всеми остальными
  // вершинами
  CPoint second;
  double min_sin = std::numeric_limits<double>::max();
  for (const auto point : point_arr) {
    if (point == first) {
      continue;
    }
    // нужно чтобы синус был минимальный
    // угол между прямой first, second и плоскостью z = 0
    if (SinFromLineAndPlane(first, point, 0, 0, 1) < min_sin + eps) {
      min_sin = SinFromLineAndPlane(first, point, 0, 0, 1);
      second = point;
    }
  }
  // находим 3 точку
  // для этого нужно найти такую грань, что угол между этой гранью и плоскостью
  // проходящей через точки first, second и additional должен быть минимальным.
  // Тоесть грань должна быть максимально "горизонтальной"
  CPoint third;
  // additional - это такая точка, что угол second - first - additional равен 90
  // градусов и first.z = additional.z
  CPoint additional;
  double max_cos = 0;
  additional =
      CPoint(second.GetY() - first.GetY() + first.GetX(),
             first.GetX() - second.GetX() + first.GetY(), first.GetZ(), -1);
  for (const auto point : point_arr) {
    if (point == first || point == second) {
      continue;
    }
    // косинус должен быть по модулю минимальным
    if (abs(CosFromPlaneAndPlane(first, second, additional, point)) >
        max_cos - eps) {
      third = point;
      max_cos = abs(CosFromPlaneAndPlane(first, second, additional, point));
    }
  }
  Plane result = Plane(first, second, third);
  return result;
}

void ConvexHull::Stek(const Plane &lower_plane) {
  const auto first = lower_plane.a;
  const auto second = lower_plane.b;
  const auto third = lower_plane.c;
  // смотрим на нормаль вектор. При правильной ориентации он должен смотреть
  // наверх, тоесть lower_plane.C > 0 будем также сохранять нужную ориентацию
  // для ребер
  if (lower_plane.C < 0) {
    // одна ориентация
    plane.push_back(
        {first.GetCounter(), second.GetCounter(), third.GetCounter()});
    edge_stek.emplace_back(first.GetCounter(), second.GetCounter(),
                           third.GetCounter());
    edge_stek.emplace_back(second.GetCounter(), third.GetCounter(),
                           first.GetCounter());
    edge_stek.emplace_back(third.GetCounter(), first.GetCounter(),
                           second.GetCounter());
  } else {
    // противоположная ориентация
    plane.push_back(
        {third.GetCounter(), second.GetCounter(), first.GetCounter()});
    edge_stek.emplace_back(third.GetCounter(), second.GetCounter(),
                           first.GetCounter());
    edge_stek.emplace_back(second.GetCounter(), first.GetCounter(),
                           third.GetCounter());
    edge_stek.emplace_back(first.GetCounter(), third.GetCounter(),
                           second.GetCounter());
  }
  while (!edge_stek.empty()) {
    // достаем из начала "стека" ребро
    const auto current_edge = edge_stek[0];
    const auto current_start = point_arr[current_edge.start];
    const auto current_end = point_arr[current_edge.end];
    const auto current_face = point_arr[current_edge.face_point];
    // теперь проходимся по всем вершинам и смотрим, чтобы угол между
    // построенной гранью и потенциальной был максимально развернутым
    auto min_cos = std::numeric_limits<double>::max();
    CPoint next; // вершина, на которую будет опираться новая грань
    for (const auto point : point_arr) {
      if (point == current_start || point == current_end ||
          point == current_face) {
        continue;
      }
      // раз угол развернутый это хорошо, то нам нужно, чтобы cos был
      // максимально близок к -1 чем меньше тем лучше
      if (min_cos + eps > CosFromPlaneAndPlane(current_start, current_end,
                                               current_face, point)) {
        min_cos = CosFromPlaneAndPlane(current_start, current_end, current_face,
                                       point);
        next = point;
      }
    }
    // удаляем наше ребро из стека
    edge_stek.erase(edge_stek.begin());
    // у нас есть 2 новых ребра (next, current_start) и (next, current_end)
    // но, они могли уже быть в стеке. Если они уже были в стеке, то тогда мы их
    // удаляем из стека
    bool have_got_in_stek_edge1 = false;
    bool have_got_in_stek_edge2 = false;
    int k, l; // будем запоминать позиции, которые надо удалить
    for (int i = 0; i < edge_stek.size(); ++i) {
      // проходимся по всем ребрам стека и проверяем на равенство ребер по
      // равенству границ ребра
      const auto edge = edge_stek[i];
      const auto edge_start = point_arr[edge.start];
      const auto edge_end = point_arr[edge.end];
      if (edge_start == current_start && edge_end == next) {
        have_got_in_stek_edge1 = true;
        k = i;
      }
      if (edge_end == current_start && edge_start == next) {
        have_got_in_stek_edge1 = true;
        k = i;
      }
      if (edge_start == current_end && edge_end == next) {
        have_got_in_stek_edge2 = true;
        l = i;
      }
      if (edge_end == current_end && edge_start == next) {
        have_got_in_stek_edge2 = true;
        l = i;
      }
    }
    // удаляем ребра из стека, если надо
    if (have_got_in_stek_edge1) {
      edge_stek.erase(edge_stek.begin() + k);
    }
    if (have_got_in_stek_edge2) {
      edge_stek.erase(edge_stek.begin() + l);
    }
    // добавляем новую грань
    // заметим, что ориентация вершин в ней сразу сохранилась, тк ориентация
    // current_edge: start -> end -> face а значит ориентация новой грани будет
    // противоположной по ребру (start, end), тоесть end -> start -> next
    plane.push_back({current_end.GetCounter(), current_start.GetCounter(),
                     next.GetCounter()});
    // пушим на стек ребра, если надо. Также сохраняем ориентацию
    if (!have_got_in_stek_edge1) {
      edge_stek.emplace_back(current_edge.start, next.GetCounter(),
                             current_edge.end);
    }
    if (!have_got_in_stek_edge2) {
      edge_stek.emplace_back(next.GetCounter(), current_edge.end,
                             current_edge.start);
    }
  }
}

void ConvexHull::SortPlate() {
  // теперь нужно просто пройтись по граням и поставить на первую позицию грань
  // с наименьшим значением, а потом отсортировать
  for (int i = 0; i < plane.size(); ++i) {
    const int sz = i;
    int my_min = std::numeric_limits<int>::max();
    int it;
    // находим наименьшее значение
    for (int j = 0; j < 3; ++j) {
      if (my_min > plane[sz][j]) {
        my_min = plane[sz][j];
        it = j;
      }
    }
    // делаем циклический сдвиг
    plane[sz] = {plane[sz][it], plane[sz][(it + 1) % 3],
                 plane[sz][(it + 2) % 3]};
  }
  // сортируем с помощью компоратора с лексиграфическим порядком
  std::sort(plane.begin(), plane.end(), CompareVector());
  // начало вывода
  cout << plane.size() << '\n';
  for (const auto element : plane) {
    cout << "3 " << element[0] << " " << element[1] << " " << element[2]
         << "\n";
  }
  point_arr.clear();
  edge_stek.clear();
  plane.clear();
}

int main() {
  int test_count;
  cin >> test_count;
  ConvexHull my_class;
  for (int i = 0; i < test_count; ++i) {
    int vertex_count;
    cin >> vertex_count;
    my_class = ConvexHull(vertex_count);
    for (int j = 0; j < vertex_count; ++j) {
      double x, y, z;
      cin >> x >> y >> z;
      my_class.AddPoint(x, y, z); // добавляем вершину
    }
    my_class.Wrapping();  // строим 3d оболочку
    my_class.SortPlate(); // выводим
  }
}
