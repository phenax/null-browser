#pragma once

#include <QLocalServer>
#include <QLocalSocket>
#include <QtCore>

#include "utils.hpp"

class InstanceManager : public QObject {
  Q_OBJECT

public:
  InstanceManager();
  ~InstanceManager() override;
  void write_open_urls_to_socket(const QStringList &urls);
  void write_lua_expr_to_socket(const QString &lua_expr);
  void send_message();

  DEFINE_GETTER(is_server, is_server_mode)

signals:
  void lua_eval_requested(const QString &lua_expr);
  void urls_open_requested(const QStringList &urls);

private slots:
  void handle_new_connection();

protected:
  bool try_server_connection();
  void server_init();

private:
  QLocalServer *server = nullptr;
  QLocalSocket *socket = nullptr;
  bool is_server_mode = false;

  // TODO: On windows, \\.\pipe\null-browser
  const char *socket_path = "/tmp/null-browser-socket.sock";
};
