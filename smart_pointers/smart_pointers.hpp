#pragma once
#include <cstddef>
#include <iostream>
#include <memory>
#include <utility>

template <typename X, typename Y>
using is_familiar =
    std::enable_if_t<std::is_same<X, Y>::value || std::is_base_of<X, Y>::value>;

struct BaseControlBlock {
  size_t shared_count;
  size_t weak_count;

  BaseControlBlock(size_t n_shared_count, size_t n_weak_count) noexcept
      : shared_count(n_shared_count), weak_count(n_weak_count) {}

  virtual void del_ptr(){};

  virtual void del_me(){};

  virtual ~BaseControlBlock() = default;
};

template <typename T, typename Deleter = std::default_delete<T>,
          typename Allocator = std::allocator<T>>
struct ControlBlock : BaseControlBlock {
  T* ptr = nullptr;
  using alloc_traits = std::allocator_traits<Allocator>;
  Allocator alloc;
  Deleter del;

  ControlBlock() {
    shared_count = 1;
    weak_count = 0;
    alloc = Allocator();
    del = Deleter();
  }

  ControlBlock(T* n_ptr, size_t n_shared_count, size_t n_weak_count,
               Allocator n_alloc = Allocator(), Deleter n_del = Deleter())
      : BaseControlBlock(n_shared_count, n_weak_count),
        ptr(n_ptr),
        alloc(n_alloc),
        del(n_del) {}

  ControlBlock(T* n_ptr, size_t n_shared_count, size_t n_weak_count,
               Deleter n_del, Allocator n_alloc = Allocator())
      : BaseControlBlock(n_shared_count, n_weak_count),
        ptr(n_ptr),
        alloc(n_alloc),
        del(n_del) {}

  template <typename... Args>
  ControlBlock(size_t n_shared_count, size_t n_weak_count, Args&&... args)
      : BaseControlBlock(n_shared_count, n_weak_count) {
    ptr = new T(std::forward<Args>(args)...);
    alloc = Allocator();
    del = Deleter();
  }

  template <typename... Args>
  ControlBlock(size_t n_shared_count, size_t n_weak_count, Allocator n_alloc,
               Args&&... args)
      : BaseControlBlock(n_shared_count, n_weak_count) {
    ptr = new T(std::forward<Args>(args)...);
    alloc = n_alloc;
    del = Deleter();
  }

  void del_ptr() {
    if (ptr != nullptr) {
      del(ptr);
    }
    ptr = nullptr;
  }

  void del_me() {
    using alloc_cb = typename std::allocator_traits<
        Allocator>::template rebind_alloc<ControlBlock<T, Deleter, Allocator>>;
    alloc_cb allocator = alloc;
    using alloc_traits_cb = std::allocator_traits<alloc_cb>;
    alloc_traits_cb::destroy(allocator, this);
    typename std::allocator_traits<Allocator>::template rebind_alloc<
        ControlBlock>(alloc)
        .deallocate(this, 1);
    alloc.~Allocator();
  }

  ~ControlBlock() {
    if (ptr != nullptr) {
      del(ptr);
    }
  }
};

template <typename T>
class WeakPtr;

template <typename T>
class SharedPtr {
 private:
  BaseControlBlock* cptr_ = nullptr;
  T* ptr_ = nullptr;
  friend WeakPtr<T>;

  template <class Y>
  friend class SharedPtr;

  template <typename Y = T, typename Deleter = std::default_delete<Y>,
            typename Alloc = std::allocator<Y>, typename = is_familiar<T, Y>>
  BaseControlBlock* create_cptr(Y* ptr, size_t shered_count, size_t weak_count,
                                const Deleter& deleter = Deleter(),
                                const Alloc& alloc = Alloc()) {
    using alloc_cb = typename std::allocator_traits<
        Alloc>::template rebind_alloc<ControlBlock<Y, Deleter, Alloc>>;
    alloc_cb allocator = alloc;
    using alloc_traits = std::allocator_traits<alloc_cb>;
    auto result = alloc_traits::allocate(allocator, 1);
    alloc_traits::construct(allocator, result, ptr, shered_count, weak_count,
                            deleter, alloc);
    return result;
  }

  template <typename Deleter = std::default_delete<T>,
            typename Alloc = std::allocator<T>>
  BaseControlBlock* create_cptr(std::nullptr_t ptr, size_t shered_count,
                                size_t weak_count,
                                const Deleter& deleter = Deleter(),
                                const Alloc& alloc = Alloc()) {
    using alloc_cb = typename std::allocator_traits<
        Alloc>::template rebind_alloc<ControlBlock<T, Deleter, Alloc>>;
    alloc_cb allocator = alloc;
    using alloc_traits = std::allocator_traits<alloc_cb>;
    auto result = alloc_traits::allocate(allocator, 1);
    alloc_traits::construct(allocator, result, ptr, shered_count, weak_count,
                            deleter, alloc);
    return result;
  }

 public:
  SharedPtr() { cptr_ = create_cptr(nullptr, 0, 0); }

  constexpr SharedPtr(std::nullptr_t ptr) noexcept {
    cptr_ = create_cptr(ptr, 0, 0);
  }

  SharedPtr(T* ptr) {
    cptr_ = create_cptr(ptr, 1, 0);
    ptr_ = ptr;
  }

  template <typename Y, typename = is_familiar<T, Y>>
  explicit SharedPtr(Y* ptr) {
    cptr_ = create_cptr(ptr, 1, 0);
    ptr_ = ptr;
  }

  SharedPtr(const SharedPtr& other) {
    cptr_ = other.cptr_;
    ++(cptr_->shared_count);
    ptr_ = other.ptr_;
  }

  SharedPtr(SharedPtr&& other) {
    cptr_ = std::move(other.cptr_);
    other.cptr_ = nullptr;
    ptr_ = std::move(other.ptr_);
    other.ptr_ = nullptr;
  }

  template <typename Y, typename Deleter, typename = is_familiar<T, Y>>
  SharedPtr(Y* ptr, Deleter del) {
    cptr_ = create_cptr(ptr, 1, 0, del);
    ptr_ = ptr;
  }

  template <typename Y, typename Deleter, typename Alloc,
            typename = is_familiar<T, Y>>
  SharedPtr(Y* ptr, Deleter del, Alloc alloc) {
    cptr_ = create_cptr(ptr, 1, 0, del, alloc);
    ptr_ = ptr;
  }

  template <typename Y, typename = is_familiar<T, Y>>
  SharedPtr(const SharedPtr<Y>& other) {
    cptr_ = other.cptr_;
    ++(cptr_->shared_count);
    ptr_ = other.ptr_;
  }

  template <typename Y, typename = is_familiar<T, Y>>
  SharedPtr(SharedPtr<Y>&& other) {
    cptr_ = std::move(other.cptr_);
    other.cptr_ = nullptr;
    ptr_ = std::move(other.ptr_);
  }

  explicit SharedPtr(BaseControlBlock* cptr) {
    cptr_ = reinterpret_cast<ControlBlock<T>*>(cptr);
    ++(cptr_->shared_count);
  }

  ~SharedPtr() {
    if (cptr_ == nullptr) {
      return;
    }
    if (cptr_->shared_count == 0 && cptr_->weak_count == 0) {
      cptr_->del_me();
      cptr_ = nullptr;
      return;
    }
    --(cptr_->shared_count);
    if (cptr_->shared_count == 0) {
      cptr_->del_ptr();
      if (cptr_->weak_count == 0) {
        cptr_->del_me();
        cptr_ = nullptr;
      }
    }
  }

  int use_count() const {
    if (cptr_ == nullptr) {
      return 0;
    }
    return cptr_->shared_count;
  }

  T* get() const { return ptr_; }

  template <class Y>
  void reset(Y* ptr) {
    if (cptr_ == nullptr) {
      cptr_ = create_cptr(ptr, 1, 0);
      return;
    }
    if (cptr_->shared_count == 0 && cptr_->weak_count == 0) {
      cptr_->del_me();
      cptr_ = create_cptr(ptr, 1, 0);
      return;
    }
    --(cptr_->shared_count);
    if (cptr_->shared_count == 0) {
      cptr_->del_ptr();
      if (cptr_->weak_count == 0) {
        cptr_->del_me();
      }
    }
    cptr_ = create_cptr(ptr, 1, 0);
  }

  template <typename Y, typename Deleter>
  void reset(Y* ptr, Deleter del) {
    if (cptr_ == nullptr) {
      cptr_ = create_cptr(ptr, 1, 0, del);
      return;
    }
    if (cptr_->shared_count == 0 && cptr_->weak_count == 0) {
      cptr_->del_me();
      cptr_ = create_cptr(ptr, 1, 0, del);
      return;
    }
    --(cptr_->shared_count);
    if (cptr_->shared_count == 0) {
      cptr_->del_ptr();
      if (cptr_->weak_count == 0) {
        cptr_->del_me();
      }
    }
    cptr_ = create_cptr(ptr, 1, 0, del);
  }

  template <typename Y, typename Deleter, typename Alloc>
  void reset(Y* ptr, Deleter del, Alloc alloc) {
    if (cptr_ == nullptr) {
      cptr_ = create_cptr(ptr, 1, 0, del, alloc);
      return;
    }
    if (cptr_->shared_count == 0 && cptr_->weak_count == 0) {
      cptr_->del_me();
      cptr_ = create_cptr(ptr, 1, 0, del, alloc);
      return;
    }
    --(cptr_->shared_count);
    if (cptr_->shared_count == 0) {
      cptr_->del_ptr();
      if (cptr_->weak_count == 0) {
        cptr_->del_me();
      }
    }
    cptr_ = create_cptr(ptr, 1, 0, del, alloc);
  }

  void reset() {
    if (cptr_ == nullptr) {
      return;
    }
    if (cptr_->shared_count == 0 && cptr_->weak_count == 0) {
      cptr_->del_me();
      cptr_ = nullptr;
      return;
    }
    --(cptr_->shared_count);
    if (cptr_->shared_count == 0) {
      cptr_->del_ptr();
      if (cptr_->weak_count == 0) {
        cptr_->del_me();
      }
    }
    cptr_ = nullptr;
  }

  SharedPtr<T>& operator=(const SharedPtr& other) {
    if (&other == this) {
      return *this;
    }
    reset();
    cptr_ = other.cptr_;
    ++(cptr_->shared_count);
    ptr_ = other.ptr_;
    return *this;
  }

  SharedPtr<T>& operator=(SharedPtr&& other) {
    if ((cptr_->shared_count == 0 || cptr_->shared_count == 1) &&
        cptr_->weak_count == 0) {
      cptr_->del_me();
      cptr_ = nullptr;
    } else {
      --(cptr_->shared_count);
    }
    cptr_ = std::move(other.cptr_);
    other.cptr_ = nullptr;
    ptr_ = std::move(other.ptr_);
    other.ptr_ = nullptr;
    return *this;
  }

  template <typename Y, typename = is_familiar<T, Y>>
  SharedPtr& operator=(const SharedPtr<Y>& other) {
    reset();
    cptr_ = other.cptr_;
    ++(cptr_->shared_count);
    ptr_ = other.ptr_;
    return *this;
  }

  template <typename Y, typename = is_familiar<T, Y>>
  SharedPtr& operator=(SharedPtr<Y>&& other) {
    if ((cptr_->shared_count == 0 || cptr_->shared_count == 1) &&
        cptr_->weak_count == 0) {
      cptr_->del_me();
      cptr_ = nullptr;
    } else {
      --(cptr_->shared_count);
    }
    cptr_ = std::move(other.cptr_);
    other.cptr_ = nullptr;
    ptr_ = std::move(other.ptr_);
    other.ptr_ = nullptr;
    return *this;
  }

  T& operator*() { return *ptr_; }

  const T& operator*() const { return *ptr_; }

  T* operator->() { return ptr_; }
};

template <typename T>
class WeakPtr {
 private:
  BaseControlBlock* cptr_ = nullptr;

 public:
  WeakPtr() = default;

  WeakPtr(const WeakPtr& other) { cptr_ = other.cptr_; }

  WeakPtr(const SharedPtr<T>& other) {
    cptr_ = other.cptr_;
    ++(cptr_->weak_count);
  }

  WeakPtr(WeakPtr&& other) {
    cptr_ = std::move(other.cptr_);
    other.cptr_ = nullptr;
  }

  WeakPtr<T>& operator=(const WeakPtr& other) {
    if (cptr_ != nullptr) {
      --(cptr_->weak_count);
      if (cptr_->shared_count == 0) {
        if (cptr_->weak_count == 0) {
          cptr_->del_me();
          cptr_ = nullptr;
        }
      }
    }
    cptr_ = other.cptr_;
    ++(cptr_->weak_count);
  }

  WeakPtr<T>& operator=(const WeakPtr&& other) {
    --(cptr_->weak_count);
    cptr_ = std::move(other.cptr_);
    other.cptr_ = nullptr;
  }

  ~WeakPtr() {
    if (cptr_ == nullptr) {
      return;
    }
    if (cptr_->shared_count == 0 && cptr_->weak_count == 0) {
      cptr_->del_me();
      cptr_ = nullptr;
      return;
    }
    --(cptr_->weak_count);
    if (cptr_->shared_count == 0) {
      if (cptr_->weak_count == 0) {
        cptr_->del_me();
        cptr_ = nullptr;
      }
    }
  }

  bool expired() const {
    if (cptr_ == nullptr) {
      return true;
    }
    return (cptr_->shared_count == 0);
  }

  SharedPtr<T> lock() const { return SharedPtr<T>(cptr_); }
};

template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
  auto cptr = new ControlBlock<T>(0, 0, std::forward<Args>(args)...);
  return SharedPtr<T>(cptr);
}

template <typename T, typename Alloc, typename... Args>
SharedPtr<T> AllocateShared(const Alloc& alloc, Args&&... args) {
  using alloc_cb = typename std::allocator_traits<Alloc>::template rebind_alloc<
      ControlBlock<T, std::default_delete<T>, Alloc>>;
  alloc_cb allocator = alloc;
  using alloc_traits = std::allocator_traits<alloc_cb>;
  auto result = alloc_traits::allocate(allocator, 1);
  alloc_traits::construct(allocator, result, 0, 0, alloc,
                          std::forward<Args>(args)...);
  return SharedPtr<T>(result);
}
