#pragma once

#include <QWebEngineUrlRequestJob>
#include <QWebEngineUrlSchemeHandler>
#include <QtCore>
#include <qcontainerfwd.h>
#include <qdebug.h>
#include <qurlquery.h>

// TODO: Install doc dir and pass this path during build
#define NULL_DOCS_DIR "./doc"

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
