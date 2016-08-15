#ifndef NET_PC_QUEUE_H_
#define NET_PC_QUEUE_H_
#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>
#include "base/macros.h"

namespace net {

class PCQueue {
 public:
  PCQueue() {}
  virtual ~PCQueue() {}
   
  void Enqueue(void* item);
  void EnqueueN(void* item, int ntimes);

  void* Dequeue();
  void* TryDequeue(bool&);
  uint32_t TryDequeueN(uint32_t n, std::vector<void*>& queue);

  int size();

 protected:
  std::queue<void*> queue_;
  
  std::condition_variable cond_;
  std::mutex mutex_;

 private:
  DISALLOW_COPY_AND_ASSIGN(PCQueue);
};

} // namespace net
#endif // NET_PC_QUEUE_H_
