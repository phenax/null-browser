#include <QLocalSocket>
#include <QtCore>
#include <cstdlib>
#include <qnamespace.h>

#include "InstanceManager.hpp"

void InstanceManager::initialize() {
  bool connected = try_server_connection();
  if (!connected) {
    server_init();
  }
}

void InstanceManager::server_init() {
  server = new QLocalServer(this);
  is_server_mode = true;

  connect(server, &QLocalServer::newConnection, this, &InstanceManager::handle_new_connection);

  QLocalServer::removeServer(socket_path);
  if (!server->listen(socket_path)) {
    qCritical() << "Unable to start server:" << server->serverError();
    exit(1); // TODO: Do something else?
  }
}

void InstanceManager::write_message(ServerMsg &msg) {
  if (socket == nullptr)
    return;

  msg.write_to_socket(socket);
  socket->waitForBytesWritten();
}

void InstanceManager::close() {
  socket->waitForBytesWritten();
  socket->close();
}

void InstanceManager::handle_new_connection() {
  if (!server->hasPendingConnections())
    return;

  qDebug() << "New connection";
  auto *client = server->nextPendingConnection();
  if (client == nullptr)
    return;

  connect(client, &QLocalSocket::disconnected, client, &QLocalSocket::deleteLater);

  client->waitForReadyRead();
  while (client->canReadLine()) {
    QString data = client->readLine();

    qDebug() << "RECV:" << data;
    if (data.startsWith("lua ")) {
      auto lua_expr = data.sliced(4);
      emit lua_eval_requested(lua_expr);
    } else {
      QRegularExpression whitespace("\\s+");
      auto urls = data.split(whitespace, Qt::SkipEmptyParts);
      emit urls_open_requested(urls);
    }
  }

  client->close();
}

bool InstanceManager::try_server_connection() {
  socket = new QLocalSocket(this);
  socket->connectToServer(socket_path);

  auto connected = socket->waitForConnected(200);
  if (connected)
    return true;

  socket->close();
  socket->deleteLater();
  socket = nullptr;
  return false;
}

InstanceManager::~InstanceManager() {
  if (socket) {
    socket->disconnectFromServer();
    socket->close();
    socket->deleteLater();
  }
  if (server) {
    server->disconnect();
    server->deleteLater();
  }
}
