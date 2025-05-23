#include <mutex>
#include <unordered_map>

#include "EventQueue.hpp"
#include "LuaRuntime.hpp"

void EventQueue::dispatch_event(Event *event) {
  auto &runtime = LuaRuntime::instance();

  runtime.queue_task([this, event]() {
    std::unordered_map<QString, std::vector<EventHandlerRequest>> event_map;
    {
      const std::lock_guard<std::mutex> lock(events_mutex);
      event_map = events;
    }

    if (!event_map.contains(event->kind))
      return;

    for (auto &event_handler : event_map.at(event->kind)) {
      // TODO: Pattern filter
      event_handler.handler(event);
    }

    // TODO: to delete or not to delete
    // delete event;
  });
}

void EventQueue::register_event(const EventHandlerRequest &event) {
  const std::lock_guard<std::mutex> lock(events_mutex);
  for (const auto &event_name : event.event_names) {
    if (!events.contains(event_name))
      events.insert({event_name, {}});
    events.at(event_name).emplace_back(event);
  }
}
