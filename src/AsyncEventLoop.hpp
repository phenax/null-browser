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
  DEFINE_GETTER(get_uv_loop, loop)

  template <typename F> void queue_task(F &&task) {
    {
      const std::lock_guard<std::mutex> lock(tasks_queue_mutex);
      tasks_queue.push(std::forward<F>(task));
    }
    wake();
  }

protected:
  void run_loop();
  void process_tasks();
  void flush_tasks();
  static void async_handle_callback(uv_async_t *handle);
  static void close_handle(uv_handle_t *handle, void *arg = nullptr);

private:
  uv_loop_t *loop;
  std::thread loop_thread;
  uv_async_t async_handle;
  std::atomic<bool> is_loop_running = false;
  std::queue<std::function<void()>> tasks_queue;
  std::mutex tasks_queue_mutex;
};
