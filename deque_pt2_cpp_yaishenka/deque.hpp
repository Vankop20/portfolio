#include <cstddef>
#include <cstdint>
#include <execution>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <vector>

template <typename T, typename Allocator = std::allocator<T>>
class Deque {
 public:
  template <bool IsConst, bool IsRevers>
  class CommonIterator;

  using iterator = CommonIterator<false, false>;
  using const_iterator = CommonIterator<true, false>;
  using reverse_iterator = CommonIterator<false, true>;
  using const_reverse_iterator = CommonIterator<true, true>;
  using alloc_traits = std::allocator_traits<Allocator>;
  using value_type = T;
  using allocator_type = Allocator;

  Deque();
  Deque(const Allocator& alloc);
  Deque(const Deque<T, Allocator>& other);
  Deque(size_t count, const Allocator& alloc = Allocator());
  Deque(size_t count, const T& value, const Allocator& alloc = Allocator());
  Deque(Deque&& other);
  Deque(std::initializer_list<T> init, const Allocator& alloc = Allocator());
  void push_back(const T& value);
  void push_front(const T& value);
  void push_back(T&& value);
  void push_front(T&& value);
  template <typename... Args>
  void emplace_back_in_head(const Args&... args);
  template <typename... Args>
  void emplace_back(const Args&... args);
  template <typename... Args>
  void emplace_front_in_tail(const Args&... args);
  template <typename... Args>
  void emplace_front(const Args&... args);
  void pop_back();
  void pop_front();
  Deque<T, Allocator>& operator=(const Deque<T, Allocator>& other);
  Deque<T, Allocator>& operator=(Deque&& other);
  size_t size() const;
  bool empty() const;
  const T& operator[](int number_el) const;
  T& operator[](int number_el);
  const T& at(size_t number_el) const;
  T& at(size_t number_el);
  iterator begin();
  const_iterator cbegin();
  reverse_iterator rbegin();
  const_reverse_iterator crbegin();
  iterator end();
  const_iterator cend();
  reverse_iterator rend();
  const_reverse_iterator crend();
  void insert(iterator iter, const T& value);
  void erase(iterator iter);
  Allocator get_allocator() const;
  ~Deque();

 private:
  int count_head_;
  int count_tail_;
  std::vector<T*> head_;
  std::vector<T*> tail_;
  bool head_empty_;
  bool tail_empty_;
  bool moved_ = false;
  const int kBase = 16384;
  Allocator alloc_t_;

  void del_in_tail(int number);
  void del_in_head(int number);
  void del_head();
  void push_back_in_head(const T& value);
  void push_front_in_tail(const T& value);
  void push_back_in_head(T&& value);
  void push_front_in_tail(T&& value);
  void clear();
  void copy(const Deque<T, Allocator>& other);
  void copy_head(const Deque<T, Allocator>& other);
  void copy_tail(const Deque<T, Allocator>& other);
};

template <typename T, typename Allocator>
template <typename... Args>
void Deque<T, Allocator>::emplace_back_in_head(const Args&... args) {
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
      head_[count_head_ / kBase] = alloc_traits::allocate(alloc_t_, kBase);
    }
    alloc_traits::construct(
        alloc_t_, head_[count_head_ / kBase] + (count_head_ % kBase), args...);
    ++count_head_;
  } catch (...) {
    alloc_traits::destroy(alloc_t_,
                          (head_[count_head_ / kBase] + (count_head_ % kBase)));
    if (count_head_ % kBase == 0) {
      del_in_head(count_head_ / kBase);
    }
    throw;
  }
}

template <typename T, typename Allocator>
template <typename... Args>
void Deque<T, Allocator>::emplace_back(const Args&... args) {
  if (count_head_ >= 0 && !head_empty_) {
    emplace_back_in_head(args...);
  } else if (count_head_ < 0) {
    try {
      if (count_head_ % kBase == 0) {
        tail_[-(count_head_ / kBase)] = alloc_traits::allocate(alloc_t_, kBase);
      }
      alloc_traits::construct(
          alloc_t_,
          tail_[-(count_head_ / kBase)] + (kBase - 1 + (count_head_ % kBase)),
          args...);
      ++count_head_;
    } catch (...) {
      alloc_traits::destroy(alloc_t_, (tail_[-(count_head_ / kBase)] +
                                       (kBase - 1 + (count_head_ % kBase))));
      if (count_head_ % kBase == 0) {
        del_in_tail(-(count_head_ / kBase));
      }
      throw;
    }
  } else if (head_empty_) {
    alloc_traits::construct(
        alloc_t_,
        tail_[-(count_head_ / kBase)] + (kBase - 1 + (count_head_ % kBase)),
        args...);
    head_empty_ = false;
  }
}

template <typename T, typename Allocator>
template <typename... Args>
void Deque<T, Allocator>::emplace_front_in_tail(const Args&... args) {
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
      tail_[count_tail_ / kBase] = alloc_traits::allocate(alloc_t_, kBase);
    }
    alloc_traits::construct(
        alloc_t_, tail_[count_tail_ / kBase] + (count_tail_ % kBase), args...);
    ++count_tail_;
  } catch (...) {
    alloc_traits::destroy(alloc_t_,
                          (tail_[count_tail_ / kBase] + (count_tail_ % kBase)));
    if (count_tail_ % kBase == 0) {
      del_in_tail(count_tail_ / kBase);
    }
    throw;
  }
}

template <typename T, typename Allocator>
template <typename... Args>
void Deque<T, Allocator>::emplace_front(const Args&... args) {
  if (count_tail_ >= 0 && !tail_empty_) {
    emplace_front_in_tail(args...);
  } else if (count_tail_ < 0) {  //
    try {
      if (count_tail_ % kBase == 0) {
        head_[-(count_tail_ / kBase)] = alloc_traits::allocate(alloc_t_, kBase);
      }
      alloc_traits::construct(
          alloc_t_,
          head_[-(count_tail_ / kBase)] + (kBase - 1 + (count_tail_ % kBase)),
          args...);
      ++count_tail_;
    } catch (...) {
      alloc_traits::destroy(alloc_t_, (head_[-(count_tail_ / kBase)] +
                                       (kBase - 1 + (count_tail_ % kBase))));
      if (count_tail_ % kBase == 0) {
        del_in_head(-(count_tail_ / kBase));
      }
      throw;
    }
  } else if (tail_empty_) {
    alloc_traits::construct(
        alloc_t_,
        head_[-(count_tail_ / kBase)] + (kBase - 1 + (count_tail_ % kBase)),
        args...);
    tail_empty_ = false;
  }
}

template <typename T, typename Allocator>
Allocator Deque<T, Allocator>::get_allocator() const {
  return alloc_t_;
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::del_in_tail(int number) {
  alloc_traits::deallocate(alloc_t_, tail_[number], kBase);
  tail_[number] = nullptr;
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::del_in_head(int number) {
  alloc_traits::deallocate(alloc_t_, head_[number], kBase);
  head_[number] = nullptr;
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::del_head() {
  if (!tail_empty_) {
    while (count_head_ > 0) {
      count_head_--;
      alloc_traits::destroy(
          alloc_t_, (head_[count_head_ / kBase] + (count_head_ % kBase)));
      if (count_head_ % kBase == 0) {
        del_in_head(count_head_ / kBase);
      }
    }
    while (count_tail_ > 0) {
      count_tail_--;
      alloc_traits::destroy(
          alloc_t_, (tail_[count_tail_ / kBase] + (count_tail_ % kBase)));
      if (count_tail_ % kBase == 0) {
        del_in_tail(count_tail_ / kBase);
      }
    }
  } else {
    while (count_head_ + count_tail_ - 1 > 0) {
      count_head_--;
      alloc_traits::destroy(
          alloc_t_, (head_[count_head_ / kBase] + (count_head_ % kBase)));

      if (count_head_ % kBase == 0) {
        del_in_head(count_head_ / kBase);
      }
    }
    if (count_head_ % kBase != 0) {
      del_in_head(count_head_ / kBase);
    }
  }
}

template <typename T, typename Allocator>
typename Deque<T, Allocator>::iterator Deque<T, Allocator>::begin() {
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

template <typename T, typename Allocator>
typename Deque<T, Allocator>::const_iterator Deque<T, Allocator>::cbegin() {
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

template <typename T, typename Allocator>
typename Deque<T, Allocator>::reverse_iterator Deque<T, Allocator>::rbegin() {
  reverse_iterator tmp;
  if (head_empty_) {
    tmp = reverse_iterator(count_head_ - 1, this);
  } else if (count_head_ == 0) {
    tmp = reverse_iterator(-1, this);
  } else {
    tmp = reverse_iterator(count_head_, this);
  }
  return tmp;
}

template <typename T, typename Allocator>
typename Deque<T, Allocator>::const_reverse_iterator
Deque<T, Allocator>::crbegin() {
  const_reverse_iterator tmp;
  if (head_empty_) {
    tmp = const_reverse_iterator(count_head_ - 1, this);
  } else if (count_head_ == 0) {
    tmp = const_reverse_iterator(-1, this);
  } else {
    tmp = const_reverse_iterator(count_head_, this);
  }
  return tmp;
}

template <typename T, typename Allocator>
typename Deque<T, Allocator>::iterator Deque<T, Allocator>::end() {
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

template <typename T, typename Allocator>
typename Deque<T, Allocator>::const_iterator Deque<T, Allocator>::cend() {
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

template <typename T, typename Allocator>
typename Deque<T, Allocator>::reverse_iterator Deque<T, Allocator>::rend() {
  reverse_iterator tmp;
  if (tail_empty_) {
    tmp = reverse_iterator(-count_tail_ + 1, this);
  } else {
    tmp = reverse_iterator(-count_tail_ - 1, this);
  }
  return tmp;
}

template <typename T, typename Allocator>
typename Deque<T, Allocator>::const_reverse_iterator
Deque<T, Allocator>::crend() {
  const_reverse_iterator tmp;
  if (tail_empty_) {
    tmp = const_reverse_iterator(-count_tail_ + 1, this);
  } else if (count_tail_ == 0) {
    tmp = const_reverse_iterator(-1, this);
  } else {
    tmp = const_reverse_iterator(-count_tail_ - 1, this);
  }
  return tmp;
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::push_back_in_head(const T& value) {
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
      head_[count_head_ / kBase] = alloc_traits::allocate(alloc_t_, kBase);
    }
    alloc_traits::construct(
        alloc_t_, head_[count_head_ / kBase] + (count_head_ % kBase), value);
    ++count_head_;
  } catch (...) {
    alloc_traits::destroy(alloc_t_,
                          (head_[count_head_ / kBase] + (count_head_ % kBase)));
    if (count_head_ % kBase == 0) {
      del_in_head(count_head_ / kBase);
    }
    throw;
  }
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::push_front_in_tail(const T& value) {
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
      tail_[count_tail_ / kBase] = alloc_traits::allocate(alloc_t_, kBase);
    }
    alloc_traits::construct(
        alloc_t_, tail_[count_tail_ / kBase] + (count_tail_ % kBase), value);
    ++count_tail_;
  } catch (...) {
    alloc_traits::destroy(alloc_t_,
                          (tail_[count_tail_ / kBase] + (count_tail_ % kBase)));
    if (count_tail_ % kBase == 0) {
      del_in_tail(count_tail_ / kBase);
    }
    throw;
  }
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::push_back_in_head(T&& value) {
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
      head_[count_head_ / kBase] = alloc_traits::allocate(alloc_t_, kBase);
    }
    alloc_traits::construct(alloc_t_,
                            head_[count_head_ / kBase] + (count_head_ % kBase),
                            std::move(value));
    ++count_head_;
  } catch (...) {
    alloc_traits::destroy(alloc_t_,
                          (head_[count_head_ / kBase] + (count_head_ % kBase)));
    if (count_head_ % kBase == 0) {
      del_in_head(count_head_ / kBase);
    }
    throw;
  }
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::push_front_in_tail(T&& value) {
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
      tail_[count_tail_ / kBase] = alloc_traits::allocate(alloc_t_, kBase);
    }
    alloc_traits::construct(alloc_t_,
                            tail_[count_tail_ / kBase] + (count_tail_ % kBase),
                            std::move(value));
    ++count_tail_;
  } catch (...) {
    alloc_traits::destroy(alloc_t_,
                          (tail_[count_tail_ / kBase] + (count_tail_ % kBase)));
    if (count_tail_ % kBase == 0) {
      del_in_tail(count_tail_ / kBase);
    }
    throw;
  }
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::clear() {
  if (!head_empty_) {
    del_head();
  } else {
    bool flag = true;
    while (count_head_ + count_tail_ - 1 > 0) {
      flag = false;
      count_tail_--;
      alloc_traits::destroy(
          alloc_t_, (tail_[count_tail_ / kBase] + (count_tail_ % kBase)));
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

template <typename T, typename Allocator>
void Deque<T, Allocator>::copy_head(const Deque<T, Allocator>& other) {
  head_.reserve(count_head_ / kBase + 1);
  if (!(count_tail_ >= 0 && !tail_empty_)) {
    head_[0] = alloc_traits::allocate(alloc_t_, kBase);
  }
  for (int i = (count_tail_ >= 0 && !tail_empty_) ? 0 : -count_tail_ + 1;
       i < count_head_; ++i) {
    try {
      if (i % kBase == 0) {
        head_[i / kBase] = alloc_traits::allocate(alloc_t_, kBase);
      }
      alloc_traits::construct(alloc_t_, head_[i / kBase] + (i % kBase),
                              other.head_[i / kBase][i % kBase]);
    } catch (...) {
      for (--i; i >= 0; --i) {
        alloc_traits::destroy(alloc_t_, (head_[i / kBase] + (i % kBase)));
        if (i % kBase == 0) {
          del_in_head(i / kBase);
        }
      }
      moved_ = true;
      count_head_ = 0;
      throw;
    }
  }
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::copy_tail(const Deque<T, Allocator>& other) {
  tail_.reserve(count_tail_ / kBase + 1);
  if (!(count_head_ >= 0 && !head_empty_)) {
    head_[0] = alloc_traits::allocate(alloc_t_, kBase);
  }
  for (int i = (count_head_ >= 0 && !head_empty_) ? 0 : -count_head_ + 1;
       i < count_tail_; ++i) {
    try {
      if (i % kBase == 0) {
        tail_[i / kBase] = alloc_traits::allocate(alloc_t_, kBase);
      }
      alloc_traits::construct(alloc_t_, tail_[i / kBase] + (i % kBase),
                              other.tail_[i / kBase][i % kBase]);
    } catch (...) {
      for (--i; i >= 0; --i) {
        alloc_traits::destroy(alloc_t_, (tail_[i / kBase] + (i % kBase)));
        if (i % kBase == 0) {
          del_in_tail(i / kBase);
        }
      }
      moved_ = true;
      count_tail_ = 0;
      throw;
    }
  }
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::copy(const Deque<T, Allocator>& other) {
  alloc_t_ =
      alloc_traits::select_on_container_copy_construction(other.alloc_t_);
  count_head_ = other.count_head_;
  count_tail_ = other.count_tail_;
  head_empty_ = other.head_empty_;
  tail_empty_ = other.tail_empty_;
  if (other.count_head_ > 0 && !other.head_empty_) {
    copy_head(other);
  }
  if (other.count_tail_ > 0 && !other.tail_empty_) {
    copy_tail(other);
  }
}

template <typename T, typename Allocator>
Deque<T, Allocator>::Deque() {
  alloc_t_ = Allocator();
  count_head_ = 0;
  count_tail_ = 0;
  head_empty_ = false;
  tail_empty_ = false;
  head_.reserve(2);
  tail_.reserve(2);
}

template <typename T, typename Allocator>
Deque<T, Allocator>::Deque(const Allocator& alloc) {
  alloc_t_ = alloc;
  count_head_ = 0;
  count_tail_ = 0;
  head_empty_ = false;
  tail_empty_ = false;
  head_.reserve(2);
  tail_.reserve(2);
}

template <typename T, typename Allocator>
Deque<T, Allocator>::Deque(const Deque<T, Allocator>& other) {
  try {
    copy(other);
  } catch (...) {
    count_tail_ = 0;
    count_head_ = 0;
    tail_empty_ = false;
    head_empty_ = false;
    throw;
  }
}

template <typename T, typename Allocator>
Deque<T, Allocator>::Deque(size_t count, const Allocator& alloc) {
  alloc_t_ = alloc;
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
        head_[number / kBase] = alloc_traits::allocate(alloc_t_, kBase);
      }
      alloc_traits::construct(alloc_t_,
                              head_[number / kBase] + (number % kBase));
    }
  } catch (...) {
    for (--number; number >= 0; --number) {
      alloc_traits::destroy(alloc_t_,
                            (head_[number / kBase] + (number % kBase)));
      if (number % kBase == 0) {
        del_in_head(number / kBase);
      }
    }
    throw;
  }
  head_empty_ = false;
  tail_empty_ = false;
}

template <typename T, typename Allocator>
Deque<T, Allocator>::Deque(size_t count, const T& value,
                           const Allocator& alloc) {
  alloc_t_ = alloc;
  head_.reserve(2);
  tail_.reserve(2);
  count_tail_ = 0;
  int number = 0;
  try {
    head_.reserve(count);
    for (number = 0; number < static_cast<int>(count); ++number) {
      if (number % kBase == 0) {
        head_[number / kBase] = alloc_traits::allocate(alloc_t_, kBase);
      }
      alloc_traits::construct(alloc_t_,
                              head_[number / kBase] + (number % kBase), value);
    }
  } catch (...) {
    for (; number > 0; --number) {
      alloc_traits::destroy(alloc_t_,
                            (head_[number / kBase] + (number % kBase)));
      if (number % kBase == 0) {
        del_in_head(number / kBase);
      }
    }
    alloc_traits::destroy(alloc_t_, (head_[number / kBase] + (number % kBase)));
    del_in_head(number / kBase);
    throw;
  }
  count_head_ = static_cast<int>(number);
  head_empty_ = false;
  tail_empty_ = false;
}

template <typename T, typename Allocator>
Deque<T, Allocator>::Deque(Deque&& other) {
  alloc_t_ =
      alloc_traits::select_on_container_copy_construction(other.alloc_t_);
  head_ = std::move(other.head_);
  tail_ = std::move(other.tail_);
  count_head_ = other.count_head_;
  count_tail_ = other.count_tail_;
  head_empty_ = other.head_empty_;
  tail_empty_ = other.tail_empty_;
  other.count_head_ = 0;
  other.count_tail_ = 0;
  other.moved_ = true;
  other.head_empty_ = false;
  other.tail_empty_ = false;
}

template <typename T, typename Allocator>
Deque<T, Allocator>::Deque(std::initializer_list<T> init,
                           const Allocator& alloc) {
  Deque<T, Allocator> other(alloc);
  auto iter = init.begin();
  for (size_t size = 0; size < init.size(); ++size, ++iter) {
    other.push_back(*iter);
  }
  alloc_t_ =
      alloc_traits::select_on_container_copy_construction(other.alloc_t_);
  head_ = std::move(other.head_);
  tail_ = std::move(other.tail_);
  count_head_ = other.count_head_;
  count_tail_ = other.count_tail_;
  head_empty_ = other.head_empty_;
  tail_empty_ = other.tail_empty_;
  other.count_head_ = 0;
  other.count_tail_ = 0;
  other.moved_ = true;
  other.head_empty_ = false;
  other.tail_empty_ = false;
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::push_back(const T& value) {
  if (count_head_ >= 0 && !head_empty_) {
    push_back_in_head(value);
  } else if (count_head_ < 0) {
    try {
      if (count_head_ % kBase == 0) {
        tail_[-(count_head_ / kBase)] = alloc_traits::allocate(alloc_t_, kBase);
      }
      alloc_traits::construct(
          alloc_t_,
          tail_[-(count_head_ / kBase)] + (kBase - 1 + (count_head_ % kBase)),
          value);
      ++count_head_;
    } catch (...) {
      alloc_traits::destroy(alloc_t_, (tail_[-(count_head_ / kBase)] +
                                       (kBase - 1 + (count_head_ % kBase))));
      if (count_head_ % kBase == 0) {
        del_in_tail(-(count_head_ / kBase));
      }
      throw;
    }
  } else if (head_empty_) {
    alloc_traits::construct(
        alloc_t_,
        tail_[-(count_head_ / kBase)] + (kBase - 1 + (count_head_ % kBase)),
        value);
    head_empty_ = false;
  }
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::push_front(const T& value) {
  if (count_tail_ >= 0 && !tail_empty_) {
    push_front_in_tail(value);
  } else if (count_tail_ < 0) {  //
    try {
      if (count_tail_ % kBase == 0) {
        head_[-(count_tail_ / kBase)] = alloc_traits::allocate(alloc_t_, kBase);
      }
      alloc_traits::construct(
          alloc_t_,
          head_[-(count_tail_ / kBase)] + (kBase - 1 + (count_tail_ % kBase)),
          value);
      ++count_tail_;
    } catch (...) {
      alloc_traits::destroy(alloc_t_, (head_[-(count_tail_ / kBase)] +
                                       (kBase - 1 + (count_tail_ % kBase))));
      if (count_tail_ % kBase == 0) {
        del_in_head(-(count_tail_ / kBase));
      }
      throw;
    }
  } else if (tail_empty_) {
    alloc_traits::construct(
        alloc_t_,
        head_[-(count_tail_ / kBase)] + (kBase - 1 + (count_tail_ % kBase)),
        value);
    tail_empty_ = false;
  }
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::push_back(T&& value) {
  if (count_head_ >= 0 && !head_empty_) {
    push_back_in_head(std::move(value));
  } else if (count_head_ < 0) {
    try {
      if (count_head_ % kBase == 0) {
        tail_[-(count_head_ / kBase)] = alloc_traits::allocate(alloc_t_, kBase);
      }
      alloc_traits::construct(
          alloc_t_,
          tail_[-(count_head_ / kBase)] + (kBase - 1 + (count_head_ % kBase)),
          std::move(value));
      ++count_head_;
    } catch (...) {
      alloc_traits::destroy(alloc_t_, (tail_[-(count_head_ / kBase)] +
                                       (kBase - 1 + (count_head_ % kBase))));
      if (count_head_ % kBase == 0) {
        del_in_tail(-(count_head_ / kBase));
      }
      throw;
    }
  } else if (head_empty_) {
    alloc_traits::construct(
        alloc_t_,
        tail_[-(count_head_ / kBase)] + (kBase - 1 + (count_head_ % kBase)),
        std::move(value));
    head_empty_ = false;
  }
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::push_front(T&& value) {
  if (count_tail_ >= 0 && !tail_empty_) {
    push_front_in_tail(std::move(value));
  } else if (count_tail_ < 0) {  //
    try {
      if (count_tail_ % kBase == 0) {
        head_[-(count_tail_ / kBase)] = alloc_traits::allocate(alloc_t_, kBase);
      }
      alloc_traits::construct(
          alloc_t_,
          head_[-(count_tail_ / kBase)] + (kBase - 1 + (count_tail_ % kBase)),
          std::move(value));
      ++count_tail_;
    } catch (...) {
      alloc_traits::destroy(alloc_t_, (head_[-(count_tail_ / kBase)] +
                                       (kBase - 1 + (count_tail_ % kBase))));
      if (count_tail_ % kBase == 0) {
        del_in_head(-(count_tail_ / kBase));
      }
      throw;
    }
  } else if (tail_empty_) {
    alloc_traits::construct(
        alloc_t_,
        head_[-(count_tail_ / kBase)] + (kBase - 1 + (count_tail_ % kBase)),
        std::move(value));
    tail_empty_ = false;
  }
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::pop_back() {
  if (count_head_ > 0) {
    --count_head_;
    alloc_traits::destroy(alloc_t_,
                          (head_[count_head_ / kBase] + (count_head_ % kBase)));
    if (count_head_ % kBase == 0) {
      del_in_head(count_head_ / kBase);
    }
  } else if (count_head_ <= 0 && head_empty_) {
    --count_head_;
    alloc_traits::destroy(alloc_t_, (tail_[-(count_head_ / kBase)] +
                                     (kBase - 1 + (count_head_ % kBase))));
    if (count_head_ % kBase == 0) {
      del_in_tail((-(kBase + count_head_) / kBase));
    }
  } else if (!head_empty_) {
    head_empty_ = true;
    alloc_traits::destroy(alloc_t_, (tail_[-(count_head_ / kBase)] +
                                     (kBase - 1 + (count_head_ % kBase))));
  }
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::pop_front() {
  if (count_tail_ > 0) {
    --count_tail_;
    alloc_traits::destroy(alloc_t_,
                          (tail_[count_tail_ / kBase] + (count_tail_ % kBase)));
    if (count_tail_ % kBase == 0) {
      del_in_tail(count_tail_ / kBase);
    }
  } else if (count_tail_ <= 0 && tail_empty_) {
    --count_tail_;
    alloc_traits::destroy(alloc_t_, (head_[(-count_tail_ / kBase)] +
                                     (kBase - 1 + (count_tail_ % kBase))));
    if (count_tail_ % kBase == 0) {
      del_in_head((-(kBase + count_tail_) / kBase));
    }
  } else if (!tail_empty_) {
    tail_empty_ = true;
    alloc_traits::destroy(alloc_t_, (head_[(-count_tail_ / kBase)] +
                                     (kBase - 1 + (count_tail_ % kBase))));
  }
}

template <typename T, typename Allocator>
Deque<T, Allocator>& Deque<T, Allocator>::operator=(
    const Deque<T, Allocator>& other) {
  try {
    Deque<T, Allocator> tmp(other);
    clear();
    operator=(std::move(tmp));
    if (alloc_traits::propagate_on_container_copy_assignment::value) {
      alloc_t_ = other.alloc_t_;
    }
    return *this;
  } catch (...) {
    throw;
  }
}

template <typename T, typename Allocator>
Deque<T, Allocator>& Deque<T, Allocator>::operator=(Deque&& other) {
  if (this == &other) {
    return *this;
  }
  clear();
  alloc_t_ =
      alloc_traits::select_on_container_copy_construction(other.alloc_t_);
  head_ = std::move(other.head_);
  tail_ = std::move(other.tail_);
  count_head_ = other.count_head_;
  count_tail_ = other.count_tail_;
  head_empty_ = other.head_empty_;
  tail_empty_ = other.tail_empty_;
  other.moved_ = true;
  other.count_head_ = 0;
  other.count_tail_ = 0;
  other.head_empty_ = 1;
  other.tail_empty_ = 1;
  return *this;
}

template <typename T, typename Allocator>
size_t Deque<T, Allocator>::size() const {
  if (!tail_empty_ && !head_empty_) {
    return static_cast<size_t>(count_head_ + count_tail_);
  }
  return static_cast<size_t>(count_head_ + count_tail_ - 1);
}

template <typename T, typename Allocator>
bool Deque<T, Allocator>::empty() const {
  if (!tail_empty_ && !head_empty_) {
    return (count_head_ + count_tail_) == 0;
  }
  return (count_head_ + count_tail_ - 1) == 0;
}

template <typename T, typename Allocator>
const T& Deque<T, Allocator>::operator[](int number_el) const {
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

template <typename T, typename Allocator>
T& Deque<T, Allocator>::operator[](int number_el) {
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

template <typename T, typename Allocator>
const T& Deque<T, Allocator>::at(size_t number_el) const {
  if (size() <= number_el) {
    throw std::out_of_range("");
  }
  return operator[](number_el);
}

template <typename T, typename Allocator>
T& Deque<T, Allocator>::at(size_t number_el) {
  if (size() <= number_el) {
    throw std::out_of_range("");
  }
  return operator[](number_el);
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::insert(iterator iter, const T& value) {
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

template <typename T, typename Allocator>
void Deque<T, Allocator>::erase(iterator iter) {
  iterator end_it = end() - 1;
  while (end_it > iter) {
    *(iter) = *(iter + 1);
    ++iter;
  }
  pop_back();
}

template <typename T, typename Allocator>
Deque<T, Allocator>::~Deque() {
  if (!moved_) {
    clear();
  }
}

template <typename T, typename Allocator>
template <bool IsConst, bool IsRevers>
class Deque<T, Allocator>::CommonIterator
    : public std::iterator<std::random_access_iterator_tag, T> {
 private:
  std::conditional_t<IsConst, const Deque<T, Allocator>*, Deque<T, Allocator>*>
      deq_;
  int number_el_;

 public:
  using value_type = typename std::conditional<IsConst, const T, T>::type;
  using pointer = typename std::conditional<IsConst, const T*, T*>::type;
  using reference = typename std::conditional<IsConst, const T&, T&>::type;
  using iterator_category = std::random_access_iterator_tag;
  using difference_type = int;

  CommonIterator();

  CommonIterator(int number, Deque<T, Allocator>* deq);

  CommonIterator(const CommonIterator<IsConst, false>& other);
  CommonIterator(const CommonIterator<IsConst, true>& other);

  // CommonIterator(const CommonReverseIterator<IsConst>& other);

  std::conditional_t<IsConst, const Deque<T, Allocator>*, Deque<T, Allocator>*>
  getdeq() const {
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

template <typename T, typename Allocator>
template <bool IsConst, bool IsRevers>
Deque<T, Allocator>::CommonIterator<IsConst, IsRevers>::CommonIterator(
    int number, Deque<T, Allocator>* deq) {
  number_el_ = number;
  deq_ = deq;
}

template <typename T, typename Allocator>
template <bool IsConst, bool IsRevers>
Deque<T, Allocator>::CommonIterator<IsConst, IsRevers>::CommonIterator() {
  number_el_ = 0;
}

template <typename T, typename Allocator>
template <bool IsConst, bool IsRevers>
Deque<T, Allocator>::CommonIterator<IsConst, IsRevers>::CommonIterator(
    const CommonIterator<IsConst, false>& other) {
  deq_ = other.getdeq();
  number_el_ = other.getnum();
}

template <typename T, typename Allocator>
template <bool IsConst, bool IsRevers>
Deque<T, Allocator>::CommonIterator<IsConst, IsRevers>::CommonIterator(
    const CommonIterator<IsConst, true>& other) {
  deq_ = other.getdeq();
  number_el_ = other.getnum();
}
