#pragma once

#include <QWidget>
#include <QtCore>
#include <cstdint>
#include <optional>
#include <qwebengineprofile.h>
#include <qwebengineview.h>

#include "utils.hpp"
#include "widgets/WebView.hpp"
#include "widgets/WebViewStack.hpp"

class EdgeDecoration : public QWidget {
  Q_OBJECT

public:
  EdgeDecoration(bool vertical, QWebEngineProfile *profile, QWidget *parent = nullptr);

  void set_size(uint16_t size_value);
  void set_html(const QString &content);
  void set_enabled(bool enabled_value);
  void set_url(const QUrl &url) {
    if (!webview.has_value())
      return;
    webview.value()->setUrl(url);
  }

  DEFINE_GETTER(is_enabled, enabled)

  std::optional<WebViewId> get_view_id() {
    return webview.has_value() ? std::make_optional(webview.value()->get_id()) : std::nullopt;
  }

private:
  bool vertical;
  std::optional<WebView *> webview = std::nullopt;
  QWebEngineProfile *profile;
  bool enabled = false;
  QString html_content = "";
  uint16_t size = 24;

  void setup_webview();
};
