#include <QtCore>
#include <chrono>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <uv.h>

#include "AsyncEventLoop.hpp"

AsyncEventLoop::AsyncEventLoop() {
  // UV Loop
  loop = (uv_loop_t *)malloc(sizeof(uv_loop_t));
  uv_loop_init(loop);

  uv_async_init(loop, &async_handle, AsyncEventLoop::async_handle_callback);
  async_handle.data = this;

  loop_thread = std::thread(&AsyncEventLoop::run_loop, this);

  // Wait for thread to start
  while (!is_loop_running)
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

void AsyncEventLoop::wake() { uv_async_send(&async_handle); }

void AsyncEventLoop::process_tasks() {
  std::queue<std::function<void()>> tasks;

  {
    const std::lock_guard<std::mutex> lock(tasks_queue_mutex);
    if (tasks_queue.empty())
      return;

    tasks_queue.swap(tasks);
  }

  while (!tasks.empty()) {
    auto task = std::move(tasks.front());
    tasks.pop();
    task();
  }
}

void AsyncEventLoop::run_loop() {
  is_loop_running = true;
  while (is_loop_running) {
    uv_run(loop, UV_RUN_NOWAIT);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

AsyncEventLoop::~AsyncEventLoop() {
  if (!is_loop_running)
    return;
  is_loop_running = false;

  flush_tasks();
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  uv_stop(loop);

  // Close all handles
  AsyncEventLoop::close_handle(reinterpret_cast<uv_handle_t *>(&async_handle));
  uv_walk(loop, AsyncEventLoop::close_handle, nullptr);

  if (loop_thread.joinable())
    loop_thread.join();

  // Close loop
  uv_loop_close(loop);

  uv_run(loop, UV_RUN_DEFAULT);

  if (uv_loop_alive(loop) > 0)
    qDebug() << "WARNING: Loop still has active handles!";

  free(loop);
  loop = nullptr;
}

void AsyncEventLoop::flush_tasks() {
  {
    const std::lock_guard<std::mutex> lock(tasks_queue_mutex);
    std::queue<std::function<void()>>().swap(tasks_queue);
  }
  wake();
}

void AsyncEventLoop::async_handle_callback(uv_async_t *handle) {
  auto *runtime = static_cast<AsyncEventLoop *>(handle->data);
  runtime->process_tasks();
}

void AsyncEventLoop::close_handle(uv_handle_t *handle, void * /*unused*/) {
  if (!uv_is_closing(handle)) {
    uv_close(handle, [](uv_handle_t *handle) { handle->data = nullptr; });
    uv_unref(handle);
  }
}
