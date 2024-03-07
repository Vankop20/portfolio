#pragma once
#include <iostream>
#include <string>
#include <vector>

class BigInt {
 private:
  std::vector<long long> num_;
  bool negative_ = false;
  const long long kBase = 1e9;
  const long long kTen = 10;
  const long long kDeg = 9;
  const int kBits = 63;
  static BigInt Absolut(const BigInt& n);
  void Normal();
  void NormalPoz();
  void NormalNeg();
  BigInt& Minus(const BigInt& number);
  void MultiplyBase();
  BigInt Dev2(const BigInt& n) const;
  BigInt BinSearchDiv(BigInt x, BigInt y);

 public:
  BigInt() = default;
  BigInt(const std::vector<unsigned long long>& num);
  BigInt(int64_t num);
  BigInt(const std::string& str1);
  BigInt(const BigInt& number);
  BigInt& operator=(const BigInt& number);
  bool operator==(const BigInt& number) const;
  bool operator!=(const BigInt& number) const;
  bool operator<(const BigInt& number) const;
  bool operator>(const BigInt& number) const;
  bool operator>=(const BigInt& number) const;
  bool operator<=(const BigInt& number) const;
  BigInt& operator+=(const BigInt& number);
  BigInt operator+(const BigInt& number) const;
  BigInt& operator-=(const BigInt& number);
  BigInt operator-(const BigInt& number) const;
  BigInt& operator++();
  BigInt operator++(int);
  BigInt& operator--();
  BigInt operator--(int);
  BigInt operator-();
  BigInt& operator*=(const BigInt& number);
  BigInt operator*(const BigInt& number) const;
  BigInt& operator/=(const BigInt& number);
  BigInt operator/(const BigInt& number) const;
  BigInt& operator%=(const BigInt& number);
  BigInt operator%(const BigInt& number) const;
  friend std::ostream& operator<<(std::ostream& out, const BigInt& number);
  friend std::istream& operator>>(std::istream& in, BigInt& number);
  ~BigInt() = default;
};
