#pragma once

#include <QWebEngineNotification>
#include <QtCore>
#include <lua.hpp>

#include "events/Event.hpp"

class NotificationReceivedEvent : public Event {
public:
  std::unique_ptr<QWebEngineNotification> notification;

  NotificationReceivedEvent(std::unique_ptr<QWebEngineNotification> notification)
      : notification(std::move(notification)) {
    kind = "NotificationReceived";
  }

  void lua_push(lua_State *state) const override {
    lua_newtable(state);
    SET_FIELD("type", string, kind.toStdString().c_str())
    SET_FIELD("title", string, notification->title().toStdString().c_str())
    SET_FIELD("message", string, notification->message().toStdString().c_str())
    SET_FIELD("tag", string, notification->tag().toStdString().c_str())
    SET_FIELD("origin", string, notification->origin().toString().toStdString().c_str())

    SET_FIELD_CLOSURE_WITH_SELF("click", [](lua_State *state) {
      auto *event = GET_CLOSURE_SELF(NotificationReceivedEvent *);
      event->notification->click();
      lua_pushnil(state);
      return 1;
    });

    SET_FIELD_CLOSURE_WITH_SELF("show", [](lua_State *state) {
      auto *event = GET_CLOSURE_SELF(NotificationReceivedEvent *);
      event->notification->show();
      lua_pushnil(state);
      return 1;
    });

    SET_FIELD_CLOSURE_WITH_SELF("close", [](lua_State *state) {
      auto *event = GET_CLOSURE_SELF(NotificationReceivedEvent *);
      event->notification->close();
      lua_pushnil(state);
      return 1;
    });
  }
};
