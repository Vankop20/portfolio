#pragma once

#include <iostream>
#include <type_traits>
#include <vector>

template <bool>
class Symmetry;

template <>
class Symmetry<true> {};

template <size_t N, size_t M, typename T = int64_t>
class Matrix {
 private:
  std::vector<std::vector<T>> matrix_;

 public:
  Matrix();
  Matrix(T elem);
  Matrix(const std::vector<std::vector<T>>& matrix);
  Matrix<N, M, T>& operator+=(const Matrix<N, M, T>& second_matrix);
  Matrix(const Matrix<N, M, T>& other);
  Matrix<N, M, T>& operator-=(const Matrix<N, M, T>& second_matrix);
  Matrix<N, M, T> operator+(const Matrix<N, M, T>& second_matrix);
  Matrix<N, M, T>& operator*=(const T& value);
  Matrix<N, M, T> operator*(const T& value);
  Matrix<N, M, T> operator-(const Matrix<N, M, T>& second_matrix);
  template <size_t K>
  Matrix<N, K, T> operator*(const Matrix<M, K, T>& second_matrix);
  Matrix<M, N, T> Transposed() const;
  const T& operator()(size_t row, size_t column) const;
  T& operator()(size_t row, size_t column);
  T Trace() const;
  Matrix<N, M, T>& operator=(const Matrix<N, M, T>& second_matrix);
  bool operator==(const Matrix<N, M, T>& second_matrix);
};

template <size_t N, size_t M, typename T>
Matrix<N, M, T>::Matrix() {
  matrix_.assign(N, std::vector<T>(M));
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < M; ++j) {
      matrix_[i][j] = T(0);
    }
  }
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T>::Matrix(T elem) {
  matrix_.assign(N, std::vector<T>(M, T(elem)));
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T>::Matrix(const std::vector<std::vector<T>>& matrix) {
  matrix_.assign(N, std::vector<T>(M));
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < M; ++j) {
      matrix_[i][j] = matrix[i][j];
    }
  }
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T>& Matrix<N, M, T>::operator+=(
    const Matrix<N, M, T>& second_matrix) {
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < M; ++j) {
      matrix_[i][j] += second_matrix.matrix_[i][j];
    }
  }
  return *this;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T>::Matrix(const Matrix<N, M, T>& other) {
  matrix_ = other.matrix_;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T>& Matrix<N, M, T>::operator-=(
    const Matrix<N, M, T>& second_matrix) {
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < M; ++j) {
      matrix_[i][j] -= second_matrix.matrix_[i][j];
    }
  }
  return *this;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T> Matrix<N, M, T>::operator+(
    const Matrix<N, M, T>& second_matrix) {
  Matrix<N, M, T> tmp = *this;
  tmp += second_matrix;
  return tmp;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T>& Matrix<N, M, T>::operator*=(const T& value) {
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < M; ++j) {
      matrix_[i][j] *= value;
    }
  }
  return *this;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T> Matrix<N, M, T>::operator*(const T& value) {
  Matrix<N, M, T> tmp = *this;
  tmp *= value;
  return tmp;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T> Matrix<N, M, T>::operator-(
    const Matrix<N, M, T>& second_matrix) {
  Matrix<N, M, T> tmp = *this;
  tmp -= second_matrix;
  return tmp;
}

template <size_t N, size_t M, typename T>
template <size_t K>
Matrix<N, K, T> Matrix<N, M, T>::operator*(
    const Matrix<M, K, T>& second_matrix) {
  Matrix<N, K, T> tmp;
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < K; ++j) {
      tmp(i, j) = T(0);
      for (size_t third = 0; third < M; ++third) {
        tmp(i, j) += matrix_[i][third] * second_matrix(third, j);
      }
    }
  }
  return tmp;
}

template <size_t N, size_t M, typename T>
Matrix<M, N, T> Matrix<N, M, T>::Transposed() const {
  Matrix<M, N, T> result;
  for (size_t i = 0; i < M; ++i) {
    for (size_t j = 0; j < N; ++j) {
      result(i, j) = matrix_[j][i];
    }
  }
  return result;
}

template <size_t N, size_t M, typename T>
const T& Matrix<N, M, T>::operator()(size_t row, size_t column) const {
  return matrix_[row][column];
}

template <size_t N, size_t M, typename T>
T& Matrix<N, M, T>::operator()(size_t row, size_t column) {
  return matrix_[row][column];
}

template <size_t N, size_t M, typename T>
T Matrix<N, M, T>::Trace() const {
  Symmetry<N == M> flag;
  Matrix<N, N, T> tmp = *this;
  T res = 0;
  for (size_t i = 0; i < N; ++i) {
    res += matrix_[i][i];
  }
  return res;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T>& Matrix<N, M, T>::operator=(
    const Matrix<N, M, T>& second_matrix) {
  matrix_ = second_matrix.matrix_;
  return *this;
}

template <size_t N, size_t M, typename T>
bool Matrix<N, M, T>::operator==(const Matrix<N, M, T>& second_matrix) {
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < M; ++j) {
      if (matrix_[i][j] != second_matrix[i][j]) {
        return false;
      }
    }
  }
  return true;
}
