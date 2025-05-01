#pragma once

#include <QtCore>
#include <mutex>
#include <unordered_map>

#include "events/Event.hpp"
#include "events/EventHandlerRequest.hpp"

class EventQueue {
public:
  EventQueue() = default;

  void dispatch_event(Event *event);
  void register_event(const EventHandlerRequest &event);

private:
  std::mutex events_mutex;
  std::unordered_map<QString, std::vector<EventHandlerRequest>> events;
};
