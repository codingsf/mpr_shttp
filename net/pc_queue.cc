#include "net/pc_queue.h"
#include <vector>

namespace net {

void PCQueue::Enqueue(void* item) {
  std::unique_lock<std::mutex> lock(mutex_);
  queue_.push(item);

  cond_.notify_one();
}

void PCQueue::EnqueueN(void* item, int ntimes) {
  std::unique_lock<std::mutex> lock(mutex_);

  for (int i = 0; i < ntimes; ++i) {
    queue_.push(item);
  }

  cond_.notify_one();
}

void* PCQueue::Dequeue() {
  std::unique_lock<std::mutex> lock(mutex_);

  while (queue_.empty()) {
    cond_.wait(lock);
  }

  void* item = queue_.front();
  queue_.pop();
  return item;
}

void* PCQueue::TryDequeue(bool& dequeued) {
  std::unique_lock<std::mutex> lock(mutex_);
  
  if (queue_.empty()) {
    dequeued = false;
    return nullptr;
  }
  void* item = queue_.front();
  queue_.pop();
  dequeued = true;
  return item;
}

uint32_t PCQueue::TryDequeueN(uint32_t n, std::vector<void*>& retval) {
  std::unique_lock<std::mutex> lock(mutex_);
  
  uint32_t dequeued = 0;
  while (!queue_.empty() && dequeued < n) {
    void* item = queue_.front();
    queue_.pop();
    retval.push_back(item);
    dequeued++;
  }
  return dequeued;
}

int PCQueue::size() {
  std::unique_lock<std::mutex> lock(mutex_);
  size_t s = queue_.size();
  return s;
}

} // namespace net
