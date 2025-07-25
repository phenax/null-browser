#pragma once

#include <QWidget>
#include <QtCore>
#include <cstdint>
#include <qwebengineprofile.h>
#include <qwebengineview.h>

#include "widgets/WebView.hpp"

class EdgeDecoration : public QWidget {
  Q_OBJECT

public:
  EdgeDecoration(bool vertical, QWebEngineProfile *profile, QWidget *parent = nullptr);

  void set_size(uint16_t size_value);
  void set_html(const QString &content);
  void set_enabled(bool enabled_value);

private:
  bool vertical;
  WebView *webview = nullptr;
  QWebEngineProfile *profile;
  bool enabled;
  QString html_content;
  uint16_t size = 24;

  void setup_webview();
};
