#include <cstddef>
#include <cstdint>
#include <execution>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <vector>

template <typename T>
class Deque {
 public:
  template <bool IsConst, bool IsRevers>
  class CommonIterator;

  using iterator = CommonIterator<false, false>;
  using const_iterator = CommonIterator<true, false>;
  using reverse_iterator = CommonIterator<false, true>;
  using const_reverse_iterator = CommonIterator<true, true>;

  Deque();
  Deque(const Deque<T>& other);
  Deque(size_t count);
  Deque(size_t count, const T& value);
  void push_back(const T& value);
  void push_front(const T& value);
  void pop_back();
  void pop_front();
  Deque<T>& operator=(const Deque<T>& other);
  size_t size() const;
  bool empty() const;
  const T& operator[](int number_el) const;
  T& operator[](int number_el);
  const T& at(size_t number_el) const;
  T& at(size_t number_el);

  iterator begin() {
    iterator tmp;
    if (tail_empty_) {
      tmp = iterator(-count_tail_ + 2, this);
    } else if (count_tail_ == 0) {
      tmp = iterator(1, this);
    } else {
      tmp = iterator(-count_tail_, this);
    }
    return tmp;
  }

  const_iterator cbegin() const {
    const_iterator tmp;
    if (tail_empty_) {
      tmp = const_iterator(-count_tail_ + 2, this);
    } else if (count_tail_ == 0) {
      tmp = const_iterator(1, this);
    } else {
      tmp = const_iterator(-count_tail_, this);
    }
    return tmp;
  }

  reverse_iterator rbegin() {
    auto tmp = end() - 1;
    return tmp;
  }

  const_reverse_iterator crbegin() const {
    auto tmp = cend() - 1;
    return tmp;
  }

  iterator end() {
    iterator tmp;
    if (head_empty_) {
      tmp = iterator(count_head_ - 1, this);
    } else if (count_head_ == 0) {
      tmp = iterator(1, this);
    } else {
      tmp = iterator(count_head_ + 1, this);
    }
    return tmp;
  }

  const_iterator cend() const {
    const_iterator tmp;
    if (head_empty_) {
      tmp = const_iterator(count_head_ - 1, this);
    } else if (count_head_ == 0) {
      tmp = const_iterator(1, this);
    } else {
      tmp = const_iterator(count_head_ + 1, this);
    }
    return tmp;
  }

  reverse_iterator rend() {
    auto tmp = begin() - 1;
    return tmp;
  }

  const_reverse_iterator crend() const {
    auto tmp = cbegin() - 1;
    return tmp;
  }

  void insert(iterator iter, const T& value);
  void erase(iterator iter);
  ~Deque();

 private:
  int count_head_;
  int count_tail_;
  std::vector<T*> head_;
  std::vector<T*> tail_;
  bool head_empty_;
  bool tail_empty_;
  const int kBase = 16384;

  void del_in_tail(int number);
  void del_in_head(int number);
  void del_head();
  void push_back_in_head(const T& value);
  void push_front_in_tail(const T& value);
  void clear();
  void copy(const Deque<T>& other);
};

template <typename T>
void Deque<T>::del_in_tail(int number) {
  delete[] reinterpret_cast<int8_t*>(tail_[number]);
  tail_[number] = nullptr;
}

template <typename T>
void Deque<T>::del_in_head(int number) {
  delete[] reinterpret_cast<int8_t*>(head_[number]);
  head_[number] = nullptr;
}

template <typename T>
void Deque<T>::del_head() {
  if (!tail_empty_) {
    while (count_head_ > 0) {
      count_head_--;
      (head_[count_head_ / kBase] + (count_head_ % kBase))->~T();
      if (count_head_ % kBase == 0) {
        del_in_head(count_head_ / kBase);
      }
    }
    while (count_tail_ > 0) {
      count_tail_--;
      (tail_[count_tail_ / kBase] + (count_tail_ % kBase))->~T();
      if (count_tail_ % kBase == 0) {
        del_in_tail(count_tail_ / kBase);
      }
    }
  } else {
    while (count_head_ + count_tail_ - 1 > 0) {
      count_head_--;
      (head_[count_head_ / kBase] + (count_head_ % kBase))->~T();
      if (count_head_ % kBase == 0) {
        del_in_head(count_head_ / kBase);
      }
    }
    if (count_head_ % kBase != 0) {
      del_in_head(count_head_ / kBase);
    }
  }
}

template <typename T>
void Deque<T>::push_back_in_head(const T& value) {
  if (static_cast<int>(head_.capacity()) <= (count_head_ / kBase) &&
      (count_head_ % kBase) == 0) {
    std::vector<T*> copy(head_.capacity());
    for (int i = count_head_ / kBase - 1; i >= 0; --i) {
      copy[i] = head_[i];
    }
    head_.reserve((2 * 2 * 2) * count_head_);
    for (int i = count_head_ / kBase - 1; i >= 0; --i) {
      head_[i] = copy[i];
    }
  }
  try {
    if (count_head_ % kBase == 0) {
      head_[count_head_ / kBase] =
          reinterpret_cast<T*>(new int8_t[sizeof(T) * kBase]);
    }
    new (head_[count_head_ / kBase] + (count_head_ % kBase)) T(value);
    ++count_head_;
  } catch (...) {
    (head_[count_head_ / kBase] + (count_head_ % kBase))->~T();
    if (count_head_ % kBase == 0) {
      del_in_head(count_head_ / kBase);
    }
    throw;
  }
}

template <typename T>
void Deque<T>::push_front_in_tail(const T& value) {
  if (static_cast<int>(tail_.capacity()) <= (count_tail_ / kBase) &&
      (count_tail_ % kBase) == 0) {
    std::vector<T*> copy(tail_.capacity());
    for (int i = count_tail_ / kBase - 1; i >= 0; --i) {
      copy[i] = tail_[i];
    }
    tail_.reserve((2 * 2 * 2) * count_tail_);
    for (int i = count_tail_ / kBase - 1; i >= 0; --i) {
      tail_[i] = copy[i];
    }
  }
  try {
    if (count_tail_ % kBase == 0) {
      tail_[count_tail_ / kBase] =
          reinterpret_cast<T*>(new int8_t[sizeof(T) * kBase]);
    }
    new (tail_[count_tail_ / kBase] + (count_tail_ % kBase)) T(value);
    ++count_tail_;
  } catch (...) {
    (tail_[count_tail_ / kBase] + (count_tail_ % kBase))->~T();
    if (count_tail_ % kBase == 0) {
      del_in_tail(count_tail_ / kBase);
    }
    throw;
  }
}

template <typename T>
void Deque<T>::clear() {
  if (!head_empty_) {
    del_head();
  } else {
    bool flag = true;
    while (count_head_ + count_tail_ - 1 > 0) {
      flag = false;
      count_tail_--;
      (tail_[count_tail_ / kBase] + (count_tail_ % kBase))->~T();
      if (count_tail_ % kBase == 0) {
        del_in_tail(count_tail_ / kBase);
      }
    }
    if (flag) {
      del_in_tail(count_tail_ / kBase);
    }
  }
  head_empty_ = false;
  tail_empty_ = false;
  count_head_ = 0;
  count_tail_ = 0;
}

template <typename T>
void Deque<T>::copy(const Deque<T>& other) {
  count_head_ = other.count_head_;
  count_tail_ = other.count_tail_;
  head_empty_ = other.head_empty_;
  tail_empty_ = other.tail_empty_;
  if (other.count_head_ > 0 && !other.head_empty_) {
    head_.reserve(count_head_ / kBase);
    for (int i = 0; i < count_head_; ++i) {
      if (i % kBase == 0) {
        head_[i / kBase] = reinterpret_cast<T*>(new int8_t[sizeof(T) * kBase]);
      }
      new (head_[i / kBase] + (i % kBase)) T(other.head_[i / kBase][i % kBase]);
    }
  }
  if (other.count_tail_ > 0 && !other.tail_empty_) {
    tail_.reserve(count_tail_);
    for (int i = 0; i < count_tail_; ++i) {
      if (i % kBase == 0) {
        tail_[i / kBase] = reinterpret_cast<T*>(new int8_t[sizeof(T) * kBase]);
      }
      new (tail_[i / kBase] + (i % kBase)) T(other.tail_[i / kBase][i % kBase]);
    }
  }
}

template <typename T>
Deque<T>::Deque() {
  count_head_ = 0;
  count_tail_ = 0;
  head_empty_ = false;
  tail_empty_ = false;
  head_.reserve(2);
  tail_.reserve(2);
}

template <typename T>
Deque<T>::Deque(const Deque<T>& other) {
  copy(other);
}

template <typename T>
Deque<T>::Deque(size_t count) {
  head_.reserve(2);
  tail_.reserve(2);
  count_tail_ = 0;
  count_head_ = static_cast<int>(count);
  head_.reserve(count);
  int number = 0;
  try {
    head_.reserve(count);
    for (number = 0; number < static_cast<int>(count); ++number) {
      if (number % kBase == 0) {
        head_[number / kBase] =
            reinterpret_cast<T*>(new int8_t[sizeof(T) * kBase]);
      }
      new (head_[number / kBase] + (number % kBase)) T(0);
    }
  } catch (...) {
    for (; number >= 0; --number) {
      (head_[number / kBase] + (number % kBase))->~T();
      if (count_head_ % kBase != 0) {
        del_in_head(number / kBase);
      }
    }
    throw;
  }
  head_empty_ = false;
  tail_empty_ = false;
}

template <typename T>
Deque<T>::Deque(size_t count, const T& value) {
  head_.reserve(2);
  tail_.reserve(2);
  count_tail_ = 0;
  int number = 0;
  try {
    head_.reserve(count);
    for (number = 0; number < static_cast<int>(count); ++number) {
      if (number % kBase == 0) {
        head_[number / kBase] =
            reinterpret_cast<T*>(new int8_t[sizeof(T) * kBase]);
      }
      new (head_[number / kBase] + (number % kBase)) T(value);
    }
  } catch (...) {
    for (; number >= 0; --number) {
      (head_[number / kBase] + (number % kBase))->~T();
      if (count_head_ % kBase != 0) {
        del_in_head(number / kBase);
      }
    }
    throw;
  }
  count_head_ = static_cast<int>(number);
  head_empty_ = false;
  tail_empty_ = false;
}

template <typename T>
void Deque<T>::push_back(const T& value) {
  if (count_head_ >= 0 && !head_empty_) {
    push_back_in_head(value);
  } else if (count_head_ < 0) {
    try {
      if (count_head_ % kBase == 0) {
        tail_[-(count_head_ / kBase)] =
            reinterpret_cast<T*>(new int8_t[sizeof(T) * kBase]);
      }
      new (tail_[-(count_head_ / kBase)] + (kBase - 1 + (count_head_ % kBase)))
          T(value);
      ++count_head_;
    } catch (...) {
      (tail_[-(count_head_ / kBase)] + (kBase - 1 + (count_head_ % kBase)))
          ->~T();
      if (count_head_ % kBase == 0) {
        del_in_tail(-(count_head_ / kBase));
      }
      throw;
    }
  } else if (head_empty_) {
    new (tail_[-(count_head_ / kBase)] + (kBase - 1 + (count_head_ % kBase)))
        T(value);
    head_empty_ = false;
  }
}

template <typename T>
void Deque<T>::push_front(const T& value) {
  if (count_tail_ >= 0 && !tail_empty_) {
    push_front_in_tail(value);
  } else if (count_tail_ < 0) {  //
    try {
      if (count_tail_ % kBase == 0) {
        head_[-(count_tail_ / kBase)] =
            reinterpret_cast<T*>(new int8_t[sizeof(T) * kBase]);
      }
      new (head_[-(count_tail_ / kBase)] + (kBase - 1 + (count_tail_ % kBase)))
          T(value);
      ++count_tail_;
    } catch (...) {
      (head_[-(count_tail_ / kBase)] + (kBase - 1 + (count_tail_ % kBase)))
          ->~T();
      if (count_tail_ % kBase == 0) {
        del_in_head(-(count_tail_ / kBase));
      }
      throw;
    }
  } else if (tail_empty_) {
    new (head_[-(count_tail_ / kBase)] + (kBase - 1 + (count_tail_ % kBase)))
        T(value);
    tail_empty_ = false;
  }
}

template <typename T>
void Deque<T>::pop_back() {
  if (count_head_ > 0) {
    --count_head_;
    (head_[count_head_ / kBase] + (count_head_ % kBase))->~T();
    if (count_head_ % kBase == 0) {
      del_in_head(count_head_ / kBase);
    }
  } else if (count_head_ <= 0 && head_empty_) {
    --count_head_;
    (tail_[-(count_head_ / kBase)] + (kBase - 1 + (count_head_ % kBase)))->~T();
    if (count_head_ % kBase == 0) {
      del_in_tail((-(kBase + count_head_) / kBase));
    }
  } else if (!head_empty_) {
    head_empty_ = true;
    (tail_[-(count_head_ / kBase)] + (kBase - 1 + (count_head_ % kBase)))->~T();
  }
}

template <typename T>
void Deque<T>::pop_front() {
  if (count_tail_ > 0) {
    --count_tail_;
    (tail_[count_tail_ / kBase] + (count_tail_ % kBase))->~T();
    if (count_tail_ % kBase == 0) {
      del_in_tail(count_tail_ / kBase);
    }
  } else if (count_tail_ <= 0 && tail_empty_) {
    --count_tail_;
    (head_[(-count_tail_ / kBase)] + (kBase - 1 + (count_tail_ % kBase)))->~T();
    if (count_tail_ % kBase == 0) {
      del_in_head((-(kBase + count_tail_) / kBase));
    }
  } else if (!tail_empty_) {
    tail_empty_ = true;
    (head_[(-count_tail_ / kBase)] + (kBase - 1 + (count_tail_ % kBase)))->~T();
  }
}

template <typename T>
Deque<T>& Deque<T>::operator=(const Deque<T>& other) {
  if (this == &other) {
    return *this;
  }
  clear();
  copy(other);
  return *this;
}

template <typename T>
size_t Deque<T>::size() const {
  if (!tail_empty_ && !head_empty_) {
    return static_cast<size_t>(count_head_ + count_tail_);
  }
  return static_cast<size_t>(count_head_ + count_tail_ - 1);
}

template <typename T>
bool Deque<T>::empty() const {
  if (!tail_empty_ && !head_empty_) {
    return (count_head_ + count_tail_) == 0;
  }
  return (count_head_ + count_tail_ - 1) == 0;
}

template <typename T>
const T& Deque<T>::operator[](int number_el) const {
  T* tmp;
  if (tail_empty_) {
    tmp = head_[(number_el - count_tail_ + 1) / kBase] +
          ((number_el - count_tail_ + 1) % kBase);
  } else if (count_tail_ <= number_el) {
    tmp = head_[(number_el - count_tail_) / kBase] +
          ((number_el - count_tail_) % kBase);
  } else {
    tmp = tail_[(count_tail_ - number_el - 1) / kBase] +
          ((count_tail_ - number_el - 1) % kBase);
  }
  return *tmp;
}

template <typename T>
T& Deque<T>::operator[](int number_el) {
  T* tmp;
  if (tail_empty_) {
    tmp = head_[(number_el - count_tail_ + 1) / kBase] +
          ((number_el - count_tail_ + 1) % kBase);
  } else if (count_tail_ <= number_el) {
    tmp = head_[(number_el - count_tail_) / kBase] +
          ((number_el - count_tail_) % kBase);
  } else {
    tmp = tail_[(count_tail_ - number_el - 1) / kBase] +
          ((count_tail_ - number_el - 1) % kBase);
  }
  return *tmp;
}

template <typename T>
const T& Deque<T>::at(size_t number_el) const {
  if (size() <= number_el) {
    throw std::out_of_range("");
  }
  return operator[](number_el);
}

template <typename T>
T& Deque<T>::at(size_t number_el) {
  if (size() <= number_el) {
    throw std::out_of_range("");
  }
  return operator[](number_el);
}

template <typename T>
void Deque<T>::insert(iterator iter, const T& value) {
  if (!empty()) {
    iterator end_it = end() - 1;
    push_back(*(end_it));
    while (end_it > iter) {
      *(end_it) = *(end_it - 1);
      --end_it;
    }
    *(iter) = value;
  } else {
    push_back(value);
  }
}

template <typename T>
void Deque<T>::erase(iterator iter) {
  iterator end_it = end() - 1;
  while (end_it > iter) {
    *(iter) = *(iter + 1);
    ++iter;
  }
  pop_back();
}

template <typename T>
Deque<T>::~Deque() {
  clear();
}

template <typename T>
template <bool IsConst, bool IsRevers>
class Deque<T>::CommonIterator
    : public std::iterator<std::random_access_iterator_tag, T> {
 private:
  std::conditional_t<IsConst, const Deque<T>*, Deque<T>*> deq_;
  int number_el_;

 public:
  using value_type = typename std::conditional<IsConst, const T, T>::type;
  using pointer = typename std::conditional<IsConst, const T*, T*>::type;
  using reference = typename std::conditional<IsConst, const T&, T&>::type;
  using iterator_category = std::random_access_iterator_tag;
  using difference_type = int;

  CommonIterator();

  CommonIterator(int number,
                 std::conditional_t<IsConst, const Deque<T>*, Deque<T>*> deq);

  CommonIterator(const CommonIterator<IsConst, false>& other);
  CommonIterator(const CommonIterator<IsConst, true>& other);

  // CommonIterator(const CommonReverseIterator<IsConst>& other);

  std::conditional_t<IsConst, const Deque<T>*, Deque<T>*> getdeq() const {
    return deq_;
  }

  int getnum() const { return number_el_; }

  CommonIterator<IsConst, IsRevers>& operator=(const CommonIterator& other) {
    deq_ = other.deq_;
    number_el_ = other.number_el_;
    return *this;
  }

  CommonIterator<IsConst, IsRevers>& operator++() {
    if (IsRevers) {
      if (number_el_ != 1) {
        --number_el_;
        return *this;
      }
      number_el_ = -1;
      return *this;
    }
    if (number_el_ != -1) {
      ++number_el_;
      return *this;
    }
    number_el_ = 1;
    return *this;
  }

  CommonIterator<IsConst, IsRevers> operator++(int) {
    CommonIterator<IsConst, IsRevers> copy = *this;
    if (IsRevers) {
      if (number_el_ != 1) {
        --number_el_;
        return copy;
      }
      number_el_ = -1;
      return copy;
    }
    if (number_el_ != -1) {
      ++number_el_;
      return copy;
    }
    number_el_ = 1;
    return copy;
  }

  CommonIterator<IsConst, IsRevers>& operator--() {
    if (IsRevers) {
      if (number_el_ != -1) {
        ++number_el_;
        return *this;
      }
      number_el_ = 1;
      return *this;
    }
    if (number_el_ != 1) {
      --number_el_;
      return *this;
    }
    number_el_ = -1;
    return *this;
  }

  CommonIterator<IsConst, IsRevers> operator--(int) {
    CommonIterator<IsConst, IsRevers> copy = *this;
    if (IsRevers) {
      if (number_el_ != -1) {
        ++number_el_;
        return copy;
      }
      number_el_ = 1;
      return copy;
    }
    if (number_el_ != 1) {
      --number_el_;
      return copy;
    }
    number_el_ = -1;
    return copy;
  }

  CommonIterator<IsConst, IsRevers>& operator+=(const int& number) {
    if (IsRevers) {
      if (number_el_ > 0 && number_el_ <= number) {
        number_el_ -= (number + 1);
        return *this;
      }
      number_el_ -= number;
      return *this;
    }
    if (number_el_ < 0 && -number_el_ <= number) {
      number_el_ += (number + 1);
      return *this;
    }
    number_el_ += number;
    return *this;
  }

  CommonIterator<IsConst, IsRevers> operator+(const int& number) const {
    CommonIterator<IsConst, IsRevers> sum = *this;
    sum += number;
    return sum;
  }

  CommonIterator<IsConst, IsRevers>& operator-=(const int& number) {
    if (IsRevers) {
      if (number_el_ < 0 && -number_el_ <= number) {
        number_el_ += (number + 1);
        return *this;
      }
      number_el_ += number;
      return *this;
    }
    if (number_el_ > 0 && number_el_ <= number) {
      number_el_ -= (number + 1);
      return *this;
    }
    number_el_ -= number;
    return *this;
  }

  CommonIterator<IsConst, IsRevers> operator-(const int& number) const {
    CommonIterator<IsConst, IsRevers> sum = *this;
    sum -= number;
    return sum;
  }

  bool operator<(const CommonIterator& other) const {
    if (IsRevers) {
      return number_el_ > other.number_el_;
    }
    return number_el_ < other.number_el_;
  }

  bool operator>(const CommonIterator& other) const {
    if (IsRevers) {
      return number_el_ < other.number_el_;
    }
    return number_el_ > other.number_el_;
  }

  bool operator==(const CommonIterator& other) const {
    return (!(*this < other) && !(*this > other));
  }

  bool operator<=(const CommonIterator& other) const {
    if (IsRevers) {
      return !(*this < other);
    }
    return !(*this > other);
  }

  bool operator>=(const CommonIterator& other) const {
    if (IsRevers) {
      return !(*this > other);
    }
    return !(*this < other);
  }

  bool operator!=(const CommonIterator& other) const {
    return !(*this == other);
  }

  int operator-(const CommonIterator& other) const {
    if (IsRevers) {
      return other.number_el_ - number_el_;
    }
    return number_el_ - other.number_el_;
  }

  std::conditional_t<IsConst, const T&, T&> operator*() {
    if (IsRevers) {
      auto iter = deq_->rend() - 1;
      size_t num = iter - *this;
      return (deq_->operator[](num));
    }
    auto iter = deq_->begin();
    return (deq_->operator[](*this - iter));
  }

  std::conditional_t<IsConst, const T*, T*> operator->() {
    if (IsRevers) {
      auto iter = deq_->rend() - 1;
      size_t num = iter - *this;
      return &(deq_->operator[](num));
    }
    auto iter = deq_->begin();
    return &(deq_->operator[](*this - iter));
  }
};

template <typename T>
template <bool IsConst, bool IsRevers>
Deque<T>::CommonIterator<IsConst, IsRevers>::CommonIterator(
    int number, std::conditional_t<IsConst, const Deque<T>*, Deque<T>*> deq) {
  number_el_ = number;
  deq_ = deq;
}

template <typename T>
template <bool IsConst, bool IsRevers>
Deque<T>::CommonIterator<IsConst, IsRevers>::CommonIterator() {
  number_el_ = 0;
}

template <typename T>
template <bool IsConst, bool IsRevers>
Deque<T>::CommonIterator<IsConst, IsRevers>::CommonIterator(
    const CommonIterator<IsConst, false>& other) {
  deq_ = other.getdeq();
  number_el_ = other.getnum();
}

template <typename T>
template <bool IsConst, bool IsRevers>
Deque<T>::CommonIterator<IsConst, IsRevers>::CommonIterator(
    const CommonIterator<IsConst, true>& other) {
  deq_ = other.getdeq();
  number_el_ = other.getnum();
}
