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

  void requestStarted(QWebEngineUrlRequestJob *job) override {
    auto url = job->requestUrl();
    auto origin = job->initiator();
    qDebug() << "REQ" << url << " | From: " << job->initiator();

    if (origin.scheme() != "nullrpc" || url.host().isEmpty() || url.host() == "noop") {
      job->reply("text/html", new QBuffer(job));
      return;
    }

    if (url.host() == "log") {
      qDebug() << url.query();
      job->reply("text/plain", new QBuffer(job));
      return;
    }

    NullRPCMessage message{
        .name = url.host(),
        .params = QUrlQuery(url.query()),
    };
    emit message_received(message);

    // TODO: responses managed with request ids
    QByteArray data = "{}";
    QBuffer *buffer = new QBuffer(job);
    buffer->setData(data);
    buffer->open(QIODevice::ReadOnly);
    job->reply("text/plain", buffer);
  }

signals:
  void message_received(NullRPCMessage message);

private:
  NullRPCSchemeHandler() = default;
};
