#ifndef __SYNCQUEUE_H__
#define __SYNCQUEUE_H__

#include <queue>
#include <mutex>
#include <condition_variable>

// A threadsafe-queue.
template <class T>
class SyncQueue
{
private:
  std::queue<T> q;
  mutable std::mutex m;
  std::condition_variable c;

public:
  SyncQueue(): q(), m(), c() {}

  ~SyncQueue() {}

  // Add an element to the queue.
  void enqueue(T t) {
    std::lock_guard<std::mutex> lock(m);
    q.push(t);
    c.notify_one();
  }

  // Get the "front"-element.
  // If the queue is empty, wait till a element is avaiable.
  T dequeue(void)
  {
    std::unique_lock<std::mutex> lock(m);

    // release lock as long as the wait and reaquire it afterwards.
    while(q.empty()) {
      c.wait(lock);
    }

    T val = q.front();
    q.pop();
    return val;
  }
};

#endif
