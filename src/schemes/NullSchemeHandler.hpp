#pragma once

#include <QWebEngineUrlRequestJob>
#include <QWebEngineUrlSchemeHandler>
#include <QtCore>
#include <qcontainerfwd.h>
#include <qdebug.h>
#include <qurlquery.h>

class NullSchemeHandler : public QWebEngineUrlSchemeHandler {
  Q_OBJECT

public:
  static NullSchemeHandler &instance() {
    static NullSchemeHandler handler;
    return handler;
  }

  void requestStarted(QWebEngineUrlRequestJob *job) override;

private:
  NullSchemeHandler() = default;

  QByteArray read_static_docs_file(const QString &path);
  QByteArray read_index_html();
  QByteArray get_content_type(const QString &path);
};
