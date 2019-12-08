#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

using std::cin;
using std::cout;
using std::vector;

struct CPoint {
public:
  CPoint() {}

  CPoint(double x_, double y_, int counter_ = -1)
      : x(x_), y(y_), counter(counter_), length(sqrt(x * x + y * y)),
        sin(y / length) {}

  double x;
  double y;
  int counter;
  double length;
  double sin;
};

// векторное сложение
CPoint operator+(const CPoint &a, const CPoint &b) {
  return CPoint(a.x + b.x, a.y + b.y);
}

// векторная разность
CPoint operator-(const CPoint &a, const CPoint &b) {
  return CPoint(a.x - b.x, a.y - b.y);
}

// умножение на константу
CPoint operator*(const CPoint &a, double k) { return CPoint(a.x * k, a.y * k); }

// сравнение по углу
bool compare_angle(const CPoint &a, const CPoint &b) {
  // вершина (0, 1) имеет наибольший приоритет, а дальше по часовой стрелке
  if (a.sin == 1) {
    return true;
  }
  if (b.sin == 1) {
    return false;
  }
  if (a.sin == -1) {
    return (b.x < 0);
  }
  if (b.sin == -1) {
    return (a.x > 0);
  }
  if (a.x > 0 && b.x < 0) {
    return true;
  }
  if (a.x < 0 && b.x > 0) {
    return false;
  }
  if (a.x > 0 && b.x > 0) {
    return (a.sin > b.sin);
  }
  if (a.x < 0 && b.x < 0) {
    return (a.sin < b.sin);
  }
}

vector<CPoint> MinkovskiSum(const vector<CPoint> &a_figure,
                            const vector<CPoint> &b_figure) {
  // найдем самую левую нижнюю вершину в каждой их фигур. С них и начнем обход
  CPoint a_left_down = a_figure[0], b_left_down = b_figure[0];
  const auto a_size = a_figure.size();
  const auto b_size = b_figure.size();
  for (const auto &point : a_figure) {
    if (a_left_down.x > point.x ||
        (a_left_down.x == point.x && a_left_down.y > point.y)) {
      a_left_down = point;
    }
  }
  for (const auto CPoint : b_figure) {
    if (b_left_down.x > CPoint.x ||
        (b_left_down.x == CPoint.x && b_left_down.y > CPoint.y)) {
      b_left_down = CPoint;
    }
  }
  vector<CPoint> minkovski_sum(a_size + b_size); // сумма двух фигур
  // сразу же заметим, что две самые левые нижние вершины в сумме дают вершину
  // новой фигуры
  minkovski_sum[0] = a_left_down + b_left_down;
  int counter_a = 0; // счетчик по 1 фигуре
  int counter_b = 0; // счетчик по 2 фигуре
  // делаем обход по каждой фигуре по часовой стрелке, каждый раз выбирая
  // меньший угол из 2 претендентов по каждой фигуре
  while (counter_a + counter_b < a_size + b_size - 1) {
    // ребро между текущей и следующей фигурой в а: next_point_a -
    // current_point_a
    const auto current_point_a =
        a_figure[(a_left_down.counter + counter_a) % a_size];
    const auto next_point_a =
        a_figure[(a_left_down.counter + 1 + counter_a) % a_size];
    // ребро между текущей и следующей фигурой в b: next_point_b -
    // current_point_b
    const auto current_point_b =
        b_figure[(b_left_down.counter + counter_b) % b_size];
    const auto next_point_b =
        b_figure[(b_left_down.counter + 1 + counter_b) % b_size];
    // проверка на угол
    if (compare_angle(next_point_a - current_point_a,
                      next_point_b - current_point_b)) {
      ++counter_a;
      minkovski_sum[counter_a + counter_b] =
          minkovski_sum[counter_a + counter_b - 1] + next_point_a -
          current_point_a;
    } else {
      ++counter_b;
      minkovski_sum[counter_a + counter_b] =
          minkovski_sum[counter_a + counter_b - 1] + next_point_b -
          current_point_b;
    }
  }
  return minkovski_sum;
}

// проверка на то, что внутри фигуры есть начало координат
bool ContainsCenter(const vector<CPoint> &figure) {
  // проходимся по фигуре и прверяем, что каждое ребро "хорошое"
  const auto center = CPoint(0, 0);
  for (int i = 0; i < figure.size(); ++i) {
    const auto current_point = figure[i];
    const auto next_point = figure[(i + 1) % figure.size()];
    const auto v = next_point - current_point;
    const auto w = center - current_point;
    // проверяем на то, что векторное произведение положительно
    if (v.x * w.y - v.y * w.x > 0) {
      return false;
    }
  }
  return true;
}

int main() {
  int a_figure_size, b_figure_size;
  cin >> a_figure_size;
  vector<CPoint> a_figure;
  a_figure.reserve(a_figure_size);
  for (int i = 0; i < a_figure_size; ++i) {
    double x, y;
    cin >> x >> y;
    a_figure.emplace_back(x, y, i);
  }
  cin >> b_figure_size;
  vector<CPoint> b_figure;
  b_figure.reserve(b_figure_size);
  for (int i = 0; i < b_figure_size; ++i) {
    double x, y;
    cin >> x >> y;
    // отразим фигуру b относительно центра координат
    b_figure.emplace_back(-x, -y, i);
  }
  // пересечение фигур a и b равносильно тому, что центр координат лежит в сумма
  // минковского фигур a и -b
  const auto sum = MinkovskiSum(a_figure, b_figure);
  // проверяем, что центр координат пренадлежит сумме
  cout << ((ContainsCenter(sum)) ? "YES" : "NO") << std::endl;
}
