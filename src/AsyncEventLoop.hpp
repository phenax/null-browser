#pragma once

#include <atomic>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <uv.h>

#include "utils.hpp"

class AsyncEventLoop {
public:
  AsyncEventLoop();
  ~AsyncEventLoop();

  void wake();
  DEFINE_GETTER(getUVLoop, loop)

  template <typename F> void queueTask(F &&task) {
    {
      std::lock_guard<std::mutex> lock(tasksQueueMutex);
      tasksQueue.push(std::forward<F>(task));
    }
    wake();
  }

protected:
  void runLoop();
  void processTasks();
  static void asyncHandleCallback(uv_async_t *handle);
  static void closeHandle(uv_handle_t *handle, void *arg = nullptr);

private:
  uv_loop_t *loop;
  std::thread loopThread;
  uv_async_t asyncHandle;
  std::atomic<bool> isLoopRunning = false;
  std::queue<std::function<void()>> tasksQueue;
  std::mutex tasksQueueMutex;
};
