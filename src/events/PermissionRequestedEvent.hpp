#pragma once

#include <QWebEnginePermission>
#include <QtCore>
#include <lua.hpp>

#include "events/Event.hpp"
#include "widgets/BrowserWindow.hpp"
#include "widgets/WebViewStack.hpp"

class PermissionRequestedEvent : public Event {
public:
  const QWebEnginePermission::PermissionType premission_type;
  const WebViewId webview_id;
  const WindowId win_id;
  std::function<void()> accept_request;
  std::function<void()> reject_request;

  PermissionRequestedEvent(QWebEnginePermission::PermissionType type,
                           std::function<void()> accept_request,
                           std::function<void()> reject_request, WebViewId webview_id,
                           WindowId win_id)
      : premission_type(type), webview_id(webview_id), win_id(win_id),
        accept_request(std::move(accept_request)), reject_request(std::move(reject_request)) {
    kind = "PermissionRequested";
  }

  static PermissionRequestedEvent *
  from_permission(std::shared_ptr<QWebEnginePermission> &permission, WebViewId webview_id,
                  WindowId win_id) {
    auto accept = [permission]() {
      QMetaObject::invokeMethod(qApp, [=]() { permission->grant(); }, Qt::QueuedConnection);
    };
    auto reject = [permission]() {
      QMetaObject::invokeMethod(qApp, [=]() { permission->deny(); }, Qt::QueuedConnection);
    };
    // TODO: Manage delete for permission object
    return new PermissionRequestedEvent(permission->permissionType(), accept, reject, webview_id,
                                        win_id);
  }

  void lua_push(lua_State *state) const override {
    lua_newtable(state);
    SET_FIELD("type", string, kind.toStdString().c_str())
    SET_FIELD("permission_type", string, permission_type())
    SET_FIELD("view_id", integer, webview_id)
    SET_FIELD("win_id", integer, win_id)

    SET_FIELD_CLOSURE_WITH_SELF("accept", [](lua_State *state) {
      auto *event = GET_CLOSURE_SELF(PermissionRequestedEvent *);
      event->accept_request();
      lua_pushnil(state);
      return 1;
    });

    SET_FIELD_CLOSURE_WITH_SELF("reject", [](lua_State *state) {
      auto *event = GET_CLOSURE_SELF(PermissionRequestedEvent *);
      event->reject_request();
      lua_pushnil(state);
      return 1;
    });
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
};
