#include <cstddef>
#include <fstream>
#include <iostream>
#include <ostream>

class Element {  // класс элементов реакции
 private:
  double* r_element_ = nullptr;
  size_t size_;

 public:
  Element() = default;
  Element(size_t);
  Element(double*, size_t);
  Element(const Element&);
  Element& operator+=(const Element&);
  Element operator+(const Element&);
  Element operator*(const double&);
  double& a();
  double& b();
  double& c();
  double& d();
  double& e();
  double& f();
  double& q();
  double& p();
  const double& a() const;
  const double& b() const;
  const double& c() const;
  const double& d() const;
  const double& e() const;
  const double& f() const;
  const double& q() const;
  const double& p() const;
  ~Element();
};

class Coeff {  // класс коэффициентов
 private:
  double* k_ = nullptr;

 public:
  Coeff() = default;
  Coeff(double* k, size_t size);
  ~Coeff();
  const double& ab_c() const;
  const double& c_ab() const;
  const double& ac_d() const;
  const double& d_ac() const;
  const double& ad_e() const;
  const double& e_ad() const;
  const double& ae_f() const;
  const double& f_ae() const;
  const double& f_bp() const;
  const double& bp_f() const;
  const double& c_bq() const;
  const double& bq_c() const;
  const double& d_bq() const;
  const double& bq_d() const;
  const double& e_bq() const;
  const double& bq_e() const;
};

Coeff::Coeff(double* k, size_t size) {
  k_ = new double[size]();
  for (size_t i = 0; i < size; ++i) {
    k_[i] = k[i];
  }
}

Coeff::~Coeff() { delete k_; }

const double& Coeff::ab_c() const { return k_[8]; }
const double& Coeff::c_ab() const { return k_[7]; }
const double& Coeff::ac_d() const { return k_[9]; }
const double& Coeff::d_ac() const { return k_[6]; }
const double& Coeff::ad_e() const { return k_[10]; }
const double& Coeff::e_ad() const { return k_[5]; }
const double& Coeff::ae_f() const { return k_[11]; }
const double& Coeff::f_ae() const { return k_[4]; }
const double& Coeff::f_bp() const { return k_[12]; }
const double& Coeff::bp_f() const { return k_[3]; }
const double& Coeff::c_bq() const { return k_[13]; }
const double& Coeff::bq_c() const { return k_[2]; }
const double& Coeff::d_bq() const { return k_[14]; }
const double& Coeff::bq_d() const { return k_[1]; }
const double& Coeff::e_bq() const { return k_[15]; }
const double& Coeff::bq_e() const { return k_[0]; }

Element::Element(size_t size) {
  r_element_ = new double[size];
  for (int i = 0; i < size; ++i) {
    r_element_[i] = 0;
  }
  size_ = size;
}

Element::Element(double* arr, size_t size) {
  r_element_ = new double[size];
  for (int i = 0; i < size; ++i) {
    r_element_[i] = arr[i];
  }
  size_ = size;
}

Element::Element(const Element& other) {
  r_element_ = new double[other.size_];
  for (int i = 0; i < other.size_; ++i) {
    r_element_[i] = other.r_element_[i];
  }
  size_ = other.size_;
}

Element& Element::operator+=(const Element& other) {
  for (int i = 0; i < other.size_; ++i) {
    r_element_[i] += other.r_element_[i];
  }
  return *this;
}

Element Element::operator+(const Element& other) {
  Element copy = *this;
  copy += other;
  return copy;
}

Element Element::operator*(const double& number) {
  Element copy = *this;
  for (int i = 0; i < size_; ++i) {
    copy.r_element_[i] *= number;
  }
  return copy;
}

Element::~Element() { delete[] r_element_; }

double& Element::a() { return r_element_[0]; }

double& Element::b() { return r_element_[1]; }

double& Element::c() { return r_element_[2]; }

double& Element::d() { return r_element_[3]; }

double& Element::e() { return r_element_[4]; }

double& Element::f() { return r_element_[5]; }

double& Element::q() { return r_element_[6]; }

double& Element::p() { return r_element_[7]; }

const double& Element::a() const { return r_element_[0]; }

const double& Element::b() const { return r_element_[1]; }

const double& Element::c() const { return r_element_[2]; }

const double& Element::d() const { return r_element_[3]; }

const double& Element::e() const { return r_element_[4]; }

const double& Element::f() const { return r_element_[5]; }

const double& Element::q() const { return r_element_[6]; }

const double& Element::p() const { return r_element_[7]; }

std::ostream& operator<<(std::ostream& out, const Element& el) {
  out << el.a() << "; " << el.b() << "; " << el.c() << "; " << el.d() << "; "
      << el.e() << "; " << el.f() << "; " << el.q() << "; " << el.p() << ";";
  return out;
}

Element function(Coeff k, const Element& y) {
  Element result(8);
  result.a() = -y.a() * y.b() * k.ab_c() + y.c() * k.c_ab() -
               y.a() * y.c() * k.ac_d() + y.d() * k.d_ac() -
               y.a() * y.d() * k.ad_e() + y.e() * k.e_ad() -
               y.a() * y.e() * k.ae_f() + y.f() * k.f_ae();
  result.b() = -y.a() * y.b() * k.ab_c() + y.c() * k.c_ab() + y.f() * k.f_bp() -
               y.b() * y.p() * k.bp_f() + y.c() * k.c_bq() -
               y.b() * y.q() * k.bq_c() + y.d() * k.d_bq() -
               y.b() * y.q() * k.bq_d() + y.e() * k.e_bq() -
               y.b() * y.q() * k.bq_e();
  result.c() = y.a() * y.b() * k.ab_c() - y.c() * k.c_ab() -
               y.a() * y.c() * k.ac_d() + y.d() * k.d_ac() - y.c() * k.c_bq() +
               y.b() * y.q() * k.bq_c();
  result.d() = y.a() * y.c() * k.ac_d() - y.d() * k.d_ac() -
               y.a() * y.d() * k.ad_e() + y.e() * k.e_ad() - y.d() * k.d_bq() +
               y.b() * y.q() * k.bq_d();
  result.e() = y.a() * y.d() * k.ad_e() - y.e() * k.e_ad() -
               y.a() * y.e() * k.ae_f() + y.f() * k.f_ae() - y.e() * k.e_bq() +
               y.b() * y.q() * k.bq_e();
  result.f() = y.a() * y.e() * k.ae_f() - y.f() * k.f_ae() - y.f() * k.f_bp() +
               y.b() * y.p() * k.bp_f();
  result.q() = y.c() * k.c_bq() - y.b() * y.q() * k.bq_c() + y.d() * k.d_bq() -
               y.b() * y.q() * k.bq_d() + y.e() * k.e_bq() -
               y.b() * y.q() * k.bq_e();
  result.p() = y.f() * k.f_bp() - y.b() * y.p() * k.bp_f();
  return result;
}

void sort_r(double& r1, double& r2) {
  if (r1 > r2) {
    double buf = r1;
    r1 = r2;
    r2 = buf;
  }
}

void reaction(Element y, const Coeff& k, double time_limit, double dt,
              int fineness, std::string way_to_file, double reached_c1,
              double reached_c2) {
  std::ofstream out(way_to_file);
  sort_r(reached_c1, reached_c2);
  double max = y.p();
  double time1 = 0;
  double time2 = 0;
  bool f1 = (y.p() >= reached_c1);
  bool f2 = (y.p() >= reached_c2);
  for (double t = 0; t < time_limit;) {
    out << t << "; " << y << '\n';
    for (int i = 0; i < fineness && t < time_limit; t += dt, ++i) {
      Element k_1 = function(k, y);
      Element k_2 = function(k, (y + (k_1 * (dt / 2))));
      Element k_3 = function(k, (y + (k_2 * (dt / 2))));  // метод Рунге-Кутта
      Element k_4 = function(k, (y + (k_3 * dt)));
      y += (k_1 + k_2 * 2 + k_3 * 2 + k_4) * (dt / 6);
      if (y.p() > max) {
        max = y.p();
        if (!f1 && y.p() >= reached_c1) {
          f1 = true;
          time1 = t;
        }
        if (!f2 && y.p() >= reached_c2) {
          f2 = true;
          time2 = t;
        }
      }
    }
  }
  out << time_limit << "; " << y << '\n';
  std::cout.precision(5);
  if (f1) {
    std::cout << "Yes, concentration reached " << reached_c1 << " at time "
              << time1 << " seconds\n";
    if (f2) {
      std::cout << "Yes, concentration reached " << reached_c2 << " at time "
                << time2 << " seconds\n";
    } else {
      std::cout << "No, concentration has not reached " << reached_c2 << '\n';
    }
  } else {
    std::cout << "No, concentration has not reached " << reached_c1 << " or "
              << reached_c2 << '\n';
  }
  std::cout.precision(5);
  std::cout << "The maximum concentration is " << max << std::endl;
}

int main() {
  double k[] = {1.0E-19, 1.0E-19, 1.7E-20, 1.0E-5, 1.0E-4, 1.2E-5,
                1.2E-5,  1.0E-6,  6.0E-1,  4.0E-2, 2.0E-3, 1.5E-4,
                1.0E-3,  1.0E-6,  1.0E-6,  1.0E-4};  // коэффициенты скорости
                                                     // химических реакций
  Coeff coef(k, 16);
  double start_num[] = {5.0, 0.3, 0.2, 0, 0,
                        0,   0,   0};  // начальные значения количества вещества
  int fineness = 100;          // мелкость разбиения
  double dt = 1.0 / fineness;  // разбиение
  std::string way_to_file;  // путь к файлу записи промежуточных концентраций
  double reached_c1;  // концентрации которые необходимо проверить у вещества р
  double reached_c2;
  double time;  // продолжительность реакции
  std::cin >> way_to_file;
  std::cin >> reached_c1 >> reached_c2 >> time;
  reaction(Element(start_num, 8), coef, time, dt, fineness, way_to_file,
           reached_c1, reached_c2);
}
