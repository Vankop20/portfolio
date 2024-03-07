#pragma once
#include <iostream>
#include <vector>

class String {
 private:
  char* string_ = nullptr;
  size_t count_ = 0;
  size_t size_ = 0;

 public:
  String();
  String(size_t size, char character);
  String(const char* str);
  String(const String& str);
  ~String();
  void Clear();
  void Reserve(size_t new_cap);
  void PushBack(char character);
  void PopBack();
  void Resize(size_t new_size);
  void Resize(size_t new_size, char character);
  void ShrinkToFit();
  void Swap(String& other);
  bool Empty() const;
  size_t Size() const;
  size_t Capacity() const;
  const char* Data() const;
  char* Data();
  char& Front();
  char Front() const;
  char& Back();
  char Back() const;
  const char& operator[](int number_el) const;
  char& operator[](int number_el);
  String& operator=(const String& str);
  String& operator=(const char* str);
  String& operator+=(const String& str2);
  String& operator*=(size_t n);
  std::vector<String> Split(const String& del = " ");
  String Join(const std::vector<String>&);
};

String operator*(const String& str1, size_t n);
String operator*(size_t n, const String& str1);
String operator+(const String& str1, const String& str2);
bool operator==(const String& str1, const String& str2);
bool operator!=(const String& str1, const String& str2);
bool operator>=(const String& str1, const String& str2);
bool operator<=(const String& str1, const String& str2);
bool operator>(const String& str1, const String& str2);
bool operator<(const String& str1, const String& str2);
std::ostream& operator<<(std::ostream& out, const String& str);
std::istream& operator>>(std::istream& input, String& str);
