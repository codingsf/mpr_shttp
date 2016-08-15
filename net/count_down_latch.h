#ifndef NET_COUNT_DOWN_LATCH_H_
#define NET_COUNT_DOWN_LATCH_H_
#include <mutex>
#include <condition_variable>
#include "base/macros.h"

namespace net {

class CountDownLatch {
 public:
  explicit CountDownLatch(uint32_t count);
  virtual ~CountDownLatch();
  
  void Wait();
  void CountDown();
  uint32_t count();

 private:
  std::mutex mutex_;
  std::condition_variable cond_;
  uint32_t count_;

  DISALLOW_COPY_AND_ASSIGN(CountDownLatch);
};

} // namespace net
#endif // NET_COUNT_DOWN_LATCH_H_
