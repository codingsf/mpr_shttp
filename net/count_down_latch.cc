#include "net/count_down_latch.h"

namespace net {

CountDownLatch::CountDownLatch(uint32_t count)
    : count_(count) {}

CountDownLatch::~CountDownLatch() {}

void CountDownLatch::Wait() {
  std::unique_lock<std::mutex> lock(mutex_);

  while (count_ > 0) {
    cond_.wait(lock);
  }
}

void CountDownLatch::CountDown() {
  std::unique_lock<std::mutex> lock(mutex_);

  if (count_ == 0) {
    return;
  }

  if (--count_ == 0) {
    cond_.notify_all();
  }
}

uint32_t CountDownLatch::count() {
  std::unique_lock<std::mutex> lock(mutex_);

  return count_;
}

} // namespace net
