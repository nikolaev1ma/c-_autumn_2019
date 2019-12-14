#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <vector>

using std::cin;
using std::cout;
using std::vector;

const double eps = std::numeric_limits<double>::epsilon();

struct CPoint {

  CPoint() = default;

  CPoint(int x_, int y_, int z_, int id_ = -1) : x(x_), y(y_), z(z_), id(id_) {}

  double Length() const { return std::sqrt(x * x + y * y + z * z); }

  int x; // координаты
  int y;
  int z;
  int id; // индекс точки в массиве вершин с которыми мы работаем
  // если точка вспомогательная, будем выставлять counter = -1
  // храним, потому что хотим за О(1) находить место в массиве
};

// векторная разность
CPoint operator-(const CPoint &a, const CPoint &b) {
  return CPoint(a.x - b.x, a.y - b.y, a.z - b.z);
}

// равенство вершин
bool operator==(const CPoint &a, const CPoint &b) {
  return (a.x == b.x && a.y == b.y && a.z == b.z);
}

struct Vector {
  double x;
  double y;
  double z;

  double Length() const { return std::sqrt(x * x + y * y + z * z); }
};

// будем хранить ребро как 3 числа: индекс вершины одного конца ребра,
//                                  индекс вершины другого конца ребра,
//                                  индекс вершины, такой, что start, end,
//                                  face_point образуют грань, которую мы уже
//                                  построили. Тк у нас стек будет хранить
//                                  только те ребра, которые смежны по лишь
//                                  только с одной гранью, то face_point
//                                  определяется однозначно
struct Edge {
  Edge(int s, int e, int f) : start(s), end(e), face_point(f) {}

  Edge() = default;

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
  Vector normal;
  // вершины, на которых мы строим нашу плоскость
  CPoint a;
  CPoint b;
  CPoint c;

  Plane() = default;

  Plane(double A_, double B_, double C_, double D_)
      : A(A_), B(B_), C(C_), D(D_), normal({A, B, C}) {}

  Plane(CPoint a_, CPoint b_, CPoint c_) : a(a_), b(b_), c(c_) {
    const auto v = b - a;
    const auto w = c - a;
    // вычисляем нормаль
    A = v.y * w.z - v.z * w.y;
    B = v.z * w.x - v.x * w.z;
    C = v.x * w.y - v.y * w.x;
    D = -A * a.x - B * a.y - C * a.z;
    normal = {A, B, C};
  }
};

// Есть плоскость  A*x + B*y + C*z + D = 0, и есть отрезок ab. Хотим найти синус
// угла между ними
double SinFromLineAndPlane(const CPoint &a, const CPoint &b, Vector normal) {
  CPoint v = b - a;
  return (normal.x * v.x + normal.y * v.y + normal.z * v.z) /
         (normal.Length() * v.Length());
}

// Находим косинус между плскостями, построенными на вершинах a, b, c и на a, b,
// d
double CosFromPlaneAndPlane(const Plane &p1, const Plane &p2) {
  // надо найти угол между p1 и p2
  if (std::abs(p1.normal.Length()) < eps ||
      std::abs(p2.normal.Length()) < eps) {
    return 1;
  }
  return (p1.A * p2.A + p1.B * p2.B + p1.C * p2.C) /
         (p1.normal.Length() * p2.normal.Length());
}


class ConvexHull {
public:
  ConvexHull() = default;

  void AddPoint(const CPoint &point); // добавление вершины в множество

  void Wrapping(); // заворачивание подарка

  void SortFacet(); // сортировка граней

  void DescriptionOfFacet() const; // вывод граней

private:
  vector<CPoint> points; // массив вершин
  vector<Edge> edge; // стек ребер, описание каждого ребра написано выше
  vector<vector<int>>
      plane; // массив граней. plane[i] это просто vector из 3 вершин

  Plane SearchLowerPlane() const; // нахождение нижней грани

  // для каждого ребра, которое смежно пока только одной гранью строим 2
  // смежнуюю к ней грань
  void AddingFacet(const Plane &lower_plane);
};

void ConvexHull::AddPoint(const CPoint &point) { points.emplace_back(point); }

void ConvexHull::Wrapping() {
  const auto lower_plane = SearchLowerPlane();
  AddingFacet(lower_plane);
}

Plane ConvexHull::SearchLowerPlane() const {
  // находим 1 вершину, которая точно принадлежит нижней грани
  // сначала просто находим вершину с минимальной z координатой
  auto first = points[0];
  for (const auto &point : points) {
    if (first.z > point.z) {
      first = point;
    }
  }
  // далее находим 2 вершину, которая точно принадлежит нижней грани
  // для этого находим минимальный нормаль-вектор между first и всеми остальными
  // вершинами
  CPoint second;
  double min_sin = std::numeric_limits<double>::max();
  for (const auto &point : points) {
    if (point == first) {
      continue;
    }
    // нужно чтобы синус был минимальный
    // угол между прямой first, second и плоскостью z = 0
    const auto current_sin = SinFromLineAndPlane(first, point, {0, 0, 1});
    if (current_sin < min_sin + eps) {
      min_sin = current_sin;
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
  const auto additional = CPoint(second.y - first.y + first.x,
                                 first.x - second.x + first.y, first.z);
  ;
  double max_cos = 0;
  for (const auto &point : points) {
    if (point == first || point == second) {
      continue;
    }
    // косинус должен быть по модулю минимальным
    const auto current_cos = std::abs(CosFromPlaneAndPlane(
        Plane(first, second, additional), Plane(first, second, point)));
    if (current_cos > max_cos - eps) {
      third = point;
      max_cos = current_cos;
    }
  }
  return Plane(first, second, third);
}

void ConvexHull::AddingFacet(const Plane &lower_plane) {
  const auto first = lower_plane.a;
  const auto second = lower_plane.b;
  const auto third = lower_plane.c;
  // смотрим на нормаль вектор. При правильной ориентации он должен смотреть
  // наверх, тоесть lower_plane.C > 0 будем также сохранять нужную ориентацию
  // для ребер
  if (lower_plane.C < 0) {
    // одна ориентация
    plane.push_back({first.id, second.id, third.id});
    edge.emplace_back(first.id, second.id, third.id);
    edge.emplace_back(second.id, third.id, first.id);
    edge.emplace_back(third.id, first.id, second.id);
  } else {
    // противоположная ориентация
    plane.push_back({third.id, second.id, first.id});
    edge.emplace_back(third.id, second.id, first.id);
    edge.emplace_back(second.id, first.id, third.id);
    edge.emplace_back(first.id, third.id, second.id);
  }
  while (!edge.empty()) {
    // достаем из начала "стека" ребро
    const auto current_edge = edge[0];
    const auto &current_start = points[current_edge.start];
    const auto &current_end = points[current_edge.end];
    const auto &current_face = points[current_edge.face_point];
    // теперь проходимся по всем вершинам и смотрим, чтобы угол между
    // построенной гранью и потенциальной был максимально развернутым
    auto min_cos = std::numeric_limits<double>::max();
    CPoint next; // вершина, на которую будет опираться новая грань
    for (const auto &point : points) {
      if (point == current_start || point == current_end ||
          point == current_face) {
        continue;
      }
      // раз угол развернутый это хорошо, то нам нужно, чтобы cos был
      // максимально близок к -1 чем меньше тем лучше
      const auto current_cos =
          CosFromPlaneAndPlane(Plane(current_start, current_end, current_face),
                               Plane(current_start, current_end, point));
      if (min_cos + eps > current_cos) {
        min_cos = current_cos;
        next = point;
      }
    }
    // удаляем наше ребро из стека
    edge.erase(edge.begin());
    // у нас есть 2 новых ребра (next, current_start) и (next, current_end)
    // но, они могли уже быть в стеке. Если они уже были в стеке, то тогда мы их
    // удаляем из стека
    bool have_got_in_stack_edge1 = false;
    bool have_got_in_stack_edge2 = false;
    int k, l; // будем запоминать позиции, которые надо удалить
    for (int i = 0; i < edge.size(); ++i) {
      // проходимся по всем ребрам стека и проверяем на равенство ребер по
      // равенству границ ребра
      const auto &edge_start = points[edge[i].start];
      const auto &edge_end = points[edge[i].end];
      if (edge_start == current_start && edge_end == next) {
        have_got_in_stack_edge1 = true;
        k = i;
      }
      if (edge_end == current_start && edge_start == next) {
        have_got_in_stack_edge1 = true;
        k = i;
      }
      if (edge_start == current_end && edge_end == next) {
        have_got_in_stack_edge2 = true;
        l = i;
      }
      if (edge_end == current_end && edge_start == next) {
        have_got_in_stack_edge2 = true;
        l = i;
      }
    }
    // удаляем ребра из стека, если надо
    if (have_got_in_stack_edge1) {
      edge.erase(edge.begin() + k);
    }
    if (have_got_in_stack_edge2) {
      edge.erase(edge.begin() + l);
    }
    // добавляем новую грань
    // заметим, что ориентация вершин в ней сразу сохранилась, тк ориентация
    // current_edge: start -> end -> face а значит ориентация новой грани будет
    // противоположной по ребру (start, end), тоесть end -> start -> next
    plane.push_back({current_end.id, current_start.id, next.id});
    // пушим на стек ребра, если надо. Также сохраняем ориентацию
    if (!have_got_in_stack_edge1) {
      edge.emplace_back(current_edge.start, next.id, current_edge.end);
    }
    if (!have_got_in_stack_edge2) {
      edge.emplace_back(next.id, current_edge.end, current_edge.start);
    }
  }
}

void ConvexHull::SortFacet() {
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
  std::sort(plane.begin(), plane.end());
}

void ConvexHull::DescriptionOfFacet() const {
  cout << plane.size() << '\n';
  for (const auto element : plane) {
    cout << "3 " << element[0] << " " << element[1] << " " << element[2]
         << "\n";
  }
}

int main() {
  int test_count;
  cin >> test_count;
  for (int i = 0; i < test_count; ++i) {
    int vertex_count;
    cin >> vertex_count;
    auto ch_builder = ConvexHull();
    for (int j = 0; j < vertex_count; ++j) {
      double x, y, z;
      cin >> x >> y >> z;
      const auto point = CPoint(x, y, z, j);
      ch_builder.AddPoint(point); // добавляем вершину
    }
    ch_builder.Wrapping(); // строим 3d оболочку
    ch_builder.SortFacet();
    ch_builder.DescriptionOfFacet();
  }
}
