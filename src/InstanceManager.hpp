#pragma once

#include <QLocalServer>
#include <QLocalSocket>
#include <QtCore>

#include "utils.hpp"

class ServerMsg {
public:
  virtual void write_to_socket(QIODevice *socket) = 0;
};

class InstanceManager : public QObject {
  Q_OBJECT

public:
  InstanceManager() = default;
  ~InstanceManager() override;

  void initialize();
  void write_message(ServerMsg &msg);
  void close();

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

class LuaExprServerMsg : public ServerMsg {
public:
  const QString &expr;
  LuaExprServerMsg(auto expr) : expr(expr) {}

  void write_to_socket(QIODevice *socket) override {
    socket->write(("lua " + expr).toStdString().c_str());
    socket->write("\n");
  }
};

class UrlServerMsg : public ServerMsg {
public:
  const QStringList &urls;
  UrlServerMsg(auto urls) : urls(urls) {}

  void write_to_socket(QIODevice *socket) override {
    socket->write(urls.join(" ").toStdString().c_str());
    socket->write("\n");
  }
};
