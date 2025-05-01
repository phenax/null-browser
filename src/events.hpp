#pragma once

#include <QtCore>
#include <functional>
#include <lua.hpp>
#include <memory>
#include <qwebenginepermission.h>

#include "LuaRuntime.hpp"
#include "lua.h"
#include "widgets/BrowserWindow.hpp"
#include "widgets/WebViewStack.hpp"

#define SET_FIELD(NAME, TYPE, VALUE)                                                               \
  lua_pushstring(state, NAME);                                                                     \
  lua_push##TYPE(state, VALUE);                                                                    \
  lua_settable(state, -3);

class BrowserEvent {
public:
  QString kind = "-";

  virtual void lua_push(lua_State *state) const {
    lua_newtable(state);
    SET_FIELD("type", string, kind.toStdString().c_str())
  };
};

class UrlChangedEvent : public BrowserEvent {
public:
  const QString &url;
  const WebViewId webview_id;
  const WindowId win_id;

  UrlChangedEvent(const QString &url, WebViewId webview_id, WindowId win_id)
      : url(url), webview_id(webview_id), win_id(win_id) {
    kind = "UrlChanged";
  }

  void lua_push(lua_State *state) const override {
    lua_newtable(state);
    SET_FIELD("type", string, kind.toStdString().c_str())
    SET_FIELD("view_id", integer, webview_id)
    SET_FIELD("win_id", integer, win_id)
    SET_FIELD("url", string, url.toStdString().c_str())
  }
};

class PermissionRequestEvent : public BrowserEvent {
public:
  const QWebEnginePermission::PermissionType premission_type;
  const WebViewId webview_id;
  const WindowId win_id;
  std::function<void()> accept_request;
  std::function<void()> reject_request;

  PermissionRequestEvent(QWebEnginePermission::PermissionType type,
                         std::function<void()> accept_request, std::function<void()> reject_request,
                         WebViewId webview_id, WindowId win_id)
      : premission_type(type), webview_id(webview_id), win_id(win_id),
        accept_request(std::move(accept_request)), reject_request(std::move(reject_request)) {
    kind = "PermissionRequested";
  }

  static PermissionRequestEvent *from_permission(std::shared_ptr<QWebEnginePermission> &permission,
                                                 WebViewId webview_id, WindowId win_id) {
    auto accept = [permission]() {
      QMetaObject::invokeMethod(qApp, [=]() { permission->grant(); }, Qt::QueuedConnection);
    };
    auto reject = [permission]() {
      QMetaObject::invokeMethod(qApp, [=]() { permission->deny(); }, Qt::QueuedConnection);
    };
    // TODO: Manage delete for permission object
    return new PermissionRequestEvent(permission->permissionType(), accept, reject, webview_id,
                                      win_id);
  }

  [[nodiscard]] const char *permission_type() const {
    switch (premission_type) {
    case QWebEnginePermission::PermissionType::MediaAudioCapture:
      return "MediaAudioCapture";
    case QWebEnginePermission::PermissionType::MediaVideoCapture:
      return "MediaVideoCapture";
    case QWebEnginePermission::PermissionType::MediaAudioVideoCapture:
      return "MediaAudioVideoCapture";
    case QWebEnginePermission::PermissionType::DesktopVideoCapture:
      return "DesktopVideoCapture";
    case QWebEnginePermission::PermissionType::DesktopAudioVideoCapture:
      return "DesktopAudioVideoCapture";
    case QWebEnginePermission::PermissionType::MouseLock:
      return "MouseLock";
    case QWebEnginePermission::PermissionType::Notifications:
      return "Notifications";
    case QWebEnginePermission::PermissionType::Geolocation:
      return "Geolocation";
    case QWebEnginePermission::PermissionType::ClipboardReadWrite:
      return "ClipboardReadWrite";
    case QWebEnginePermission::PermissionType::LocalFontsAccess:
      return "LocalFontsAccess";
    default:
      return "Unsupported";
    }
  }

  void lua_push(lua_State *state) const override {
    lua_newtable(state);
    SET_FIELD("type", string, kind.toStdString().c_str())
    SET_FIELD("permission_type", string, permission_type())
    SET_FIELD("view_id", integer, webview_id)
    SET_FIELD("win_id", integer, win_id)

    lua_pushstring(state, "accept");
    lua_pushlightuserdata(state, (void *)this);
    lua_pushcclosure(
        state,
        [](lua_State *state) {
          void *userdata = lua_touserdata(state, lua_upvalueindex(1));
          auto *event = static_cast<PermissionRequestEvent *>(userdata);
          event->accept_request();
          lua_pushnil(state);
          return 1;
        },
        1);
    lua_settable(state, -3);

    lua_pushstring(state, "reject");
    lua_pushlightuserdata(state, (void *)this);
    lua_pushcclosure(
        state,
        [](lua_State *state) {
          void *userdata = lua_touserdata(state, lua_upvalueindex(1));
          auto *event = static_cast<PermissionRequestEvent *>(userdata);
          event->reject_request();
          lua_pushnil(state);
          return 1;
        },
        1);
    lua_settable(state, -3);
  }
};

struct EventHandlerRequest {
  std::vector<QString> event_names;
  std::vector<QString> patterns;
  std::function<void(BrowserEvent *)> handler;
  int function_ref;
};
