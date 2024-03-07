#include "geometry.hpp"

Vector::Vector(int64_t axisx, int64_t axisy) {
  x_ = axisx;
  y_ = axisy;
}

Vector::Vector(const Vector& other) {
  x_ = other.x_;
  y_ = other.y_;
}

int64_t Vector::operator*(const Vector& other) const {
  return (x_ * other.x_ + y_ * other.y_);
}

int64_t Vector::operator^(const Vector& other) const {
  return (x_ * other.y_ - y_ * other.x_);
}

Vector& Vector::operator+=(const Vector& second) {
  x_ += second.x_;
  y_ += second.y_;
  return *this;
}

Vector& Vector::operator-=(const Vector& second) {
  x_ -= second.x_;
  y_ -= second.y_;
  return *this;
}

Vector Vector::operator+(const Vector& second) const {
  Vector tmp = *this;
  tmp += second;
  return tmp;
}

Vector Vector::operator-(const Vector& second) const {
  Vector tmp = *this;
  tmp -= second;
  return tmp;
}

Vector Vector::operator*=(const int64_t& second) {
  x_ *= second;
  y_ *= second;
  return *this;
}

Vector Vector::operator-() {
  Vector tmp = *this;
  tmp.x_ = -tmp.x_;
  tmp.y_ = -tmp.y_;
  return tmp;
}

int64_t Vector::GetX() const { return x_; }

int64_t Vector::GetY() const { return y_; }

Vector operator*(const Vector& first, const int64_t& second) {
  Vector tmp = first;
  tmp *= second;
  return tmp;
}

Vector operator*(const int64_t& second, const Vector& first) {
  return first * second;
}

Point::Point() {
  x_ = 0;
  y_ = 0;
}

Point::Point(int64_t axisx, int64_t axisy) {
  x_ = axisx;
  y_ = axisy;
}

Point::Point(const Point& other) {
  x_ = other.GetX();
  y_ = other.GetY();
}

int64_t Point::GetX() const { return x_; }

int64_t Point::GetY() const { return y_; }

Point& Point::operator=(Point other) {
  x_ = other.GetX();
  y_ = other.GetY();
  return *this;
}

Point& Point::operator-=(Point other) {
  x_ -= other.GetX();
  y_ -= other.GetY();
  return *this;
}

Point Point::operator-(Point other) {
  Point tmp = *this;
  tmp -= other;
  return tmp;
}

void Point::Move(const Vector& slip) {
  x_ += slip.GetX();
  y_ += slip.GetY();
}

bool Point::ContainsPoint(const Point& point) {
  return ((point.GetX() == GetX()) && (point.GetY() == GetY()));
}

bool Point::CrossSegment(const Segment& seg) {
  Segment tmp = seg;
  return tmp.ContainsPoint(*this);
}

Vector::Vector(Point pointa, Point pointb) {
  x_ = pointb.GetX() - pointa.GetX();
  y_ = pointb.GetY() - pointa.GetY();
}

Vector::Vector(Point pointa) {
  x_ = pointa.GetX();
  y_ = pointa.GetY();
}

double Lenght(Point first, Point second) {
  double x_one = first.GetX();
  double y_one = first.GetY();
  double x_two = second.GetX();
  double y_two = second.GetY();
  double res = sqrt((x_two - x_one) * (x_two - x_one) +
                    (y_two - y_one) * (y_two - y_one));
  return res;
}

Segment::Segment() {}

Segment::Segment(Point first, Point second) {
  apex_[0] = first;
  apex_[1] = second;
}

Segment::Segment(const Segment& other) {
  apex_[0] = other.GetA();
  apex_[1] = other.GetB();
}

Point Segment::GetA() const { return apex_[0]; }

Point Segment::GetB() const { return apex_[1]; }

void Segment::Move(const Vector& slip) {
  for (int64_t i = 0; i < base_; i++) {
    apex_[i].Move(slip);
  }
}

bool Segment::ContainsPoint(const Point& point) {
  Vector a_b(apex_[0], apex_[1]);
  Vector a_p(apex_[0], point);
  Vector p_a(point, apex_[0]);
  Vector p_b(point, apex_[1]);
  return ((a_b ^ a_p) == 0 && (p_a * p_b) <= 0);
}

bool Segment::CrossSegment(const Segment& seg) {
  Point point_a = seg.GetA();
  Point point_b = seg.GetB();
  Segment tmp = seg;
  if (ContainsPoint(point_a) || ContainsPoint(point_b) ||
      tmp.ContainsPoint(apex_[0]) || tmp.ContainsPoint(apex_[1])) {
    return true;
  }
  Vector p_p(point_a, point_b);
  Vector p_b(point_a, apex_[1]);
  Vector p_a(point_a, apex_[0]);
  Vector a_b(apex_[0], apex_[1]);
  Vector a_p1(apex_[0], point_a);
  Vector a_p2(apex_[0], point_b);
  Line one(point_a, point_b);
  Line two(apex_[0], apex_[1]);
  bool flag = (one.CrossSegment(seg) && two.CrossSegment(*this));
  return (((((p_p ^ p_b) * (p_p ^ p_a)) < 0) &&
           ((a_b ^ a_p1) * (a_b ^ a_p2)) < 0) ||
          ContainsPoint(point_a) || ContainsPoint(point_b)) &&
         flag;
}

Ray::Ray() {}

Ray::Ray(Point first, Point second) {
  apex_[0] = first;
  apex_[1] = second;
}

Ray::Ray(const Ray& other) {
  apex_[0] = other.apex_[0];
  apex_[1] = other.apex_[1];
}

Point Ray::GetA() { return apex_[0]; }

Vector Ray::GetVector() {
  Vector direction(apex_[0], apex_[1]);
  return direction;
}

void Ray::Move(const Vector& slip) {
  for (int64_t i = 0; i < base_; i++) {
    apex_[i].Move(slip);
  }
}

bool Ray::ContainsPoint(const Point& point) {
  Vector a_b(apex_[0], apex_[1]);
  Vector a_p(apex_[0], point);
  return ((a_b ^ a_p) == 0 && (a_b * a_p) >= 0);
}

bool Ray::CrossSegment(const Segment& seg) {
  Point point_a = seg.GetA();
  Point point_b = seg.GetB();
  Segment tmp = seg;
  if (ContainsPoint(point_a) || ContainsPoint(point_b) ||
      tmp.ContainsPoint(apex_[0]) || tmp.ContainsPoint(apex_[1])) {
    return true;
  }
  Point point_p = apex_[0];
  Vector p_a(point_p, point_a);
  Vector p_b(point_p, point_b);
  Vector p_p(apex_[0], apex_[1]);
  return (((p_p ^ p_a) * (p_p ^ p_b)) <= 0 && (p_a * p_p) >= 0 &&
          (p_p * p_b) >= 0);
}

Line::Line() {}

Line::Line(Point first, Point second) {
  apex_[0] = first;
  apex_[1] = second;
}

Line::Line(const Line& other) {
  apex_[0] = other.apex_[0];
  apex_[1] = other.apex_[1];
}

int64_t Line::GetA() { return (apex_[1].GetY() - apex_[0].GetY()); }

int64_t Line::GetB() { return (apex_[0].GetX() - apex_[1].GetX()); }

int64_t Line::GetC() {
  return ((apex_[0].GetY() * apex_[1].GetX()) -
          (apex_[1].GetY() * apex_[0].GetX()));
}

void Line::Move(const Vector& slip) {
  for (int64_t i = 0; i < base_; i++) {
    apex_[i].Move(slip);
  }
}

bool Line::ContainsPoint(const Point& point) {
  Vector a_b(apex_[0], apex_[1]);
  Vector a_p(apex_[0], point);
  return ((a_b ^ a_p) == 0);
}

bool Line::CrossSegment(const Segment& seg) {
  Point point_a = seg.GetA();
  Point point_b = seg.GetB();
  Point point_p = apex_[0];
  Vector p_a(point_p, point_a);
  Vector p_b(point_p, point_b);
  Vector p_p(apex_[0], apex_[1]);
  return (((p_p ^ p_a) * (p_p ^ p_b)) <= 0);
}

Circle::Circle() { r_ = 0; }

Circle::Circle(Point center, size_t radius) {
  apex_ = center;
  r_ = radius;
}

Circle::Circle(const Circle& other) {
  apex_ = other.apex_;
  r_ = other.r_;
}

Point Circle::GetCentre() { return apex_; }

int64_t Circle::GetRadius() const { return r_; }

void Circle::Move(const Vector& slip) { apex_.Move(slip); }

bool Circle::ContainsPoint(const Point& point) {
  return ((size_t)(point.GetX() - apex_.GetX()) *
                  (point.GetX() - apex_.GetX()) +
              (point.GetY() - apex_.GetY()) * (point.GetY() - apex_.GetY()) <=
          r_ * r_);
}

bool Circle::CrossSegment(const Segment& seg) {
  Point point_a = seg.GetA();
  Point point_b = seg.GetB();
  Point center = apex_;
  Vector a_o(point_a, center);
  Vector a_b(point_a, point_b);
  double radius = (double)r_;
  if ((a_o * a_b) < 0) {
    double len1 = Lenght(center, point_a);
    double len2 = Lenght(center, point_b);
    return !(len1 > radius && len2 > radius);
  }
  double square = (double)(a_b ^ a_o);
  double base = Lenght(point_a, point_b);
  if (square < 0) {
    square = -square;
  }
  double height = square / base;
  return ((height <= radius) && (Lenght(center, point_a) >= radius ||
                                 Lenght(center, point_b) >= radius));
}

IShape* Point::Clone() { return new Point(*this); }

IShape* Segment::Clone() { return new Segment(*this); }

IShape* Ray::Clone() { return new Ray(*this); }

IShape* Line::Clone() { return new Line(*this); }

IShape* Circle::Clone() { return new Circle(*this); }
