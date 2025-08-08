#include "NullSchemeHandler.hpp"
#include <qstringview.h>
#include <qwebengineurlrequestjob.h>

void NullSchemeHandler::requestStarted(QWebEngineUrlRequestJob *job) {
  auto url = job->requestUrl();

  if (url.host() == "docs") {
    QBuffer *buffer = new QBuffer(job);
    buffer->setData(read_static_docs_file(url.path()));
    buffer->open(QIODevice::ReadOnly);
    job->reply(get_content_type(url.path()), buffer);
  } else {
    job->reply("text/html", new QBuffer(job));
  }
}

QByteArray NullSchemeHandler::read_static_docs_file(const QString &path) {
  QFile file(NULL_DOCS_DIR + path);
  if (!file.exists())
    return read_index_html();

  if (file.open(QIODevice::ReadOnly)) {
    auto contents = file.readAll();
    file.close();
    return contents;
  }
  return QByteArray{};
}

QByteArray NullSchemeHandler::read_index_html() {
  QFile file(NULL_DOCS_DIR + QString("/index.html"));
  qDebug() << ":::" << file.exists();
  if (file.open(QIODevice::ReadOnly)) {
    auto contents = file.readAll();
    file.close();
    return contents;
  }
  return QByteArray{};
}

QByteArray NullSchemeHandler::get_content_type(const QString &path) {
  if (path.endsWith(".css"))
    return "text/css";
  return "text/html";
}
