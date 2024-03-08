#include <ctime>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <stdio.h>
#include <pthread.h>
#include <thread>
#include <ctime>

static pthread_mutex_t total_mutex = PTHREAD_MUTEX_INITIALIZER;

//trivial solution
/*
int main() {
  const int n = 3;
  const int m = 3;
  //std::cin>>n>>m;
  int (*matrix_a)[n] = new int[n][m];
  int (*matrix_b)[m] = new int[m][n];
  int (*result)[n] = new int[n][n];
  for(int i = 0; i < n; ++i) {
  	for (int j = 0; j < m; ++j) {
  	  matrix_a[i][j] = rand() % 100;
  	  matrix_b[j][i] = rand() % 100;
	}
  }
  for(int i = 0; i < n; ++i) {
  	for (int j = 0; j < n; ++j) {
  	  for(int l = 0; l < m; ++l) {
  	  	result[i][j] += matrix_a[i][l] * matrix_b[l][j];
	  }
	}
  }
  for(int i = 0; i < n; ++i) {
  	for (int j = 0; j < m; ++j) {
  	  std::cout<<matrix_a[i][j]<<" ";
	}
	std::cout<<std::endl;
  }
  std::cout<<std::endl;
  for(int i = 0; i < m; ++i) {
  	for (int j = 0; j < n; ++j) {
  	  std::cout<<matrix_b[i][j]<<" ";
	}
	std::cout<<std::endl;
  }
  std::cout<<std::endl;
  for(int i = 0; i < n; ++i) {
  	for (int j = 0; j < n; ++j) {
  	  std::cout<<result[i][j]<<" ";
	}
	std::cout<<std::endl;
  }
}
*/

//solution using transpose
/*
template<size_t N, size_t M>
class Matrix {
  private:
    std::vector<std::vector<int>> matrix_;
  public:
    Matrix() {
      matrix_.assign(N, std::vector<int>(M));
      for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < M; ++j) {
          matrix_[i][j] = 0;
        }
      }
    }

    Matrix<N, M>& operator=(const Matrix<N, M>& second_matrix) {
      matrix_ = second_matrix.matrix_;
      return *this;
    }

    void Random(int del) {
      for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < M; ++j) {
          matrix_[i][j] = rand() % del;
        }
      }
    }

    int& operator()(size_t row, size_t column) {
      return matrix_[row][column];
    }

    Matrix<M, N> Transposed() const {
      Matrix<M, N> result;
      for (size_t i = 0; i < M; ++i) {
        for (size_t j = 0; j < N; ++j) {
          result(i, j) = matrix_[j][i];
        }
      }
      return result;
    }

    Matrix<N, N> operator*(const Matrix<M, N>& matrix_c) {
      Matrix<N, M> matrix_b;
      Matrix<N, N> result;
      matrix_b = matrix_c.Transposed();
      for(size_t i = 0; i < N; ++i) {
      	for (size_t j = 0; j < N; ++j) {
      	  for(size_t l = 0; l < M; ++l) {
      	  	result(i, j) += matrix_[i][l] * matrix_b(j, l);
    	  }
    	}
      }
      return result;
    }

};

int main() {
  const int n = 600;
  const int m = 600;
  Matrix<n, m> matrix_a;
  Matrix<n, m> matrix_b;
  Matrix<n, n> result;
  matrix_a.Random(100);
  matrix_b.Random(100);
  result = matrix_a * matrix_b;
  for(int i = 0; i < n; ++i) {
  	for (int j = 0; j < m; ++j) {
  	  std::cout<<matrix_a(i, j)<<" ";
	}
	std::cout<<std::endl;
  }
  std::cout<<std::endl;
  for(int i = 0; i < m; ++i) {
  	for (int j = 0; j < n; ++j) {
  	  std::cout<<matrix_b(i, j)<<" ";
	}
	std::cout<<std::endl;
  }
  std::cout<<std::endl;
  for(int i = 0; i < n; ++i) {
  	for (int j = 0; j < n; ++j) {
  	  std::cout<<result(i, j)<<" ";
	}
	std::cout<<std::endl;
  }
}
*/

//threads

unsigned int p = 40;
bool f[10] = {};

class MyArgv {
	public:
  size_t N;
  size_t M;
  long long* matrix_a;
  long long* matrix_b;
  long long* res;
  MyArgv(){}
  MyArgv(size_t n, size_t m, long long* matrix_a_, long long* matrix_b_){
	  N = n;
	  M = m;
      matrix_a = matrix_a_;
      matrix_b = matrix_b_;
  }
};

void* mult_colum_row(void* argv);

class Matrix {
  private:
    std::vector<std::vector<int>> matrix_;
    size_t N;
    size_t M;
  public:
    Matrix(){}
    Matrix(size_t n, size_t m) {
      N=n;
      M=m;
      matrix_.assign(N, std::vector<int>(M));
      for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < M; ++j) {
          matrix_[i][j] = 0;
        }
      }
    }

  Matrix(const Matrix& other) {
    matrix_ = other.matrix_;
  }


    Matrix& operator=(const Matrix& second_matrix) {
      matrix_ = second_matrix.matrix_;
      return *this;
    }

    void Random() {
      for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < M; ++j) {
          matrix_[i][j] = rand();
        }
      }
    }

    int& operator()(size_t row, size_t column) {
      return matrix_[row][column];
    }

    Matrix Transposed() const {
      Matrix result(M, N);
      for (size_t i = 0; i < M; ++i) {
        for (size_t j = 0; j < N; ++j) {
          result(i, j) = matrix_[j][i];
        }
      }
      return result;
    }

    Matrix operator*(const Matrix& matrix_c) {
      for(int i = 0; i < p; ++i){
        f[i] = 0;
      }
      //pthread_t *threads = new pthread_t(p * sizeof(pthread_t));
      pthread_t threads[p] = {};
      Matrix matrix_b(N, M);
      Matrix result(N, N);
      matrix_b = matrix_c.Transposed();
      MyArgv argv(N, M, reinterpret_cast <long long*> (this), reinterpret_cast <long long*> (&matrix_b));
      for (int k = 0; k < p; ++k) {
        argv.res = reinterpret_cast <long long*> (&result);
        pthread_create(threads + k, 0, mult_colum_row, (void*) &argv);
      }
      for (size_t k = 0; k < p; ++k) {
        (void) pthread_join(threads[k], 0);
      }
      return result;
    }
};

    void* mult_colum_row(void* argv) {
      MyArgv* arg = (MyArgv*) argv;
      Matrix* matrix_a = reinterpret_cast <Matrix*>(arg->matrix_a);
      Matrix* matrix_b = reinterpret_cast <Matrix*>(arg->matrix_b);
      Matrix* res = reinterpret_cast <Matrix*>(arg->res);
      int a = 0;
      int b = 0;

      for(int i = 0; i < p; ++i) {
        if(!f[i]) {
          f[i] = 1;
          a = ((arg->N * i)) / p;
          b = ((arg->N * (i + 1))) / p;
          break;
        }
      }
      for(size_t i = a; i < b; ++i) {
        for (size_t j = 0; j < arg->N; ++j){
          int result = 0;
          for(size_t l = 0; l < arg->M; ++l) {
        	result += (*matrix_a)(i, l) * (*matrix_b)(j, l);
          }
          pthread_mutex_lock (&total_mutex);
          (*res)(i, j) += result;
          pthread_mutex_unlock (&total_mutex);
    	}
      }
      return 0;
    }

int main() {
  for(int i = 0; i < 1000; i++){
    size_t n = 100 * i;
    size_t m = 100 * i;
    Matrix matrix_a(n, m);
    Matrix matrix_b(n, m);
    Matrix result(n, n);
    std::cout<<"num: "<<i * 100;
    matrix_a.Random();
    matrix_b.Random();
    time_t first;
    time(&first);
    int start = clock();
    result = matrix_a * matrix_b;
    int end = clock();
    time_t second;
    time(&second);
    std::cout<<" col time: "<<difftime(second, first)<<" sec; time: "<<(double(end - start) / CLOCKS_PER_SEC)<<std::endl;
  }
  /*
  for(int i = 0; i < n; ++i) {
  	for (int j = 0; j < m; ++j) {
  	  std::cout<<matrix_a(i, j)<<" ";
	}
	std::cout<<std::endl;
  }
  std::cout<<std::endl;
  for(int i = 0; i < m; ++i) {
  	for (int j = 0; j < n; ++j) {
  	  std::cout<<matrix_b(i, j)<<" ";
	}
	std::cout<<std::endl;
  }
  std::cout<<std::endl;
  for(int i = 0; i < n; ++i) {
  	for (int j = 0; j < n; ++j) {
  	  std::cout<<result(i, j)<<" ";
	}
	std::cout<<std::endl;
  }
  */
}


