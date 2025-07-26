#pragma once

#include <QWebEngineUrlRequestJob>
#include <QWebEngineUrlSchemeHandler>
#include <QtCore>
#include <qurlquery.h>

class NullRPCSchemeHandler : public QWebEngineUrlSchemeHandler {
  Q_OBJECT

public:
  static NullRPCSchemeHandler &instance() {
    static NullRPCSchemeHandler handler;
    return handler;
  }

  void requestStarted(QWebEngineUrlRequestJob *job) override {
    auto url = job->requestUrl();
    qDebug() << "REQ" << url << url.host();

    if (url.host().isEmpty() || url.host() == "noop") {
      job->reply("text/html", new QBuffer(job));
      return;
    }

    if (url.host() == "log") {
      qDebug() << url.query();
      job->reply("text/plain", new QBuffer(job));
      return;
    }

    QUrlQuery query(url.query());
    emit message_received(url.host(), query);

    QByteArray data = "{}";
    QBuffer *buffer = new QBuffer(job);
    buffer->setData(data);
    buffer->open(QIODevice::ReadOnly);
    job->reply("text/plain", buffer);
  }

signals:
  void message_received(const QString &action, QUrlQuery params);

private:
  NullRPCSchemeHandler() = default;
};
