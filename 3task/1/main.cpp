#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <vector>

using std::cin;
using std::cout;
using std::vector;

const double eps = std::numeric_limits<double>::epsilon();
const double max_double = std::numeric_limits<double>::max();

struct CPoint {
public:
  CPoint(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

  double x;
  double y;
  double z;
};

// растояние между 2 точками
double Distance(const CPoint &a, const CPoint &b) {
  return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) +
                   (a.z - b.z) * (a.z - b.z));
}

// унарный минус
CPoint operator-(const CPoint &a) { return CPoint(-a.x, -a.y, -a.z); }

// векторное сложение
CPoint operator+(const CPoint &a, const CPoint &b) {
  return CPoint(a.x + b.x, a.y + b.y, a.z + b.z);
}

// векторная разница
CPoint operator-(const CPoint &a, const CPoint &b) { return a + (-b); }

// равенство
bool operator==(const CPoint &a, const CPoint &b) {
  if (std::fabs(a.x - b.x) <= eps && std::fabs(a.y - b.y) <= eps &&
      std::fabs(a.z - b.z) <= eps) {
    return true;
  }
  return false;
}

// умножение на константу
CPoint operator*(const CPoint &a, const double k) {
  return CPoint(a.x * k, a.y * k, a.z * k);
}

double operator*(const CPoint &a, const CPoint &b) {
  return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
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
double Perpendicular(const CPoint &x, const CPoint &m, const CPoint &n) {
  // если m и n это одна точка, то растояние будет xm
  if (m == n) {
    return Distance(x, m);
  }
  // ищем перпендикуляр из x на mn
  const auto u = x - m;
  const auto v = n - m;
  const double alpha = (u * v) / (v * v);
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
double Distance(const CPoint &a, const CPoint &b, const CPoint &c,
                const CPoint &d) {
  // считаем 4 потенциально минимальных растояния
  vector<double> potential_distance = {
      Perpendicular(a, c, d), Perpendicular(b, c, d), Perpendicular(c, a, b),
      Perpendicular(d, a, b)};
  const auto x1 = (b - a).x;
  const auto y1 = (b - a).y;
  const auto z1 = (b - a).z;
  const auto x2 = (d - c).x;
  const auto y2 = (d - c).y;
  const auto z2 = (d - c).z;
  bool degenerate_case = false;
  // далее проверяем, являются ли отрезки вырожденным случаем: точки совпадают
  // или отрезки параллельны
  if (std::fabs(x1 * y2 - x2 * y1) <= eps &&
      std::fabs(x1 * z2 - x2 * z1) <= eps &&
      std::fabs(y1 * z2 - y2 * z1) <= eps) {
    degenerate_case = true;
  }
  if (a == b) {
    degenerate_case = true;
  }
  if (c == d) {
    degenerate_case = true;
  }
  // если у нас не вырожденный случай, то мы может посчитать общий перпендикуляр
  if (!degenerate_case) {
    potential_distance.emplace_back(CommonPerpendicular(a, b, c, d));
  }
  // далее просто находим минимум из 5 значений
  const auto result =
      std::min_element(potential_distance.begin(), potential_distance.end());
  return *result;
}

int main() {
  std::cout.precision(12);
  int x1, y1, z1;
  cin >> x1 >> y1 >> z1;
  const CPoint a(x1, y1, z1);
  int x2, y2, z2;
  cin >> x2 >> y2 >> z2;
  const CPoint b(x2, y2, z2);
  int x3, y3, z3;
  cin >> x3 >> y3 >> z3;
  const CPoint c(x3, y3, z3);
  int x4, y4, z4;
  cin >> x4 >> y4 >> z4;
  const CPoint d(x4, y4, z4);
  cout << Distance(a, b, c, d) << std::endl;
  return 0;
}
