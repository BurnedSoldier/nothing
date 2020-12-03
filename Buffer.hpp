#pragma once

#include <stdlib.h>
#include <string.h>


class RingBuffer {
public:
  RingBuffer() {}
  ~RingBuffer() { delete[] mgs_; }

  void push(const char *newMgs, size_t size) {
    size_t used = end_ - start_;
    size_t available = cap_ - used;
    if (available < size) { 
      if (start_ > size) { 
        memmove(mgs_, mgs_ + start_, used);
        start_ = 0;
        memcpy(mgs_ + used, newMgs, size);
        end_ = used + size;
      } else {
        cap_ = cap_ * 2 + size;
        char *new_msg = (char *)malloc(cap_);
        if (used > 0) {
          memcpy(new_msg, mgs_ + start_, used);
        }
        memcpy(new_msg + used, newMgs, size);
        if (mgs_) { 
          free(mgs_);
        }
        mgs_ = new_msg;
        start_ = 0;
        end_ = size + used;
      }
    } else {
      memcpy(mgs_ + end_, newMgs, size);
      end_ += size;
    }
  }

  char *readableStart() { return mgs_ + start_; }
  size_t readableBytes() const { return end_ - start_; }
  size_t availableByte() { return cap_ - end_; }

  void releaseBytes(size_t size) {
    if (mgs_ + start_) {
      memset(mgs_ + start_, 0, size);
    }
    start_ += size;
    if (start_ == end_) {
      start_ = end_ = 0;
    }
  }

  char *getMgs(size_t *len) {
    char *mgs = mgs_ + start_;
    if (len) {
      *len = end_ - start_;
    }
    if (end_ == start_) {
      mgs = NULL;
    }
    return mgs;
  }

private:
  size_t start_;
  size_t end_;
  size_t cap_;

  char *mgs_;
};

