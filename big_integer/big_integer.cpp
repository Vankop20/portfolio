#include "big_integer.hpp"

BigInt BigInt::Absolut(const BigInt& n) {
  BigInt res = n;
  res.negative_ = false;
  return res;
}

void BigInt::Normal() {
  for (size_t i = num_.size(); i > 0; i--) {
    if (num_[i - 1] == 0) {
      num_.pop_back();
    } else {
      break;
    }
  }
  if (num_.empty()) {
    negative_ = false;
    num_.push_back(0);
  }
}

void BigInt::NormalPoz() {
  size_t size = num_.size() - 1;
  for (size_t i = 0; i < size; i++) {
    num_[i + 1] += num_[i] / kBase;
    num_[i] %= kBase;
  }
  if (num_[size] >= kBase) {
    num_.push_back(num_[size] / kBase);
    num_[size] %= kBase;
  }
}

void BigInt::NormalNeg() {
  size_t size = num_.size() - 1;
  for (size_t i = 0; i < size; i++) {
    if (num_[i] < 0) {
      --num_[i + 1];
      num_[i] += kBase;
    }
  }
  Normal();
}

BigInt& BigInt::Minus(const BigInt& number) {
  for (size_t i = 0; i < number.num_.size(); i++) {
    num_[i] -= number.num_[i];
  }
  NormalNeg();
  return *this;
}

void BigInt::MultiplyBase() {
  BigInt number = *this;
  number.num_.push_back(0);
  for (size_t i = number.num_.size() - 1; i > 0; i--) {
    number.num_[i] = number.num_[i - 1];
  }
  number.num_[0] = 0;
  *this = number;
}

BigInt BigInt::Dev2(const BigInt& n) const {
  BigInt number = n;
  for (size_t i = number.num_.size(); i > 0; i--) {
    if (number.num_[i - 1] % 2 == 1 && i > 1) {
      number.num_[i - 2] += kBase;
    }
    number.num_[i - 1] /= 2;
  }
  return number;
}

BigInt BigInt::BinSearchDiv(BigInt x, BigInt y) {
  BigInt start = 0;
  BigInt end = x;
  BigInt dev = start + Dev2(end - start);
  BigInt z = x - y;
  while (end - start > 0) {
    if (dev * y <= x && dev * y > z) {
      return dev;
    }
    if (dev * y > x) {
      end = dev;
    } else {
      start = dev + 1;
    }
    dev = start + Dev2(end - start);
  }
  return dev;
}

BigInt::BigInt(const std::vector<unsigned long long>& num) {
  for (size_t i = 0; i < num.size(); i++) {
    num_.push_back(num[i]);
  }
}

BigInt::BigInt(int64_t num) {
  if (num == 0) {
    num_.push_back(0);
  } else if (num < 0) {
    negative_ = true;
    int64_t min = 1;
    min = min << kBits;
    if (num == min) {
      unsigned long long positivenum = 1;
      positivenum = positivenum << kBits;
      while (positivenum != 0) {
        unsigned long long res = positivenum % kBase;
        num_.push_back(res);
        positivenum /= kBase;
      }
      return;
    }
    num = -num;
  }
  while (num != 0) {
    unsigned long long res = num % kBase;
    num_.push_back(res);
    num /= kBase;
  }
}

BigInt::BigInt(const std::string& str1) {
  std::string str = str1;
  if (!str1.empty()) {
    long long i = str1.size();
    if (str1[0] == '-') {
      negative_ = (str1[1] != '0');
      str = str1.substr(1, i);
      --i;
    }
    for (; i > kDeg; i -= kDeg) {
      std::string str2 = str.substr(i - kDeg, kDeg);
      long long val = stoll(str2);
      num_.push_back(val);
    }
    if (i >= 0) {
      std::string str2 = str.substr(0, i);
      long long val = stoll(str2);
      num_.push_back(val);
    }
  }
}

BigInt::BigInt(const BigInt& number) {
  negative_ = number.negative_;
  for (size_t i = 0; i < number.num_.size(); i++) {
    num_.push_back(number.num_[i]);
  }
}

BigInt& BigInt::operator=(const BigInt& number) {
  if (*this == number) {
    return *this;
  }
  num_.clear();
  for (size_t i = 0; i < number.num_.size(); i++) {
    num_.push_back(number.num_[i]);
  }
  negative_ = number.negative_;
  return *this;
}

bool BigInt::operator==(const BigInt& number) const {
  if (negative_ != number.negative_) {
    return false;
  }
  if (num_.size() != number.num_.size()) {
    return false;
  }
  for (size_t i = num_.size(); i > 0; --i) {
    if (num_[i - 1] != number.num_[i - 1]) {
      return false;
    }
  }
  return true;
}

bool BigInt::operator!=(const BigInt& number) const {
  return !(*this == number);
}

bool BigInt::operator<(const BigInt& number) const {
  if (negative_ != number.negative_) {
    return negative_ && !number.negative_;
  }
  if (num_.size() != number.num_.size()) {
    if (negative_) {
      return num_.size() > number.num_.size();
    }
    return num_.size() < number.num_.size();
  }
  for (size_t i = num_.size(); i > 0; --i) {
    if (num_[i - 1] != number.num_[i - 1]) {
      if (negative_) {
        return (num_[i - 1] > number.num_[i - 1]);
      }
      return (num_[i - 1] < number.num_[i - 1]);
    }
  }
  return false;
}

bool BigInt::operator>(const BigInt& number) const { return number < *this; }

bool BigInt::operator>=(const BigInt& number) const {
  return !(*this < number);
}

bool BigInt::operator<=(const BigInt& number) const {
  return !(*this > number);
}

BigInt& BigInt::operator+=(const BigInt& number) {
  if (number.negative_ != negative_) {
    BigInt sum = number;
    if (number.negative_) {
      sum.negative_ = false;
      *this -= sum;
      return *this;
    }
    sum += *this;
    *this = sum;
    return *this;
  }
  size_t size = number.num_.size();
  if (num_.size() < number.num_.size()) {
    num_.resize(size, 0);
  }
  for (size_t i = 0; i < size; i++) {
    num_[i] += number.num_[i];
  }
  NormalPoz();
  return *this;
}

BigInt BigInt::operator+(const BigInt& number) const {
  BigInt sum = *this;
  sum += number;
  return sum;
}

BigInt& BigInt::operator-=(const BigInt& number) {
  if (number.negative_ || negative_) {
    if (number.negative_) {
      BigInt sum = number;
      sum.negative_ = false;
      *this += sum;
      return *this;
    }
    BigInt sum = number;
    sum.negative_ = true;
    *this += sum;
    return *this;
  }
  if (*this >= number) {
    return Minus(number);
  }
  BigInt x = *this;
  BigInt y = number;
  y -= x;
  y.negative_ = true;
  *this = y;
  return *this;
}

BigInt BigInt::operator-(const BigInt& number) const {
  BigInt sum = *this;
  sum -= number;
  return sum;
}

BigInt& BigInt::operator++() {
  *this += 1;
  return *this;
}

BigInt BigInt::operator++(int) {
  BigInt copy = *this;
  *this += 1;
  return copy;
}

BigInt& BigInt::operator--() {
  *this -= 1;
  return *this;
}

BigInt BigInt::operator--(int) {
  BigInt copy = *this;
  *this -= 1;
  return copy;
}

BigInt BigInt::operator-() {
  if (num_.size() == 1 && num_[0] == 0) {
    return *this;
  }
  negative_ = true;
  return *this;
}

BigInt& BigInt::operator*=(const BigInt& number) {
  BigInt res = 0;
  BigInt x = *this;
  x.negative_ = false;
  for (size_t i = 0; i < number.num_.size(); i++) {
    BigInt y = x;
    for (size_t j = 0; j < y.num_.size(); j++) {
      y.num_[j] *= number.num_[i];
    }
    y.NormalPoz();
    res += y;
    x.MultiplyBase();
  }
  res.negative_ = number.negative_ != negative_;
  *this = res;
  Normal();
  return *this;
}

BigInt BigInt::operator*(const BigInt& number) const {
  BigInt n = *this;
  return n *= number;
}

BigInt& BigInt::operator/=(const BigInt& number) {
  if (number == 0) {
    std::cerr << "UB";
    return *this;
  }
  if (Absolut(*this) < Absolut(number)) {
    negative_ = false;
    *this = 0;
    return *this;
  }
  if (number == 1 || number == -1) {
    negative_ = number.negative_ != negative_;
    return *this;
  }
  BigInt x = *this;
  x.negative_ = false;
  BigInt y = number;
  y.negative_ = false;
  if (x == y) {
    *this = 1;
    negative_ = number.negative_ != negative_;
    return *this;
  }
  BigInt res = BinSearchDiv(x, y);
  res.negative_ = number.negative_ != negative_;
  *this = res;
  return *this;
}

BigInt BigInt::operator/(const BigInt& number) const {
  BigInt div = *this;
  div /= number;
  return div;
}

BigInt& BigInt::operator%=(const BigInt& number) {
  BigInt mod = *this / number;
  mod *= number;
  *this -= mod;
  return *this;
}

BigInt BigInt::operator%(const BigInt& number) const {
  BigInt mod = *this;
  return mod %= number;
}

std::ostream& operator<<(std::ostream& out, const BigInt& number) {
  if (number.negative_) {
    out << "-";
  }
  int i = number.num_.size() - 1;
  out << number.num_[i];
  i--;
  for (; i >= 0; i--) {
    long long base = number.kBase / number.kTen;
    while (base > number.num_[i] + 1) {
      out << 0;
      base /= number.kTen;
    }
    out << number.num_[i];
  }
  return out;
}

std::istream& operator>>(std::istream& in, BigInt& number) {
  std::string str;
  in >> str;
  BigInt buf(str);
  number = buf;
  return in;
}
