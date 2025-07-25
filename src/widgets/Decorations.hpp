#pragma once

#include "widgets/EdgeDecoration.hpp"
#include "widgets/WebViewStack.hpp"
#include <QWidget>
#include <QtCore>
#include <optional>
#include <qwebengineprofile.h>

enum DecorationType : uint8_t {
  DecorationTop = 1,
  DecorationBottom = 2,
  DecorationLeft = 3,
  DecorationRight = 4,
};

class Decorations : public QWidget {
  Q_OBJECT

public:
  Decorations(QWidget *content_widget, QWebEngineProfile *profile, QWidget *parent = nullptr);

  void set_enabled(DecorationType type, bool enabled);
  bool get_enabled(DecorationType type);

  std::optional<WebViewId> get_view_id(DecorationType type) {
    auto decoration = get_decoration_widget(type);
    return decoration.has_value() ? decoration.value()->get_view_id() : std::nullopt;
  }

  bool has_webview(WebViewId view_id) {
    for (auto *decoration : decorations())
      if (decoration->get_view_id() == view_id)
        return true;
    return false;
  }

  void open_url(const QUrl &url, WebViewId view_id) {
    for (auto *decoration : decorations()) {
      if (decoration->get_view_id() == view_id) {
        decoration->set_url(url);
        return;
      }
    }
  }

  void set_html(const QString &html, WebViewId view_id) {
    for (auto *decoration : decorations()) {
      if (decoration->get_view_id() == view_id) {
        decoration->set_html(html);
        return;
      }
    }
  }

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
  std::optional<EdgeDecoration *> get_decoration_widget(DecorationType type);
};
