#include "string.hpp"

#include <cstring>
#include <iostream>
#include <vector>

String::String() { string_ = nullptr; }

String::String(size_t size, char character) {
  if (size > 0) {
    size_++;
    while (size > size_) {
      size_ *= 2;
    }
  }
  string_ = new char[size_ + 1];
  for (size_t i = 0; i < size; i++) {
    string_[i] = character;
  }
  count_ = size;
  string_[count_] = '\0';
}

String::String(const char* str) {
  count_ = strlen(str);
  size_ = count_;
  string_ = new char[size_ + 1];
  for (size_t i = 0; i < count_; i++) {
    string_[i] = str[i];
  }
  string_[size_] = '\0';
}

String::String(const String& str) {
  count_ = str.Size();
  size_ = str.Capacity();
  string_ = new char[size_ + 1];
  for (size_t i = 0; i < count_; i++) {
    string_[i] = str[i];
  }
  string_[size_] = '\0';
}

String::~String() {
  delete[] string_;
  count_ = 0;
  size_ = 0;
}

void String::Clear() {
  count_ = 0;
  size_ = 0;
  string_[size_] = '\0';
}

void String::Reserve(size_t new_cap) {
  if (new_cap > size_) {
    char* string = new char[new_cap + 1];
    if (string_ != nullptr) {
      for (size_t i = 0; i <= count_; i++) {
        string[i] = string_[i];
      }
    }
    delete[] string_;
    string_ = string;
    size_ = new_cap;
  }
}

void String::PushBack(char character) {
  Reserve(count_ + 1);
  string_[count_] = character;
  ++count_;
  string_[count_] = '\0';
}

void String::PopBack() {
  if (count_ > 0) {
    count_--;
    string_[count_] = '\0';
  }
}

void String::Resize(size_t new_size) {
  if (new_size > size_) {
    if (size_ == 0) {
      size_++;
    }
    while (new_size > size_) {
      size_ *= 2;
    }
    char* string = new char[size_ + 1];
    if (string_ != nullptr) {
      for (size_t i = 0; i <= count_; i++) {
        string[i] = string_[i];
      }
    }
    delete[] string_;
    string_ = string;
  }
  count_ = new_size;
}

void String::Resize(size_t new_size, char character) {
  if (new_size > size_) {
    if (size_ == 0) {
      size_++;
    }
    while (new_size > size_) {
      size_ *= 2;
    }
    char* string = new char[size_ + 1];
    for (size_t i = 0; i <= count_; i++) {
      string[i] = string_[i];
    }
    delete[] string_;
    string_ = string;
  }
  if (new_size > count_) {
    for (size_t i = count_; i < new_size; i++) {
      string_[i] = character;
    }
  }
  count_ = new_size;
  string_[count_] = '\0';
}

void String::ShrinkToFit() {
  char* string = new char[count_ + 1];
  for (size_t i = 0; i <= count_; i++) {
    string[i] = string_[i];
  }
  delete[] string_;
  string_ = string;
  size_ = count_;
}

void String::Swap(String& other) {
  char* str = other.string_;
  other.string_ = string_;
  string_ = str;
  size_t tmp_size = other.size_;
  other.size_ = size_;
  size_ = tmp_size;
  tmp_size = other.count_;
  other.count_ = count_;
  count_ = tmp_size;
}

bool String::Empty() const { return count_ == 0; }

size_t String::Size() const { return count_; }

size_t String::Capacity() const { return size_; }

const char* String::Data() const { return string_; }

char* String::Data() { return string_; }

char& String::Front() { return string_[0]; }

char String::Front() const { return string_[0]; }

char& String::Back() { return string_[count_ - 1]; }

char String::Back() const { return string_[count_ - 1]; }

const char& String::operator[](int number_el) const {
  return string_[number_el];
}

char& String::operator[](int number_el) { return string_[number_el]; }

String& String::operator=(const String& str) {
  if (this == &str) {
    return *this;
  }
  Resize(str.Size());
  for (size_t counter = 0; counter <= count_; counter++) {
    string_[counter] = str[counter];
  }
  return *this;
}

String& String::operator=(const char* str) {
  Resize(strlen(str));
  for (size_t i = 0; i < count_; i++) {
    string_[i] = str[i];
  }
  string_[count_] = '\0';
  return *this;
}

bool operator<(const String& str1, const String& str2) {
  size_t counter = 0;
  while (str1.Size() > counter && str2.Size() > counter &&
         str1[counter] == str2[counter]) {
    ++counter;
  }
  return !((str1.Size() >= counter && str2.Size() == counter) ||
           str1[counter] > str2[counter]);
}

bool operator==(const String& str1, const String& str2) {
  size_t counter = 0;
  if (str1.Size() != str2.Size()) {
    return false;
  }
  while (str1.Size() > counter && str1[counter] == str2[counter]) {
    ++counter;
  }
  return (str1.Size() == counter);
}

bool operator!=(const String& str1, const String& str2) {
  return !(str1 == str2);
}

bool operator<=(const String& str1, const String& str2) {
  return (str1 < str2 || str1 == str2);
}

bool operator>(const String& str1, const String& str2) {
  return !(str1 <= str2);
}

bool operator>=(const String& str1, const String& str2) {
  return (str1 > str2 || str1 == str2);
}

String& String::operator+=(const String& str2) {
  size_t resalt = count_ + str2.Size();
  Reserve(resalt);
  for (size_t i = 0; count_ < resalt; ++count_, ++i) {
    string_[count_] = str2[i];
  }
  string_[count_] = '\0';
  return *this;
}

String operator+(const String& str1, const String& str2) {
  String str = str1;
  str += str2;
  return str;
}

String operator*(const String& str1, size_t n) {
  String str = "\0";
  str.Reserve(str1.Size() * n);
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < str1.Size(); ++j) {
      str.PushBack(str1[j]);
    }
  }
  str[str1.Size() * n] = '\0';
  return str;
}

String operator*(size_t n, const String& str1) {
  String tmp = str1;
  return tmp * n;
}

String& String::operator*=(size_t n) {
  String res;
  res = *this * n;
  *this = res;
  return *this;
}

std::vector<String> String::Split(const String& delim) {
  std::vector<String> vec;
  String str;
  size_t cap = delim.Size();
  str.Reserve(cap);
  size_t counter = 0;
  size_t counter2 = 0;
  for (; counter < cap && counter < count_; ++counter) {
    str.PushBack(string_[counter]);
  }
  for (; counter <= count_; ++counter) {
    if (str == delim) {
      String res;
      for (; counter2 < counter - cap; ++counter2) {
        res.PushBack(string_[counter2]);
      }
      counter2 += cap;
      vec.push_back(res);
    }
    for (size_t counter3 = 0; counter3 < cap - 1; ++counter3) {
      str.string_[counter3] = str.string_[counter3 + 1];
    }
    str[cap - 1] = string_[counter];
  }
  String res;
  for (; counter2 < count_; ++counter2) {
    res.PushBack(string_[counter2]);
  }
  vec.push_back(res);
  return vec;
}

String String::Join(const std::vector<String>& strings) {
  String str = "\0";
  if (!strings.empty()) {
    str = strings[0];
    Reserve(sizeof(strings) + count_ * strings.size());
    for (size_t counter = 1; counter < strings.size(); ++counter) {
      str += *this;
      str += strings[counter];
    }
  }
  return str;
}

std::ostream& operator<<(std::ostream& out, const String& str) {
  for (size_t i = 0; i < str.Size(); i++) {
    out << str[i];
  }
  return out;
}

std::istream& operator>>(std::istream& input, String& str) {
  if (!str.Empty()) {
    str.Clear();
  }
  char symbol;
  while ((symbol = input.get()) != EOF) {
    str.PushBack(symbol);
  }
  return input;
}
