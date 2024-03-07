#include <cstddef>
#include <iostream>
#include <iterator>
#include <memory>

template <typename T, typename Allocator = std::allocator<T>>
class List {
 private:
  using alloc_traits = std::allocator_traits<Allocator>;
  Allocator alloc_t_;
  struct Node;
  template <bool IsConst, bool IsReverse>
  class CommonIterator;
  using alloc_for_node = typename std::allocator_traits<
      std::allocator<T>>::template rebind_alloc<Node>;
  using node_alloc_traits = std::allocator_traits<alloc_for_node>;
  alloc_for_node alloc_;
  Node* first_;
  size_t size_;
  void push_node();
  void clear();

 public:
  using iterator = CommonIterator<false, false>;
  using const_iterator = CommonIterator<true, false>;
  using reverse_iterator = CommonIterator<false, true>;
  using const_reverse_iterator = CommonIterator<true, true>;
  using value_type = T;
  using allocator_type = Allocator;
  List();
  List(size_t count, const T& value, const Allocator& alloc = Allocator());
  explicit List(size_t count, const Allocator& alloc = Allocator());
  List(const List& other);
  List(std::initializer_list<T> other, const Allocator& alloc = Allocator());
  List& operator=(const List& other);
  ~List();
  bool empty() const;
  size_t size() const;
  void push_back(const T& value);
  void push_front(const T& value);
  void push_back(T&& value);
  void push_front(T&& value);
  void pop_back();
  void pop_front();
  T& front();
  const T& front() const;
  T& back();
  const T& back() const;
  Allocator get_allocator() const;
  iterator begin() const {
    iterator tmp(first_->next);
    return tmp;
  }
  iterator end() const {
    iterator tmp(first_);
    return tmp;
  }
  const_iterator cbegin() const {
    const_iterator tmp(first_->next);
    return tmp;
  }
  const_iterator cend() const {
    const_iterator tmp(first_);
    return tmp;
  }
  reverse_iterator rbegin() const {
    reverse_iterator tmp(first_->prev);
    return tmp;
  }
  reverse_iterator rend() const {
    reverse_iterator tmp(first_);
    return tmp;
  }
  const_reverse_iterator crbegin() const {
    const_reverse_iterator tmp(first_->prev);
    return tmp;
  }
  const_reverse_iterator crend() const {
    const_reverse_iterator tmp(first_);
    return tmp;
  }
};

template <typename T, typename Allocator>
struct List<T, Allocator>::Node {
  using allocator_traits = std::allocator_traits<Allocator>;
  Node* next;
  Node* prev;
  Allocator alloc = Allocator();
  T* value;
  Node(const Allocator& allocator = Allocator()) {
    try {
      alloc = allocator;
      value = allocator_traits::allocate(alloc, 1);
      allocator_traits::construct(alloc, value);
    } catch (...) {
      allocator_traits::deallocate(alloc, value, 1);
      throw;
    }
  }

  Node(const T& val, const Allocator& allocator = Allocator()) {
    try {
      alloc = allocator;
      value = allocator_traits::allocate(alloc, 1);
      allocator_traits::construct(alloc, value, val);
    } catch (...) {
      allocator_traits::deallocate(alloc, value, 1);
      throw;
    }
  }

  Node(Node& other, const Allocator& allocator = Allocator()) {
    try {
      alloc = allocator;
      value = allocator_traits::allocate(alloc, 1);
      allocator_traits::construct(alloc, value, other.value);
    } catch (...) {
      allocator_traits::deallocate(alloc, value, 1);
      throw;
    }
    //не уверен зачем
  }

  ~Node() {
    allocator_traits::destroy(alloc, value);
    allocator_traits::deallocate(alloc, value, 1);
  }
};

template <typename T, typename Allocator>
template <bool IsConst, bool IsReverse>
class List<T, Allocator>::CommonIterator
    : public std::iterator<std::bidirectional_iterator_tag, T> {
 private:
  Node* node_;

 public:
  using value_type = typename std::conditional<IsConst, const T, T>::type;
  using pointer = typename std::conditional<IsConst, const T*, T*>::type;
  using reference = typename std::conditional<IsConst, const T&, T&>::type;
  using iterator_category = std::bidirectional_iterator_tag;
  CommonIterator() = default;
  CommonIterator(Node* node) { node_ = node; }
  CommonIterator(const CommonIterator& other) { node_ = other.node_; }
  CommonIterator<IsConst, IsReverse>& operator=(
      const CommonIterator<IsConst, IsReverse>& other) {
    node_ = other.node_;
  }
  CommonIterator<IsConst, IsReverse>& operator++() {
    if (IsReverse) {
      node_ = node_->prev;
    } else {
      node_ = node_->next;
    }
    return *this;
  }
  CommonIterator<IsConst, IsReverse> operator++(int) {
    CommonIterator<IsConst, IsReverse> copy = *this;
    if (IsReverse) {
      node_ = node_->prev;
    } else {
      node_ = node_->next;
    }
    return copy;
  }
  CommonIterator<IsConst, IsReverse>& operator--() {
    if (!IsReverse) {
      node_ = node_->prev;
    } else {
      node_ = node_->next;
    }
    return *this;
  }
  CommonIterator<IsConst, IsReverse> operator--(int) {
    CommonIterator<IsConst, IsReverse> copy = *this;
    if (!IsReverse) {
      node_ = node_->prev;
    } else {
      node_ = node_->next;
    }
    return copy;
  }
  bool operator==(const CommonIterator<IsConst, IsReverse>& other) const {
    return node_ == other.node_;
  }
  bool operator!=(const CommonIterator<IsConst, IsReverse>& other) const {
    return node_ != other.node_;
  }
  std::conditional_t<IsConst, const T&, T&> operator*() {
    return *(node_->value);
  }

  std::conditional_t<IsConst, const T*, T*> operator->() {
    return (node_->value);
  }
};

template <typename T, typename Allocator>
void List<T, Allocator>::push_node() {
  Node* tmp;
  try {
    tmp = node_alloc_traits::allocate(alloc_, 1);
    node_alloc_traits::construct(alloc_, tmp, alloc_t_);
    tmp->prev = first_->prev;
    (first_->prev)->next = tmp;
    first_->prev = tmp;
    tmp->next = first_;
    ++size_;
  } catch (...) {
    node_alloc_traits::deallocate(alloc_, tmp, 1);
    throw;
  }
}

template <typename T, typename Allocator>
void List<T, Allocator>::clear() {
  while (size_ > 0) {
    pop_back();
  }
  node_alloc_traits::deallocate(alloc_, first_, 1);
}

template <typename T, typename Allocator>
List<T, Allocator>::List() {
  try {
    alloc_ = alloc_for_node();
    size_ = 0;
    first_ = node_alloc_traits::allocate(alloc_, 1);
    first_->next = first_;
    first_->prev = first_;
  } catch (...) {
    clear();
    throw;
  }
}

template <typename T, typename Allocator>
Allocator List<T, Allocator>::get_allocator() const {
  return alloc_t_;
}

template <typename T, typename Allocator>
List<T, Allocator>::List(size_t count, const T& value, const Allocator& alloc) {
  try {
    alloc_t_ = alloc;
    alloc_ = alloc_for_node();
    first_ = node_alloc_traits::allocate(alloc_, 1);
    first_->next = first_;
    first_->prev = first_;
    size_ = 0;
    for (size_t i = 0; i < count; ++i) {
      push_back(value);
    }
  } catch (...) {
    clear();
    throw;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::List(size_t count, const Allocator& alloc) {
  try {
    alloc_t_ = alloc;
    alloc_ = alloc_for_node();
    first_ = node_alloc_traits::allocate(alloc_, 1);
    first_->next = first_;
    first_->prev = first_;
    size_ = 0;
    for (size_t i = 0; i < count; ++i) {
      push_node();
    }
  } catch (...) {
    clear();
    throw;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::List(const List& other) {
  try {
    alloc_t_ =
        alloc_traits::select_on_container_copy_construction(other.alloc_t_);
    auto iter = other.cbegin();
    alloc_ = alloc_for_node();
    first_ = node_alloc_traits::allocate(alloc_, 1);
    first_->next = first_;
    first_->prev = first_;
    size_ = 0;
    for (size_ = 0; size_ < other.size_; ++iter) {
      push_back(*iter);
    }
  } catch (...) {
    clear();
    throw;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::List(std::initializer_list<T> other,
                         const Allocator& alloc) {
  try {
    alloc_t_ = alloc;
    auto iter = other.begin();
    alloc_ = alloc_for_node();
    first_ = node_alloc_traits::allocate(alloc_, 1);
    first_->next = first_;
    first_->prev = first_;
    size_ = 0;
    for (size_ = 0; size_ < other.size(); ++iter) {
      push_back(*iter);
    }
  } catch (...) {
    clear();
    throw;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>& List<T, Allocator>::operator=(const List& other) {
  try {
    if (&other != this) {
      List copy = other;
      Node* tmp1 = first_;
      size_t tmp2 = size_;
      first_ = copy.first_;
      size_ = copy.size_;
      copy.first_ = tmp1;
      copy.size_ = tmp2;
      if (alloc_traits::propagate_on_container_copy_assignment::value) {
        alloc_t_ = other.alloc_t_;
      }
    }
  } catch (...) {
    throw;
  }
  return *this;
}

template <typename T, typename Allocator>
List<T, Allocator>::~List() {
  clear();
};

template <typename T, typename Allocator>
bool List<T, Allocator>::empty() const {
  return size_ == 0;
}

template <typename T, typename Allocator>
size_t List<T, Allocator>::size() const {
  return size_;
}

template <typename T, typename Allocator>
void List<T, Allocator>::push_back(const T& value) {
  Node* tmp;
  try {
    tmp = node_alloc_traits::allocate(alloc_, 1);
    node_alloc_traits::construct(alloc_, tmp, value, alloc_t_);
    tmp->prev = first_->prev;
    (first_->prev)->next = tmp;
    first_->prev = tmp;
    tmp->next = first_;
    ++size_;
  } catch (...) {
    node_alloc_traits::deallocate(alloc_, tmp, 1);
    throw;
  }
}

template <typename T, typename Allocator>
void List<T, Allocator>::push_front(const T& value) {
  Node* tmp;
  try {
    tmp = node_alloc_traits::allocate(alloc_, 1);
    node_alloc_traits::construct(alloc_, tmp, value, alloc_t_);
    tmp->next = first_->next;
    (first_->next)->prev = tmp;
    first_->next = tmp;
    tmp->prev = first_;
    ++size_;
  } catch (...) {
    node_alloc_traits::deallocate(alloc_, tmp, 1);
    throw;
  }
}

template <typename T, typename Allocator>
void List<T, Allocator>::push_back(T&& value) {
  Node* tmp;
  try {
    tmp = node_alloc_traits::allocate(alloc_, 1);
    node_alloc_traits::construct(alloc_, tmp, value, alloc_t_);
    tmp->prev = first_->prev;
    (first_->prev)->next = tmp;
    first_->prev = tmp;
    tmp->next = first_;
    ++size_;
  } catch (...) {
    node_alloc_traits::deallocate(alloc_, tmp, 1);
    throw;
  }
}

template <typename T, typename Allocator>
void List<T, Allocator>::push_front(T&& value) {
  Node* tmp;
  try {
    tmp = node_alloc_traits::allocate(alloc_, 1);
    node_alloc_traits::construct(alloc_, tmp, value, alloc_t_);
    tmp->next = first_->next;
    (first_->next)->prev = tmp;
    first_->next = tmp;
    tmp->prev = first_;
    ++size_;
  } catch (...) {
    node_alloc_traits::deallocate(alloc_, tmp, 1);
    throw;
  }
}

template <typename T, typename Allocator>
void List<T, Allocator>::pop_back() {
  Node* tmp = first_->prev;
  (tmp->prev)->next = first_;
  first_->prev = tmp->prev;
  // node_alloc_traits::destroy(alloc_, tmp); //в тесте
  // ASSERT_TRUE(MemoryManager::allocator_destroyed == kSize); но аллокатор
  // вызывается и для ноды и для Т() следовательно получаем 2*kSize ==> выхода
  // два: либо писать аллокатор для ноды, либо вызывать деструктор явно
  // tmp->~Node();  //но так как все равно остается проблема с задвоением
  // construct было принято решение использовать std::allocator, поэтому
  // вернем
  node_alloc_traits::destroy(alloc_, tmp);
  node_alloc_traits::deallocate(alloc_, tmp, 1);
  --size_;
}

template <typename T, typename Allocator>
void List<T, Allocator>::pop_front() {
  Node* tmp = first_->next;
  first_->next = tmp->next;
  (tmp->next)->prev = first_;
  node_alloc_traits::destroy(alloc_, tmp);
  node_alloc_traits::deallocate(alloc_, tmp, 1);
  --size_;
}

template <typename T, typename Allocator>
T& List<T, Allocator>::front() {
  return *((first_->next)->value);
}

template <typename T, typename Allocator>
const T& List<T, Allocator>::front() const {
  return *((first_->next)->value);
}

template <typename T, typename Allocator>
T& List<T, Allocator>::back() {
  return *((first_->prev)->value);
}

template <typename T, typename Allocator>
const T& List<T, Allocator>::back() const {
  return *((first_->prev)->value);
}
