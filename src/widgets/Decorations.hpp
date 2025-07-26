#pragma once

#include "WebViewData.hpp"
#include "widgets/EdgeDecoration.hpp"
#include "widgets/IWebViewMediator.hpp"
#include <QWidget>
#include <QtCore>
#include <optional>
#include <qwebengineprofile.h>

/// @see DecorationType in ./lua/null-browser/api.lua
enum DecorationType : uint8_t {
  DecorationTop = 1,
  DecorationBottom = 2,
  DecorationLeft = 3,
  DecorationRight = 4,
};

class Decorations : public QWidget, public IWebViewMediator {
  Q_OBJECT

public:
  Decorations(QWidget *content_widget, QWebEngineProfile *profile, QWidget *parent = nullptr);

  void set_enabled(DecorationType type, bool enabled);
  bool get_enabled(DecorationType type);

  std::optional<WebViewId> get_view_id(DecorationType type);
  bool has_webview(WebViewId view_id) override;
  void open_url(const QUrl &url, OpenType /*unused*/, WebViewId view_id) override;
  void set_html(const QString &html, WebViewId view_id) override;

private:
  EdgeDecoration *decoration_top;
  EdgeDecoration *decoration_bottom;
  EdgeDecoration *decoration_left;
  EdgeDecoration *decoration_right;

  std::vector<EdgeDecoration *> decorations() {
    return {
        decoration_top,
        decoration_bottom,
        decoration_left,
        decoration_right,
    };
  }
  std::optional<EdgeDecoration *> get_decoration_widget_type(DecorationType type);
  std::optional<EdgeDecoration *> get_decoration_widget_by_view_id(WebViewId view_id);
};
