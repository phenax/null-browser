#pragma once

#include <QtCore>

#include "events/Event.hpp"

struct EventHandlerRequest {
  std::vector<QString> event_names;
  std::vector<QString> patterns;
  std::function<void(Event *)> handler;
  int function_ref;
};
