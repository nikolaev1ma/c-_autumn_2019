#include <iostream>
#include <math.h>

using std::cin;
using std::cout;

class CPoint {
public:
  CPoint(const double x_, const double y_, double z_) : x(x_), y(y_), z(z_) {}

  double GetX() const {
    return x;
  }

  double GetY() const {
    return y;
  }

  double GetZ() const {
    return z;
  }


private:
  double x;
  double y;
  double z;

};

double Distance(const CPoint &a, const CPoint &b) {
  return sqrt((a.GetX() - b.GetX()) * (a.GetX() - b.GetX()) + (a.GetY() - b.GetY()) * (a.GetY() - b.GetY()) +
              (a.GetZ() - b.GetZ()) * (a.GetZ() - b.GetZ()));
}

class CVector {
public:
  CVector(double x1_, double x2_, double y1_, double y2_, double z1_, double z2_) : x(x2_ - x1_), y(y2_ - y1_),
                                                                                    z(z2_ - z1_) {}

  CVector(const CPoint &a, const CPoint &b) : x(b.GetX() - a.GetX()), y(b.GetY() - a.GetY()), z(b.GetZ() - a.GetZ()) {}

  CVector(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

  double GetX() const {
    return x;
  }

  double GetY() const {
    return y;
  }

  double GetZ() const {
    return z;
  }

  double Norm() const {
    return Distance(CPoint(0, 0, 0), CPoint(x, y, z));
  }

  CVector operator-() const {
    return CVector(-x, -y, -z);
  }

  CVector operator+(const CVector &a) const {
    return CVector(x + a.GetX(), y + a.GetY(), z + a.GetZ());
  }

  CVector operator-(const CVector &a) const {
    return this->operator+(-a);
  }

  //умножение на скаляр
  CVector operator*(double k) {
    return CVector(x * k, y * k, z * k);
  }

  //скалярное произведение
  double DotProduct(const CVector &a) const {
    return x * a.GetX() + y * a.GetY() + z * a.GetZ();
  }

  //угол
  double corner(const CVector &a) const {
    return DotProduct(a) / (a.Norm() * Norm());
  }

  //векторное произведение
  CVector CrossProduct(const CVector &a) {
    return CVector(y * a.GetZ() - z * a.GetY(), z * a.GetX() - x * a.GetZ(), x * a.GetY() - y * a.GetX());
  }
  
private:
  double x;
  double y;
  double z;
};

