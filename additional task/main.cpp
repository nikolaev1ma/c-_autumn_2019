#include <iostream>
#include <math.h>

using std::cin;
using std::cout;

class CPoint {
public:
  double x;
  double y;
  double z;

  CPoint(const double x_, const double y_, double z_);

  friend double distance(const CPoint &a, const CPoint &b);
};

CPoint::CPoint(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

double distance(const CPoint &a, const CPoint &b) {
  return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z));
}

class CVector {
public:
  double x;
  double y;
  double z;

  CVector(double x1_, double x2_, double y1_, double y2_, double z1_, double z2_);

  CVector(const CPoint &a, const CPoint &b);

  CVector(double x_, double y_, double z_);

  double length() const;

};

CVector::CVector(const CPoint &a, const CPoint &b) : x(b.x - a.x), y(b.y - a.y), z(b.z - a.z) {}

CVector::CVector(double x1_, double x2_, double y1_, double y2_, double z1_, double z2_) : x(x2_ - x1_), y(y2_ - y1_),
                                                                                           z(z2_ - z1_) {}

CVector::CVector(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

double CVector::length() const {
  return distance(CPoint(0, 0, 0), CPoint(x, y, z));
}

//cумма + нормализация
CVector operator+(const CVector &a, const CVector &b) {
  return CVector(a.x + b.x, a.y + b.y, a.z + b.z);
}

//разность + нормализация
CVector operator-(const CVector &a, const CVector &b) {
  return CVector(a.x - b.x, a.y - b.y, a.z - b.z);
}

CVector operator*(const CVector &a, int k) {
  return CVector(a.x * k, a.y * k, a.z * k);
}

//скалярное произведение
double DotProduct(const CVector &a, const CVector &b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

//угол
double corner(const CVector &a, const CVector &b) {
  return DotProduct(a, b) / (a.length() * b.length());
}

//векторное произведение
CVector CrossProduct(const CVector &a, const CVector &b) {
  return CVector(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

