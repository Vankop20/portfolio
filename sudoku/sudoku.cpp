#include <iostream>
#include <vector>

class Sudoku {
 private:
  short int** field_;

 public:
  Sudoku();
  Sudoku(short int other[9][9]);
  Sudoku(const Sudoku& other);
  Sudoku& operator=(const Sudoku& other);
  std::pair<short int, short int> square(const short int& num_i,
                                         const short int& num_j) const;
  std::vector<bool> possible(const short int& num_i,
                             const short int& num_j) const;
  bool decided();
  std::pair<bool, bool> add_cells();
  std::pair<bool, bool> add_lins();
  std::pair<bool, bool> add_colums();
  std::pair<bool, bool> add_squares();
  std::pair<bool, bool> add_rec();
  bool solution();
  short int& Get(const int& i, const int& j) const;
  ~Sudoku();
};

Sudoku::Sudoku() {
  field_ = new short int*[9]();
  for (int i = 0; i < 9; ++i) {
    field_[i] = new short int[9]();
  }
}

Sudoku::Sudoku(short int other[9][9]) {
  field_ = new short int*[9]();
  for (int i = 0; i < 9; ++i) {
    field_[i] = new short int[9]();
    for (int j = 0; j < 9; ++j) {
      field_[i][j] = other[i][j];
    }
  }
}

Sudoku::Sudoku(const Sudoku& other) {
  field_ = new short int*[9]();
  for (int i = 0; i < 9; ++i) {
    field_[i] = new short int[9]();
    for (int j = 0; j < 9; ++j) {
      field_[i][j] = other.field_[i][j];
    }
  }
}

Sudoku& Sudoku::operator=(const Sudoku& other) {
  if (&other != this) {
    for (int i = 0; i < 9; ++i) {
      for (int j = 0; j < 9; ++j) {
        field_[i][j] = other.field_[i][j];
      }
    }
  }
  return *this;
}

std::pair<short int, short int> Sudoku::square(const short int& num_i,
                                               const short int& num_j) const {
  std::pair<short int, short int> result;
  result.first = num_i / 3;
  result.second = num_j / 3;
  return result;
}

std::vector<bool> Sudoku::possible(const short int& num_i,
                                   const short int& num_j) const {
  std::vector<bool> nums(10, 1);
  for (int i = 0; i < 9; ++i) {
    if (field_[i][num_j] != '#') {
      nums[field_[i][num_j]] = 0;
    }
  }
  for (int j = 0; j < 9; ++j) {
    if (field_[num_i][j] != '#') {
      nums[field_[num_i][j]] = 0;
    }
  }
  std::pair<short int, short int> sq = square(num_i, num_j);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      if (field_[sq.first * 3 + i][sq.second * 3 + j] != '#') {
        nums[field_[sq.first * 3 + i][sq.second * 3 + j]] = 0;
      }
    }
  }
  return nums;
}

bool Sudoku::decided() {
  for (int i = 0; i < 9; ++i) {
    for (int j = 0; j < 9; ++j) {
      if (field_[i][j] == '#') {
        return false;
      }
    }
  }
  return true;
}

std::pair<bool, bool> Sudoku::add_cells() {
  bool change = false;
  for (int i = 0; i < 9; ++i) {
    for (int j = 0; j < 9; ++j) {
      if (field_[i][j] == '#') {
        std::vector<bool> pos = possible(i, j);
        short int num = '#';
        bool uno = true;
        for (int l = 1; l < 10; ++l) {
          if (pos[l]) {
            if (num == '#') {
              num = l;
            } else {
              uno = false;
              break;
            }
          }
        }
        if (uno) {
          if (num != '#') {
            field_[i][j] = num;
            change = true;
          } else {
            return std::pair<bool, bool>(false, change);
          }
        }
      }
    }
  }
  return std::pair<bool, bool>(true, change);
}

std::pair<bool, bool> Sudoku::add_lins() {
  bool change = false;
  for (int i = 0; i < 9; ++i) {
    int empty = 0;
    std::vector<std::vector<bool>> in_line(9);
    for (int j = 0; j < 9; ++j) {
      if (field_[i][j] == '#') {
        in_line[j] = possible(i, j);
        ++empty;
      } else {
        in_line[j] = std::vector<bool>(10, 0);
      }
    }
    std::vector<bool> pos_l_var(10, 0);
    std::vector<int> pos_l(10, 0);
    for (int l = 1; l < 10; ++l) {
      for (int j = 0; j < 9; ++j) {
        pos_l[l] += in_line[j][l];
        pos_l_var[l] = (pos_l_var[l] || in_line[j][l]);
      }
    }
    int sum = 0;
    for (int l = 1; l < 10; ++l) {
      sum += pos_l_var[l];
    }
    if (sum < empty) {
      return std::pair<bool, bool>(false, change);
    }
    for (int l = 1; l < 10; ++l) {
      if (pos_l[l] == 1) {
        for (int j = 0; j < 9; ++j) {
          if (in_line[j][l]) {
            field_[i][j] = l;
            change = true;
          }
        }
      }
    }
  }
  return std::pair<bool, bool>(true, change);
}

std::pair<bool, bool> Sudoku::add_colums() {
  bool change = false;
  for (int j = 0; j < 9; ++j) {
    int empty = 0;
    std::vector<std::vector<bool>> in_column(9);
    for (int i = 0; i < 9; ++i) {
      if (field_[i][j] == '#') {
        in_column[i] = possible(i, j);
        ++empty;
      } else {
        in_column[i] = std::vector<bool>(10, 0);
      }
    }
    std::vector<bool> pos_c_var(10, 0);
    std::vector<int> pos_c(10, 0);
    for (int l = 1; l < 10; ++l) {
      for (int i = 0; i < 9; ++i) {
        pos_c[l] += in_column[i][l];
        pos_c_var[l] = (pos_c_var[l] || in_column[i][l]);
      }
    }
    int sum = 0;
    for (int l = 1; l < 10; ++l) {
      sum += pos_c_var[l];
    }
    if (sum < empty) {
      return std::pair<bool, bool>(false, change);
    }
    for (int l = 1; l < 10; ++l) {
      if (pos_c[l] == 1) {
        for (int i = 0; i < 9; ++i) {
          if (in_column[i][l]) {
            field_[i][j] = l;
            change = true;
          }
        }
      }
    }
  }
  return std::pair<bool, bool>(true, change);
}

std::pair<bool, bool> Sudoku::add_squares() {
  bool change = false;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      std::vector<std::vector<std::vector<bool>>> in_sq(3);
      for (int k = 0; k < 3; ++k) {
        in_sq[k].resize(3, std::vector<bool>(10, 0));
      }
      int empty = 0;
      for (int i_c = 0; i_c < 3; ++i_c) {
        for (int j_c = 0; j_c < 3; ++j_c) {
          if (field_[i * 3 + i_c][j * 3 + j_c] == '#') {
            in_sq[i_c][j_c] = possible(i * 3 + i_c, j * 3 + j_c);
            ++empty;
          } else {
            in_sq[i_c][j_c] = std::vector<bool>(10, 0);
          }
        }
      }
      std::vector<int> pos_sq(10, 0);
      std::vector<bool> pos_sq_var(10, 0);
      for (int l = 1; l < 10; ++l) {
        for (int i_c = 0; i_c < 3; ++i_c) {
          for (int j_c = 0; j_c < 3; ++j_c) {
            pos_sq[l] += in_sq[i_c][j_c][l];
            pos_sq_var[l] = (pos_sq_var[l] || in_sq[i_c][j_c][l]);
          }
        }
      }
      int sum = 0;
      for (int l = 1; l < 10; ++l) {
        sum += pos_sq_var[l];
      }
      if (sum < empty) {
        return std::pair<bool, bool>(false, change);
      }
      for (int l = 1; l < 10; ++l) {
        if (pos_sq[l] == 1) {
          for (int i_c = 0; i_c < 3; ++i_c) {
            for (int j_c = 0; j_c < 3; ++j_c) {
              if (in_sq[i_c][j_c][l]) {
                field_[i * 3 + i_c][j * 3 + j_c] = l;
                change = true;
              }
            }
          }
        }
      }
    }
  }
  return std::pair<bool, bool>(true, change);
}

std::pair<bool, bool> Sudoku::add_rec() {
  bool change = false;
  for (int i = 0; i < 9; ++i) {
    for (int j = 0; j < 9; ++j) {
      if (field_[i][j] == '#') {
        std::vector<bool> pos = possible(i, j);
        for (int l = 1; l < 10; ++l) {
          if (pos[l]) {
            Sudoku copy = *this;
            copy.field_[i][j] = l;
            if (copy.solution()) {
              operator=(copy);
              return std::pair<bool, bool>(true, change);
            }
          }
        }
        return std::pair<bool, bool>(false, change);
      }
    }
  }
  return std::pair<bool, bool>(false, change);
}

bool Sudoku::solution() {
  while (!decided()) {
    std::pair<bool, bool> res = add_cells();
    if (!res.first) {
      return false;
    }
    if (!res.second) {
      bool change = false;
      res = add_lins();
      if (!res.first) {
        return false;
      }
      change = change || res.second;
      res = add_colums();
      if (!res.first) {
        return false;
      }
      change = change || res.second;
      res = add_squares();
      if (!res.first) {
        return false;
      }
      change = change || res.second;
      if (!change) {
        res = add_rec();
        if (!res.first) {
          return false;
        }
      }
    }
  }
  return true;
}

short int& Sudoku::Get(const int& i, const int& j) const {
  return field_[i][j];
}

Sudoku::~Sudoku() {
  for (int i = 0; i < 9; ++i) {
    delete[] field_[i];
  }
  delete[] field_;
}

std::ostream& operator<<(std::ostream& out, const Sudoku& sud) {
  for (int i = 0; i < 9; ++i) {
    if (i % 3 == 0) {
      out << std::endl;
    }
    for (int j = 0; j < 9; ++j) {
      if (j % 3 == 0) {
        out << '\t';
      }
      if (sud.Get(i, j) != '#') {
        out << sud.Get(i, j) << " ";
      } else {
        out << "# ";
      }
    }
    std::cout << std::endl;
  }
  return out;
}

std::istream& operator>>(std::istream& input, Sudoku& sud) {
  short int d[9][9];
  for (int i = 0; i < 9; ++i) {
    for (int j = 0; j < 9; ++j) {
      char buf;
      input >> buf;
      d[i][j] = (buf == '#') ? '#' : (buf - '0');
    }
  }
  Sudoku tmp = d;
  sud = tmp;
  return input;
}

int main() {
  short int a[9][9] = {{5, 3, '#', '#', 7, '#', '#', '#', '#'},
                       {6, '#', '#', 1, 9, 5, '#', '#', '#'},
                       {'#', 9, 8, '#', '#', '#', '#', 6, '#'},
                       {8, '#', '#', '#', 6, '#', '#', '#', 3},
                       {4, '#', '#', 8, '#', 3, '#', '#', 1},
                       {7, '#', '#', '#', 2, '#', '#', '#', 6},
                       {'#', 6, '#', '#', '#', '#', 2, 8, '#'},
                       {'#', '#', '#', 4, 1, 9, '#', '#', 5},
                       {'#', '#', '#', '#', 8, '#', '#', 7, 9}};
  short int b[9][9] = {{9, 3, 4, 1, 8, 2, 5, 6, '#'},
                       {2, '#', '#', '#', '#', '#', '#', '#', '#'},
                       {8, 6, 1, 7, 3, 5, 4, '#', '#'},
                       {'#', 2, 8, '#', '#', 7, '#', 4, '#'},
                       {'#', 7, 6, '#', '#', 8, 9, 2, 1},
                       {5, '#', 9, 4, 2, 6, 3, 7, 8},
                       {6, 9, 5, '#', '#', '#', 8, '#', '#'},
                       {7, 8, '#', 5, 4, 9, 2, '#', 6},
                       {1, '#', '#', '#', 6, 3, 7, 5, '#'}};
  short int c[9][9] = {{3, '#', 7, 5, 9, '#', 6, '#', '#'},
                       {'#', '#', '#', '#', '#', 8, 7, 5, '#'},
                       {2, 6, '#', '#', '#', 1, '#', '#', '#'},
                       {8, 3, '#', 4, 2, 9, 5, 7, 1},
                       {'#', '#', '#', '#', '#', '#', '#', '#', 3},
                       {'#', '#', '#', '#', 1, '#', '#', 2, 6},
                       {5, 1, '#', 2, 8, 6, '#', '#', 7},
                       {6, '#', '#', '#', '#', '#', '#', 9, 8},
                       {'#', '#', 8, 9, 7, 3, '#', '#', '#'}};
  short int d[9][9] = {{'#', '#', '#', '#', '#', '#', '#', '#', '#'},
                       {'#', '#', '#', '#', '#', '#', '#', '#', '#'},
                       {'#', '#', '#', '#', '#', '#', '#', '#', '#'},
                       {'#', '#', '#', '#', '#', '#', '#', '#', '#'},
                       {'#', '#', '#', '#', '#', '#', '#', '#', '#'},
                       {'#', '#', '#', '#', '#', '#', '#', '#', '#'},
                       {'#', '#', '#', '#', '#', '#', '#', '#', '#'},
                       {'#', '#', '#', '#', '#', '#', '#', '#', '#'},
                       {'#', '#', '#', '#', '#', '#', '#', '#', '#'}};

  short int e[9][9] = {{7, '#', 4, '#', '#', '#', '#', '#', '#'},
                       {'#', '#', '#', '#', 5, '#', 9, '#', '#'},
                       {6, '#', '#', '#', '#', '#', '#', '#', '#'},
                       {'#', 9, '#', '#', '#', 8, 5, '#', '#'},
                       {'#', '#', '#', 3, '#', '#', '#', 7, '#'},
                       {'#', '#', '#', 6, '#', '#', '#', '#', '#'},
                       {'#', '#', '#', '#', '#', '#', 8, 6, 1},
                       {'#', '#', '#', 4, 7, '#', '#', '#', '#'},
                       {'#', '#', '#', '#', '#', '#', '#', '#', 2}};

  Sudoku f;
  std::cin >> f;
  std::cout << f;
  std::cout << f.solution() << std::endl;
  std::cout << f;
}
/*
        7 # 4   # # #   # # #
        # # #   # 5 #   9 # #
        6 # #   # # #   # # #

        # 9 #   # # 8   5 # #
        # # #   3 # #   # 7 #
        # # #   6 # #   # # #

        # # #   # # #   8 6 1
        # # #   4 7 #   # # #
        # # #   # # #   # # 2

7#4 ### ###
### #5# 9##
6## ### ###

#9# ##8 5##
### 3## #7#
### 6## ###

### ### 861
### 47# ###
### ### ##2
*/
