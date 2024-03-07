#pragma once

#include <cstddef>
#include <vector>

class RingBuffer {
 private:
  std::vector<int> buf_;
  size_t count_ = 0;
  size_t size_;

 public:
  explicit RingBuffer(size_t capacity) {
    buf_.reserve(capacity);
    size_ = capacity;
  }

  size_t Size() const { return count_; }

  bool Empty() const { return count_ == 0; }

  bool TryPush(int element) {
    if (size_ == count_) {
      return false;
    }
    buf_.push_back(element);
    ++count_;
    return true;
  }

  bool TryPop(int* element) {
    if (count_ == 0) {
      return false;
    }
    *element = buf_[0];
    for (size_t i = 0; i < count_ - 1; i++) {
      buf_[i] = buf_[i + 1];
    }
    buf_.pop_back();
    --count_;
    return true;
  }
};
