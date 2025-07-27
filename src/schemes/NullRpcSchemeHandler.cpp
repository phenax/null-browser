#include "NullRpcSchemeHandler.hpp"

const QStringList NullRPCSchemeHandler::allowed_origin_schemes = {
    "nullrpc",
};

void NullRPCSchemeHandler::requestStarted(QWebEngineUrlRequestJob *job) {
  auto url = job->requestUrl();
  auto origin = job->initiator();
  qDebug() << "RPC Msg:" << url << " | From: " << job->initiator();

  auto is_origin_allowed = allowed_origin_schemes.contains(origin.scheme());
  if (!is_origin_allowed || url.host().isEmpty()) {
    send_empty_response(job);
    return;
  }

  if (url.host() == "noop") {
    send_empty_response(job, "text/html");
    return;
  }

  if (url.host() == "log") {
    qInfo() << url.query();
    send_empty_response(job);
    return;
  }

  NullRPCMessage message{
      .name = url.host(),
      .params = QUrlQuery(url.query()),
  };
  emit message_received(message);

  // TODO: responses managed with request ids
  send_empty_response(job);
}

void NullRPCSchemeHandler::send_empty_response(QWebEngineUrlRequestJob *job,
                                               const QByteArray &content_type) {
  QBuffer *buffer = new QBuffer(job);
  buffer->open(QIODevice::ReadOnly);
  job->reply(content_type, new QBuffer(job));
}
