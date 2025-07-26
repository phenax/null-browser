#pragma once

#include <QWidget>
#include <QtCore>
#include <cstdint>
#include <optional>
#include <qwebengineprofile.h>
#include <qwebengineview.h>

#include "WebViewData.hpp"
#include "utils.hpp"
#include "widgets/WebView.hpp"

class EdgeDecoration : public QWidget {
  Q_OBJECT

public:
  EdgeDecoration(bool vertical, QWebEngineProfile *profile, QWidget *parent = nullptr);

  void set_size(uint16_t size_value);
  void set_html(const QString &content);
  void set_enabled(bool enabled_value);
  void set_url(const QUrl &url);
  std::optional<WebViewId> get_view_id();
  void expose_rpc_function(const QString &name, const RpcFunc &action);

  DEFINE_GETTER(is_enabled, enabled)

private:
  bool vertical;
  std::optional<WebView *> webview = std::nullopt;
  QWebEngineProfile *profile;
  bool enabled = false;
  QString html_content = "";
  uint16_t size = 20;

  void setup_webview();
};
