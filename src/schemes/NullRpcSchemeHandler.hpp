#pragma once

#include <QWebEngineUrlRequestJob>
#include <QWebEngineUrlSchemeHandler>
#include <QtCore>
#include <qurlquery.h>

struct NullRPCMessage {
  QString name;
  QUrlQuery params;
};

class NullRPCSchemeHandler : public QWebEngineUrlSchemeHandler {
  Q_OBJECT

public:
  static NullRPCSchemeHandler &instance() {
    static NullRPCSchemeHandler handler;
    return handler;
  }

  void requestStarted(QWebEngineUrlRequestJob *job) override;

signals:
  void message_received(NullRPCMessage message);

private:
  NullRPCSchemeHandler() = default;
  static const QStringList allowed_origin_schemes;

  void send_empty_response(QWebEngineUrlRequestJob *job,
                           const QByteArray &content_type = "text/plain");
};
