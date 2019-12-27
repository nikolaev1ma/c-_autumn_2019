#ifndef C__AUTUMN_2019_BIGINTEGER_H
#define C__AUTUMN_2019_BIGINTEGER_H

#include <iostream>
#include <string>
#include <vector>

using std::cin;
using std::cout;
using std::istream;
using std::ostream;
using std::string;
using std::vector;

class BigInteger {
public:
  BigInteger() { *this = BigInteger(0); }

  BigInteger(long long value);

  BigInteger(const BigInteger &other);

  BigInteger(const string &str);

  BigInteger &operator=(long long value);

  BigInteger &operator=(const BigInteger &other);

  BigInteger operator++();

  BigInteger operator++(int);

  BigInteger operator--();

  BigInteger operator--(int);

  string toString() const;

  explicit operator bool();

  friend ostream &operator<<(ostream &os, const BigInteger &bg);

  friend istream &operator>>(istream &is, BigInteger &bg);

  friend BigInteger operator+(const BigInteger &a, const BigInteger &b);

  BigInteger operator+=(const BigInteger &other);

  friend BigInteger operator-(const BigInteger &a, const BigInteger &b);

  BigInteger operator-=(const BigInteger &other);

  friend BigInteger operator-(const BigInteger &a);

  friend BigInteger operator*(const BigInteger &a, const BigInteger &b);

  BigInteger operator*=(const BigInteger &other);

  friend BigInteger operator/(const BigInteger &a, const BigInteger &b);

  BigInteger operator/=(const BigInteger &other);

  friend BigInteger operator%(const BigInteger &a, const BigInteger &b);

  BigInteger operator%=(const BigInteger &other);

  friend bool operator==(const BigInteger &a, const BigInteger &b);

  friend bool operator!=(const BigInteger &a, const BigInteger &b);

  friend bool operator<(const BigInteger &a, const BigInteger &b);

  friend bool operator>(const BigInteger &a, const BigInteger &b);

  friend bool operator<=(const BigInteger &a, const BigInteger &b);

  friend bool operator>=(const BigInteger &a, const BigInteger &b);

private:
  vector<int> digit_arr;
  long long size;
  long long sign;

  BigInteger modul() const;

  BigInteger MULT10(long long k) const;

  BigInteger TYPIFICATION();
};

BigInteger::BigInteger(const BigInteger &other) {
  size = other.size;
  sign = other.sign;
  digit_arr = other.digit_arr;
}

BigInteger::BigInteger(const string &str) {
  if ((int)str.size() == 0) {
    *this = BigInteger(0);
  } else {
    long long i = 0;
    if (str[0] == '-') {
      sign = -1;
      i = 1;
    } else {
      sign = 1;
    }
    for (; i < (int)str.size(); ++i) {
      digit_arr.emplace_back(str[i] - '0');
    }
  }
  size = digit_arr.size();
}

BigInteger &BigInteger::operator=(long long value) {
  *this = BigInteger(value);
  return *this;
}

BigInteger &BigInteger::operator=(const BigInteger &other) {
  size = other.size;
  sign = other.sign;
  digit_arr = other.digit_arr;
  return *this;
}

BigInteger BigInteger::modul() const {
  BigInteger res = *this;
  res.sign = 1;
  return res;
}

BigInteger::BigInteger(long long value) {
  if (value < 0) {
    sign = -1;
    value = -value;
  } else {
    sign = 1;
  }
  if (value == 0) {
    digit_arr = {0};
    size = 1;
  } else {
    while (value != 0) {
      long long digit = value % 10;
      value /= 10;
      digit_arr.emplace_back(digit);
    }
    size = digit_arr.size();
    vector<int> reverse(size);
    for (long long i = 0; i < size; ++i) {
      reverse[size - 1 - i] = digit_arr[i];
    }
    digit_arr = reverse;
  }
}

bool operator==(const BigInteger &a, const BigInteger &b) {
  if (a.digit_arr[0] == 0 && b.digit_arr[0] == 0) {
    return true;
  }
  bool simple_flag = (a.sign == b.sign && a.size == b.size);
  if (!simple_flag) {
    return false;
  }
  for (long long i = 0; i < a.size; ++i) {
    if (a.digit_arr[i] != b.digit_arr[i]) {
      return false;
    }
  }
  return true;
}

bool operator!=(const BigInteger &a, const BigInteger &b) { return !(a == b); }

bool operator<(const BigInteger &a, const BigInteger &b) {
  if (a.digit_arr[0] == 0 && b.digit_arr[0] == 0) {
    return false;
  }
  if (a.sign < b.sign) {
    return true;
  }
  if (a.sign > b.sign) {
    return false;
  }
  if (a.sign > 0) {
    if (a.size < b.size) {
      return true;
    }
    if (a.size > b.size) {
      return false;
    }
    for (long long i = 0; i < a.size; ++i) {
      if (a.digit_arr[i] < b.digit_arr[i]) {
        return true;
      }
      if (a.digit_arr[i] > b.digit_arr[i]) {
        return false;
      }
    }
    return false;
  }
  if (a.size < b.size) {
    return false;
  }
  if (a.size > b.size) {
    return true;
  }
  for (long long i = 0; i < a.size; ++i) {
    if (a.digit_arr[i] > b.digit_arr[i]) {
      return true;
    }
    if (a.digit_arr[i] < b.digit_arr[i]) {
      return false;
    }
  }
  return false;
}

bool operator<=(const BigInteger &a, const BigInteger &b) {
  return ((a == b) || (a < b));
}

bool operator>(const BigInteger &a, const BigInteger &b) { return !(a <= b); }

bool operator>=(const BigInteger &a, const BigInteger &b) {
  return ((a == b) || (a > b));
}

BigInteger operator+(const BigInteger &a, const BigInteger &b) {
  BigInteger sum = BigInteger();
  sum.digit_arr.clear();
  sum.size = 0;
  if (a.sign == b.sign) {
    long long max_size = std::max(a.size, b.size) + 1;
    vector<int> tmp_sum(max_size, 0);
    vector<int> tmp1 = a.digit_arr;
    vector<int> tmp2 = b.digit_arr;
    tmp1.insert(tmp1.begin(), max_size - tmp1.size() - 1, 0);

    tmp2.insert(tmp2.begin(), max_size - tmp2.size() - 1, 0);
    for (long long i = max_size - 2; i >= 0; --i) {
      long long number_sum = tmp1[i] + tmp2[i];
      tmp_sum[i + 1] += number_sum;
      if (tmp_sum[i + 1] > 9) {
        tmp_sum[i + 1] -= 10;
        tmp_sum[i] = 1;
      }
    }
    if (tmp_sum[0] == 0) {
      tmp_sum.erase(tmp_sum.begin());
      --max_size;
    }
    sum.digit_arr = tmp_sum;
    sum.size = tmp_sum.size();
    sum.sign = a.sign;
    // cout << sum;
    sum.TYPIFICATION();
    return sum;
  }
  sum = a - (-b);
  return sum;
}

BigInteger operator-(const BigInteger &a) {
  BigInteger result = a;
  result.sign = -result.sign;
  return result;
}

BigInteger operator-(const BigInteger &a, const BigInteger &b) {
  // cout << "sosy";
  if (a.sign != b.sign) {
    return a + (-b);
  }
  BigInteger sum;
  sum.digit_arr.clear();
  sum.size = 0;
  long long max_size = std::max(a.size, b.size);
  vector<int> tmp_sum(max_size, 0);
  vector<int> tmp1 = a.digit_arr;
  vector<int> tmp2 = b.digit_arr;
  tmp1.insert(tmp1.begin(), max_size - tmp1.size(), 0);
  tmp2.insert(tmp2.begin(), max_size - tmp2.size(), 0);
  if (a.modul() >= b.modul()) {
    for (long long i = max_size - 1; i >= 0; --i) {
      long long number_sum = tmp1[i] - tmp2[i];
      tmp_sum[i] += number_sum;
      if (tmp_sum[i] < 0) {
        tmp_sum[i] += 10;
        tmp_sum[i - 1] = -1;
      }
    }
  } else {
    for (long long i = max_size - 1; i >= 0; --i) {
      long long number_sum = tmp2[i] - tmp1[i];
      tmp_sum[i] += number_sum;
      if (tmp_sum[i] < 0) {
        tmp_sum[i] += 10;
        tmp_sum[i - 1] = -1;
      }
    }
  }
  if (a >= b) {
    sum.sign = 1;
  } else {
    sum.sign = -1;
  }
  //уменьшение
  long long counter = 0;
  while (counter < max_size && tmp_sum[counter] == 0) {
    ++counter;
  }
  if (counter == max_size) {
    sum.digit_arr = {0};
    sum.size = 1;
  } else {
    tmp_sum.erase(tmp_sum.begin(), tmp_sum.begin() + counter);
    sum.digit_arr = tmp_sum;
    sum.size = tmp_sum.size();
  }
  sum.TYPIFICATION();
  return sum;
}

BigInteger BigInteger::operator++() {
  *this = *this + BigInteger(1);
  return *this;
}

BigInteger BigInteger::operator++(int) {
  BigInteger tmp = *this;
  ++*this;
  return tmp;
}

BigInteger BigInteger::operator--() {
  *this = *this - BigInteger(1);
  return *this;
}

BigInteger BigInteger::operator--(int) {
  BigInteger tmp = *this;
  --*this;
  return tmp;
}

BigInteger operator*(const BigInteger &a, const BigInteger &b) {
  if (a.size == 1) {
    long long x = a.digit_arr[0];
    BigInteger mult = 0;
    while (x != 0) {
      mult += b;
      --x;
    }
    mult.sign = a.sign * b.sign;
    return mult;
  }
  if (b.size == 1) {
    long long y = b.digit_arr[0];
    BigInteger mult(0);
    while (y != 0) {
      mult += a;
      --y;
    }
    mult.sign = a.sign * b.sign;
    return mult;
  }
  BigInteger a1, a2, b1, b2;
  a1.digit_arr.clear();
  a1.size = 0;
  a2.digit_arr.clear();
  a2.size = 0;
  b1.digit_arr.clear();
  b1.size = 0;
  b2.digit_arr.clear();
  b2.size = 0;
  a1.sign = 1;
  a2.sign = 1;
  b1.sign = 1;
  b2.sign = 1;
  long long n = std::min(a.size, b.size);
  a1.digit_arr = {a.digit_arr.begin(), a.digit_arr.begin() + a.size - (n / 2)};
  a2.digit_arr = {a.digit_arr.begin() + a.size - (n / 2), a.digit_arr.end()};
  b1.digit_arr = {b.digit_arr.begin(), b.digit_arr.begin() + b.size - (n / 2)};
  b2.digit_arr = {b.digit_arr.begin() + b.size - (n / 2), b.digit_arr.end()};
  a1.size = a1.digit_arr.size();
  a2.size = a2.digit_arr.size();
  b1.size = b1.digit_arr.size();
  b2.size = b2.digit_arr.size();
  a1.TYPIFICATION();
  a2.TYPIFICATION();
  b1.TYPIFICATION();
  b2.TYPIFICATION();
  // cout << a1 << " " << a2 << " " << b1 << " " << b2 << '\n';
  BigInteger sum1 = a1 * b1;
  BigInteger sum2 = a2 * b2;
  BigInteger sum3 = ((a1 + a2) * (b1 + b2));
  // cout << sum1 << " " << sum2 << " " << sum3 << '\n';
  auto mult =
      sum1.MULT10((n / 2) * 2) + (sum3 - sum1 - sum2).MULT10(n / 2) + sum2;
  mult.sign = a.sign * b.sign;
  mult.TYPIFICATION();
  return mult;
}

BigInteger BigInteger::MULT10(long long k) const {
  BigInteger res = *this;
  res.size += k;
  while (k != 0) {
    res.digit_arr.emplace_back(0);
    --k;
  }
  res.TYPIFICATION();
  return res;
}

BigInteger BigInteger::TYPIFICATION() {
  if (size == 0) {
    *this = BigInteger(0);
    return *this;
  }
  long long counter = 0;
  while (counter < size && digit_arr[counter] == 0) {
    ++counter;
  }
  if (counter == size) {
    *this = BigInteger(0);
    return *this;
  }
  digit_arr.erase(digit_arr.begin(), digit_arr.begin() + counter);
  size = digit_arr.size();
  return *this;
}

BigInteger operator/(const BigInteger &a, const BigInteger &b) {
  BigInteger additional, result;
  additional.digit_arr.clear();
  additional.size = 0;
  result.digit_arr.clear();
  result.size = 0;
  additional.sign = 1;
  result.sign = 1;
  if (a.modul() < b.modul()) {
    result = BigInteger(0);
    return result;
  } else {
    for (long long i = 0; i < b.size; ++i) {
      additional.digit_arr.emplace_back(a.digit_arr[i]);
    }
    additional.size = additional.digit_arr.size();
    long long counter = additional.size;
    while (true) {
      if (additional < b.modul()) {
        result.digit_arr.emplace_back(0);
        ++result.size;
        if (counter == a.size) {
          break;
        }
        additional.digit_arr.emplace_back(a.digit_arr[counter]);
        ++additional.size;
        if (additional.digit_arr[0] == 0) {
          additional.digit_arr.erase(additional.digit_arr.begin());
          --additional.size;
        }
        ++counter;
      } else {
        BigInteger tmp = b.modul();
        long long digit = 1;
        while (tmp + b.modul() <= additional) {
          tmp = tmp + b.modul();
          ++digit;
        }
        additional = additional - tmp;
        result.digit_arr.emplace_back(digit);
        ++result.size;
        if (counter == a.size) {
          break;
        }
        additional.digit_arr.emplace_back(a.digit_arr[counter]);
        ++additional.size;
        if (additional.digit_arr[0] == 0) {
          additional.digit_arr.erase(additional.digit_arr.begin());
          --additional.size;
        }
        ++counter;
      }
    }
  }
  result.sign = a.sign * b.sign;
  result.size = result.digit_arr.size();
  result.TYPIFICATION();
  return result;
}

BigInteger operator%(const BigInteger &a, const BigInteger &b) {
  BigInteger quotient = (a / b);
  BigInteger tmp = b * quotient;
  return (a - tmp);
}

string BigInteger::toString() const {
  BigInteger tmp = *this;
  tmp.TYPIFICATION();
  string result;
  if (tmp.sign == -1) {
    result = '-';
  }
  for (long long i = 0; i < size; ++i) {
    result += '0' + tmp.digit_arr[i];
  }
  return result;
}

BigInteger::operator bool() {
  return !((sign == 1) && (size == 1) && (digit_arr[0] == 0));
}

BigInteger BigInteger::operator%=(const BigInteger &other) {
  *this = *this % other;
  return *this;
}

BigInteger BigInteger::operator*=(const BigInteger &other) {
  *this = *this * other;
  return *this;
}

BigInteger BigInteger::operator+=(const BigInteger &other) {
  *this = *this + other;
  return *this;
}

BigInteger BigInteger::operator-=(const BigInteger &other) {
  *this = *this - other;
  return *this;
}

BigInteger BigInteger::operator/=(const BigInteger &other) {
  *this = *this / other;
  return *this;
}

ostream &operator<<(ostream &os, const BigInteger &bg) {
  os << bg.toString();
  return os;
}

istream &operator>>(istream &is, BigInteger &bg) {
  string value;
  // int value;
  is >> value;
  bg = BigInteger(value);
  return is;
}

#endif // C__AUTUMN_2019_BIGINTEGER_H
