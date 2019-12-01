#include <algorithm>
#include <iostream>
#include <math.h>
#include <vector>

using std::cin;
using std::cout;
using std::vector;

struct CPoint {
public:
  CPoint() {}

  CPoint(double x, double y, int counter) : x_(x), y_(y), counter_(counter) {}

  double GetX() const { return x_; }

  double GetY() const { return y_; }

  double Length() const { return sqrt(x_ * x_ + y_ * y_); }

  double Angle() const { return y_ / Length(); }

  int GetCounter() const { return counter_; }

private:
  double x_;
  double y_;
  int counter_;
};

// векторное сложение
CPoint operator+(const CPoint &a, const CPoint &b) {
  return CPoint(a.GetX() + b.GetX(), a.GetY() + b.GetY(), -1);
}

// векторная разность
CPoint operator-(const CPoint &a, const CPoint &b) {
  return CPoint(a.GetX() - b.GetX(), a.GetY() - b.GetY(), -1);
}

// умножение на константу
CPoint operator*(const CPoint &a, double k) {
  return CPoint(a.GetX() * k, a.GetY() * k, -1);
}

// сравнение по углу
bool compare_angle(const CPoint &a, const CPoint &b) {
  // вершина (0, 1) имеет наибольший приоритет, а дальше по часовой стрелке
  if (a.GetX() > 0 && b.GetX() < 0) {
    return true;
  }
  if (a.GetX() < 0 && b.GetX() > 0) {
    return false;
  }
  if (a.GetX() > 0 && b.GetX() > 0) {
    return (a.Angle() > b.Angle());
  }
  if (a.GetX() < 0 && b.GetX() < 0) {
    return (a.Angle() < b.Angle());
  }
  if (a.Angle() == 1) {
    return true;
  }
  if (b.Angle() == 1) {
    return false;
  }
  if (a.Angle() == -1) {
    return (b.GetX() < 0);
  }
  if (b.Angle() == -1) {
    return (a.GetX() > 0);
  }
}

vector<CPoint> MinkovskiSum(vector<CPoint> a_figure, vector<CPoint> b_figure) {
  // найдем самую левую нижнюю вершину в каждой их фигур. С них и начнем обход
  CPoint a_left_down = a_figure[0], b_left_down = b_figure[0];
  const auto a_size = a_figure.size();
  const auto b_size = b_figure.size();
  for (const auto CPoint : a_figure) {
    if (a_left_down.GetX() > CPoint.GetX() ||
        (a_left_down.GetX() == CPoint.GetX() &&
         a_left_down.GetY() > CPoint.GetY())) {
      a_left_down = CPoint;
    }
  }
  for (const auto CPoint : b_figure) {
    if (b_left_down.GetX() > CPoint.GetX() ||
        (b_left_down.GetX() == CPoint.GetX() &&
         b_left_down.GetY() > CPoint.GetY())) {
      b_left_down = CPoint;
    }
  }
  vector<CPoint> minkovski_sum(a_size + b_size); // сумма двух фигур
  // сразу же заметим, что две самые левые нижние вершины в сумме дают вершину
  // новой фигуры
  minkovski_sum[0] = a_left_down + b_left_down;
  int counter_a = 0; // счетчик по 1 фигуре
  int counter_b = 0; // счетчик по 2 фигуре
  int counter = 0;   // просто сумма counter_a и counter_b
  // делаем обход по каждой фигуре по часовой стрелке, каждый раз выбирая
  // меньший угол из 2 претендентов по каждой фигуре
  while (counter < a_size + b_size - 1) {
    // ребро между текущей и следующей фигурой в а: next_point_a -
    // current_point_a
    const auto current_point_a =
        a_figure[(a_left_down.GetCounter() + counter_a) % a_size];
    const auto next_point_a =
        a_figure[(a_left_down.GetCounter() + 1 + counter_a) % a_size];
    // ребро между текущей и следующей фигурой в b: next_point_b -
    // current_point_b
    const auto current_point_b =
        b_figure[(b_left_down.GetCounter() + counter_b) % b_size];
    const auto next_point_b =
        b_figure[(b_left_down.GetCounter() + 1 + counter_b) % b_size];
    // проверка на угол
    if (compare_angle(next_point_a - current_point_a,
                      next_point_b - current_point_b)) {
      ++counter_a;
      ++counter;
      minkovski_sum[counter] =
          minkovski_sum[counter - 1] + next_point_a - current_point_a;
    } else {
      ++counter_b;
      ++counter;
      minkovski_sum[counter] =
          minkovski_sum[counter - 1] + next_point_b - current_point_b;
    }
  }
  return minkovski_sum;
}

// проверка на то, что внутри фигуры есть начало координат
bool ContainsCenter(vector<CPoint> figure) {
  // проходимся по фигуре и прверяем, что каждое ребро "хорошое"
  for (int i = 0; i < figure.size(); ++i) {
    const auto current_point = figure[i];
    const auto next_point = figure[(i + 1) % figure.size()];
    const double alpha = ((current_point.GetX() * next_point.GetY()) -
                          (current_point.GetY() * next_point.GetX())) /
                         ((next_point - current_point).Length() *
                          (next_point - current_point).Length());
    // basic - основание перпендикуляра из начала координат на текущее ребро
    const auto basic =
        CPoint(alpha * (current_point.GetY() - next_point.GetY()),
               alpha * (next_point.GetX() - current_point.GetX()), -1);
    // проверяем, что векторное произведение текущего ребра и ребра между 0 и
    // основанием перпендикуляра будет > 0 в противном случае, центр координат
    // не пренадлежит фигуре
    if ((next_point.GetX() - current_point.GetX()) * (basic.GetY()) >
        (next_point.GetY() - current_point.GetY()) * (basic.GetX())) {
      return false;
    }
  }
  return true;
}

int main() {
  int a_figure_size, b_figure_size;
  cin >> a_figure_size;
  vector<CPoint> a_figure(a_figure_size);
  for (int i = 0; i < a_figure_size; ++i) {
    double x, y;
    cin >> x >> y;
    a_figure[i] = CPoint(x, y, i);
  }
  cin >> b_figure_size;
  vector<CPoint> b_figure(b_figure_size);
  for (int i = 0; i < b_figure_size; ++i) {
    double x, y;
    cin >> x >> y;
    // отразим фигуру b относительно центра координат
    b_figure[i] = CPoint(-x, -y, i);
  }
  // пересечение фигур a и b равносильно тому, что центр координат лежит в сумма
  // минковского фигур a и -b
  const auto sum = MinkovskiSum(a_figure, b_figure);
  // проверяем, что центр координат пренадлежит сумме
  if (ContainsCenter(sum)) {
    cout << "YES";
  } else {
    cout << "NO";
  }
}
