// solution

#include <algorithm>
#include <cmath>
#include <iostream>

class Point;
class Segment;

class Vector {
 private:
  int64_t x_;
  int64_t y_;

 public:
  Vector() : x_{0}, y_{0} {}
  Vector(int64_t axisx, int64_t axisy);
  Vector(const Vector& other);
  Vector(Point pointa, Point pointb);
  Vector(Point pointa);
  int64_t operator*(const Vector& other) const;
  int64_t operator^(const Vector& other) const;
  Vector& operator+=(const Vector& second);
  Vector& operator-=(const Vector& second);
  Vector operator+(const Vector& second) const;
  Vector operator-(const Vector& second) const;
  Vector operator*=(const int64_t& second);
  Vector operator-();
  int64_t GetX() const;
  int64_t GetY() const;
};

Vector operator*(const Vector& first, const int64_t& second);
Vector operator*(const int64_t& second, const Vector& first);

class IShape {
 public:
  virtual void Move(const Vector& slip) = 0;
  virtual bool ContainsPoint(const Point& point) = 0;
  virtual bool CrossSegment(const Segment& seg) = 0;
  virtual IShape* Clone() = 0;
  virtual ~IShape() = default;
};

class Point : public virtual IShape {
 private:
  int64_t x_;
  int64_t y_;

 public:
  Point();
  Point(int64_t axisx, int64_t axisy);
  Point(const Point& other);
  int64_t GetX() const;
  int64_t GetY() const;
  Point& operator=(Point other);
  Point& operator-=(Point other);
  Point operator-(Point other);
  void Move(const Vector& slip) override;
  bool ContainsPoint(const Point& point) override;
  bool CrossSegment(const Segment& seg) override;
  IShape* Clone() override;
};

double Lenght(Point first, Point second);

class Segment : public virtual IShape {
 private:
  Point apex_[2];
  int64_t base_ = 2;

 public:
  Segment();
  Segment(Point first, Point second);
  Segment(const Segment& other);
  Point GetA() const;
  Point GetB() const;
  void Move(const Vector& slip) override;
  bool ContainsPoint(const Point& point) override;
  bool CrossSegment(const Segment& seg) override;
  IShape* Clone() override;
};

class Ray : public virtual IShape {
 private:
  Point apex_[2];
  int64_t base_ = 2;

 public:
  Ray();
  Ray(Point first, Point second);
  Ray(const Ray& other);
  Point GetA();
  Vector GetVector();
  void Move(const Vector& slip) override;
  bool ContainsPoint(const Point& point) override;
  bool CrossSegment(const Segment& seg) override;
  IShape* Clone() override;
};

class Line : public virtual IShape {
 private:
  Point apex_[2];
  int64_t base_ = 2;

 public:
  Line();
  Line(Point first, Point second);
  Line(const Line& other);
  int64_t GetA();
  int64_t GetB();
  int64_t GetC();
  void Move(const Vector& slip) override;
  bool ContainsPoint(const Point& point) override;
  bool CrossSegment(const Segment& seg) override;
  IShape* Clone() override;
};

class Circle : public virtual IShape {
 private:
  Point apex_;
  size_t r_;

 public:
  Circle();
  Circle(Point center, size_t radius);
  Circle(const Circle& other);
  Point GetCentre();
  int64_t GetRadius() const;
  void Move(const Vector& slip) override;
  bool ContainsPoint(const Point& point) override;
  bool CrossSegment(const Segment& seg) override;
  IShape* Clone() override;
};
