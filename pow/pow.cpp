#include <cstdlib>
#include <iostream>

/*

double ln(double x) {
  double d = 1.0 / 100000000;
  double res = 0;
  double a;
  double b;
  if (x < 1) {
    a = x;
    b = 1;
    for (; a < b; a += d) {
      res -= (1.0 / a) * d;
    }
  } else {
    a = 1;
    b = x;

    for (; a < b; a += d) {
      res += (1.0 / a) * d;
    }
  }
  return res;
}

double double_to_int_deg(double x, int n) {
  double result = 1;
  for (int i = 0; i < n; ++i) {
    result *= x;
  }
  return result;
}

double double_to_double_deg(double x, double n) {
  double a = n * ln(x);
  double res = 0;
  long long factorial[21] = {};
  factorial[0] = 1;
  for (int i = 1; i <= 20; ++i) {
    factorial[i] = factorial[i - 1] * i;
  }
  for (int i = 0; i <= 20; ++i) {
    res += double_to_int_deg(a, i) / factorial[i];
  }
  return res;
}

int main() {
  std::cout.precision(308);
  //std::cout << double_to_double_deg(5, 4);
  //std::cout << double_to_double_deg(5, 2);
  std::cout << double_to_double_deg(2.123444, 12.123);
  return 0;
}*/
/*

double ln(double x) {
  double d = 1.0 / 10000000;
  double res = 0;
  double a;
  double b;
  if (x < 1) {
    a = x;
    b = 1;
    for (; a < b; a += d) {
      res -= (1.0 / a) * d;
    }
  } else {
    a = 1;
    b = x;

    for (; a < b; a += d) {
      res += (1.0 / a) * d;
    }
  }
  return res;
}

double double_to_int_deg(double x, int n) {
  double result = 1;
  for (int i = 0; i < n; ++i) {
    result *= x;
  }
  return result;
}

double double_to_double_deg(double x, double n) {
  double res1 = double_to_int_deg(x, static_cast<int>(n));
  double a = (n - static_cast<int>(n)) * ln(x);
  double res = 0;
  long long factorial[21] = {};
  factorial[0] = 1;
  for (int i = 1; i <= 20; ++i) {
    factorial[i] = factorial[i - 1] * i;
  }
  for (int i = 0; i <= 20; ++i) {
    res += double_to_int_deg(a, i) / factorial[i];
  }
  return res * res1;
}

int main() {
  std::cout.precision(308);
  //std::cout << double_to_double_deg(5, 4);
  //std::cout << double_to_double_deg(5, 2);
  std::cout << double_to_double_deg(216.1, 2.2);
  return 0;
}
*/

//при увеличении результата теряется точность, но это вина не алгоритма, а стандарта IEEE-754
//маленьких аргументах точность примерно 6 знаков при аргументах больше сотни точность падает до трех или двух знаков

/*
//лучше
long double ln(long double x) { //тейлор
  int t = 0;
  while(x > 10) {
    x = x / 10;
    t++;
  }
  long double d = 1.0 / 1000000000; //если long double - 16 байт, то можно 10^9, точность выше но считает 2 минуты, иначе 10^8 точность ниже, но считает быстрее
  long double res = 0;
  long double a;
  long double b;
  if (x < 1) {
    a = x;
    b = 1;
    for (; a <= b - d; a += d) {
      res -= (1.0 / a) * d;
    }
    res -= (1.0 / b) * (b - a);
  } else {
    a = 1;
    b = x;
    for (; a <= b - d; a += d) {
      res += (1.0 / a) * d;
    }
    res += (1.0 / b) * (b - a);
  }
  if(t > 0) {
    return res + t * ln(10);
  }
  return res;
}*/

long double exp(long double x);

//метод ньютона
long double ln(long double x) {
  int t = 0;
  while(x > 10) {
    x = x / 10;
    t++;
  }
  long double res = 1;
  for(int i = 0; i < 30; ++i) {
    res = res - 1 + x / exp(res);
  }
  if(t > 0) {
    return res + t * ln(10);
  }
  return res;
}

/*
//хуже
long double ln(long double x) {
  int t = 0;
  while(x > 10) {
    x = x / 10;
    t++;
  }
  long double d = 1.0 / 100000000;
  long double res = 0;
  long double a;
  long double b;
  if (x < 1) {
    a = x;
    b = 1;
    for (; a <= b; a += d) {
      res -= (1.0 / a) * d;
    }
  } else {
    a = 1;
    b = x;
    for (; a <= b; a += d) {
      res += (1.0 / a) * d;
    }
  }
  if(t > 0) {
    return res + t * ln(10);
  }
  return res;
}*/

long double double_to_int_deg(long double x, int n) {
  long double result = 1;
  for (int i = 0; i < n; ++i) {
    result *= x;
  }
  return result;
}

/*
long double double_to_double_deg(long double x, long double n) {
  long double res1 = double_to_int_deg(x, static_cast<int>(n));
  long double a = (n - static_cast<int>(n)) * ln(x);
  long double res = 0;
  long long factorial[21] = {};
  factorial[0] = 1;
  for (int i = 1; i <= 20; ++i) {
    factorial[i] = factorial[i - 1] * i;
  }
  for (int i = 0; i <= 20; ++i) {
    res += double_to_int_deg(a, i) / factorial[i];
  }
  return res * res1;
}
*/

/*
long double double_div_factorial(long double x, int n) {
  for(; n > 0; n--) {
    x /= static_cast<long double>(n);
  }
  return x;
}

long double double_to_double_deg(long double x, long double n) {
  long double res1 = double_to_int_deg(x, static_cast<int>(n));
  long double a = (n - static_cast<int>(n)) * ln(x);
  long double res = 0;
  for (int i = 0; i <= 20; ++i) {
    res += double_div_factorial(double_to_int_deg(a, i), i);
  }
  return res * res1;
}*/

long double exp(long double x) {
  long double res = 1;
  for (int i = 30; i > 0; --i) {
    res = ( res * x ) / static_cast<long double>(i) + 1;//Горнер
  }
  return res;
}

long double double_to_double_deg(long double x, long double n) {
  long double res1 = double_to_int_deg(x, static_cast<int>(n));
  long double a = (n - static_cast<int>(n)) * ln(x);
  long double res = exp(a);
  return res * res1;
}

//коментарии для тейлора, теперь там где было 2 знака дает целых 7
int main() {
  std::cout.precision(100);
  std::cout << double_to_double_deg(5, -1) << std::endl;
  std::cout << double_to_double_deg(5, 2) << std::endl;
  std::cout << double_to_double_deg(216.7, 2.2) << std::endl;
  std::cout << double_to_double_deg(1.123444, 1.123) << std::endl;
  std::cout << double_to_double_deg(302.2, 2.4) << std::endl;
  std::cout << double_to_double_deg(4, 2.5) << std::endl;
  std::cout << double_to_double_deg(390625, 0.125) << std::endl;
  return 0;
}
