#include <QtCore>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

#include "AsyncEventLoop.hpp"

AsyncEventLoop::AsyncEventLoop() {
  // UV Loop
  loop = (uv_loop_t *)malloc(sizeof(uv_loop_t));
  uv_loop_init(loop);

  uv_async_init(loop, &asyncHandle, AsyncEventLoop::asyncHandleCallback);
  asyncHandle.data = this;

  loopThread = std::thread(&AsyncEventLoop::runLoop, this);

  // Wait for thread to start
  while (!isLoopRunning)
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

void AsyncEventLoop::wake() { uv_async_send(&asyncHandle); }

void AsyncEventLoop::processTasks() {
  std::queue<std::function<void()>> tasks;

  {
    std::lock_guard<std::mutex> lock(tasksQueueMutex);
    tasksQueue.swap(tasks);
  }

  while (!tasks.empty()) {
    auto task = std::move(tasks.front());
    tasks.pop();
    task();
  }
}

void AsyncEventLoop::runLoop() {
  isLoopRunning = true;
  while (isLoopRunning) {
    // qDebug() << "loop iteration" << uv_loop_alive(loop);
    uv_run(loop, UV_RUN_NOWAIT);
    // qDebug() << "uv_run() returned: " << result;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

AsyncEventLoop::~AsyncEventLoop() {
  if (!isLoopRunning)
    return;
  isLoopRunning = false;

  // Wake it up. Stab it to death. (clear async queue)
  wake();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  {
    std::lock_guard<std::mutex> lock(tasksQueueMutex);
    std::queue<std::function<void()>>().swap(tasksQueue);
  }
  uv_stop(loop);

  // Close all handles
  AsyncEventLoop::closeHandle(reinterpret_cast<uv_handle_t *>(&asyncHandle));
  uv_walk(loop, AsyncEventLoop::closeHandle, nullptr);
  while (uv_run(loop, UV_RUN_NOWAIT) != 0)
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  // TODO: Fix pending handler case (setTimeout(100) wait(20) close() -> error)

  if (loopThread.joinable())
    loopThread.join();

  while (uv_loop_close(loop))
    uv_run(loop, UV_RUN_DEFAULT);

  free(loop);
  loop = nullptr;
}

void AsyncEventLoop::asyncHandleCallback(uv_async_t *handle) {
  auto *runtime = static_cast<AsyncEventLoop *>(handle->data);
  runtime->processTasks();
}

void AsyncEventLoop::closeHandle(uv_handle_t *handle, void *) {
  if (!uv_is_closing(handle)) {
    uv_close(handle, [](uv_handle_t *h) { h->data = nullptr; });
    uv_unref(handle);
  }
}
