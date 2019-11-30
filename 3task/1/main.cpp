#include <iostream>
#include <limits>
#include <math.h>

using std::cin;
using std::cout;

double eps = 1e-9999;
const double max_double = std::numeric_limits<double>::max();

class CPoint {
public:
  CPoint(double x, double y, double z) : x_(x), y_(y), z_(z) {}

  CPoint(const CPoint &a_) : x_(a_.GetX()), y_(a_.GetY()), z_(a_.GetZ()) {}

  double GetX() const { return x_; }

  double GetY() const { return y_; }

  double GetZ() const { return z_; }

private:
  double x_;
  double y_;
  double z_;
};

// растояние между 2 точками
double Distance(const CPoint &a, const CPoint &b) {
  return sqrt((a.GetX() - b.GetX()) * (a.GetX() - b.GetX()) +
              (a.GetY() - b.GetY()) * (a.GetY() - b.GetY()) +
              (a.GetZ() - b.GetZ()) * (a.GetZ() - b.GetZ()));
}

// унарный минус
CPoint operator-(const CPoint &a) {
  return CPoint(-a.GetX(), -a.GetY(), -a.GetZ());
}

// векторное сложение
CPoint operator+(const CPoint &a, const CPoint &b) {
  return CPoint(a.GetX() + b.GetX(), a.GetY() + b.GetY(), a.GetZ() + b.GetZ());
}

// векторная разница
CPoint operator-(const CPoint &a, const CPoint &b) { return a + (-b); }

// равенство
bool operator==(const CPoint &a, const CPoint &b) {
  if (fabs(a.GetX() - b.GetX()) <= eps && fabs(a.GetY() - b.GetY()) <= eps &&
      fabs(a.GetZ() - b.GetZ()) <= eps) {
    return true;
  }
  return false;
}

// умножение на константу
CPoint operator*(const CPoint &a, const double k) {
  return CPoint(a.GetX() * k, a.GetY() * k, a.GetZ() * k);
}

double operator*(const CPoint &a, const CPoint &b) {
  return (a.GetX() * b.GetX()) + (a.GetY() * b.GetY()) + (a.GetZ() * b.GetZ());
}

// считаем общий перпендикуляр между ab и cd, при условии, что он есть
double CommonPerpendicular(const CPoint a, const CPoint b, const CPoint c,
                           const CPoint d) {
  const auto p = a;
  const auto q = b - a;
  const auto r = c;
  const auto s = d - c;

  // хотим найти точки на отрезках ab и cd, такие что отрезок между ними будет
  // являтся их общим перпендикуляром alpha и beta это то, в каком отношении
  // данные точки делят данные отрезки соответсвенно
  const auto alpha = ((s * p - s * r) * (q * s) + (q * r - q * p) * (s * s)) /
                     ((s * s) * (q * q) - (q * s) * (q * s));
  const auto beta = (s * q * alpha + s * p - s * r) / (s * s);

  const auto u = p + q * alpha;
  const auto v = r + s * beta;

  // если alpha и beta лежат в отрезке от 0 до 1, то точки u, v лежат на отрезке
  // и общий перпендикуляр имеет смысл рассматривать
  if (alpha >= -eps && alpha <= 1 + eps && beta >= -eps && beta <= 1 + eps) {
    return Distance(u, v);
  }
  return max_double;
}

// считаем минимальное растояние от точки x до отрезка mn
double Perpendicular(const CPoint x, const CPoint m, const CPoint n) {
  // если m и n это одна точка, то растояние будет xm
  if (m == n) {
    return Distance(x, m);
  }
  // ищем перпендикуляр из x на mn
  CPoint u = x - m;
  CPoint v = n - m;
  double alpha = (u * v) / (v * v);
  // если alpha от 0 до 1, то перпендикуляр падает на отрезок и заодно является
  // самым минимальным растоянием
  if (alpha >= (-eps) && alpha <= 1 + eps) {
    CPoint basic = m + (n - m) * alpha;
    return Distance(basic, x);
  }
  // если же не падает, то минимум это минимум из xm и xn
  const auto dis1 = Distance(x, m);
  const auto dis2 = Distance(x, n);
  return std::min(dis1, dis2);
}

// считаем минимум между 2 отрезками ab и cd
double MinDistant(CPoint a, CPoint b, CPoint c, CPoint d) {
  // считаем 4 потенциально минимальных растояния
  const auto dis1 = Perpendicular(a, c, d);
  const auto dis2 = Perpendicular(b, c, d);
  const auto dis3 = Perpendicular(c, a, b);
  const auto dis4 = Perpendicular(d, a, b);
  const auto x1 = (b - a).GetX();
  const auto y1 = (b - a).GetY();
  const auto z1 = (b - a).GetZ();
  const auto x2 = (d - c).GetX();
  const auto y2 = (d - c).GetY();
  const auto z2 = (d - c).GetZ();
  bool degenerate_case = true;
  // далее проверяем, являются ли отрезки вырожденным случаем: точки совпадают
  // или отрезки параллельны
  if (fabs(x1 * y2 - x2 * y1) <= eps && fabs(x1 * z2 - x2 * z1) <= eps &&
      fabs(y1 * z2 - y2 * z1) <= eps) {
    degenerate_case = false;
  }
  if (a == b) {
    degenerate_case = false;
  }
  if (c == d) {
    degenerate_case = false;
  }
  auto dis5 = max_double;
  // если у нас не вырожденный случай, то мы может посчитать общий перпендикуляр
  if (degenerate_case) {
    dis5 = CommonPerpendicular(a, b, c, d);
  }
  // далее просто находим минимум из 5 значений
  double min_distance = dis5;
  if (dis4 <= min_distance + eps) {
    min_distance = dis4;
  }
  if (dis3 <= min_distance + eps) {
    min_distance = dis3;
  }
  if (dis2 <= min_distance + eps) {
    min_distance = dis2;
  }
  if (dis1 <= min_distance + eps) {
    min_distance = dis1;
  }
  return min_distance;
}

int main() {
  std::cout.precision(12);
  int x1, y1, z1;
  cin >> x1 >> y1 >> z1;
  CPoint a(x1, y1, z1);
  int x2, y2, z2;
  cin >> x2 >> y2 >> z2;
  CPoint b(x2, y2, z2);
  int x3, y3, z3;
  cin >> x3 >> y3 >> z3;
  CPoint c(x3, y3, z3);
  int x4, y4, z4;
  cin >> x4 >> y4 >> z4;
  CPoint d(x4, y4, z4);
  cout << MinDistant(a, b, c, d);
}
