#include <QWidget>
#include <QtCore>
#include <qboxlayout.h>
#include <qlabel.h>
#include <qwebengineview.h>

#include "widgets/WebView.hpp"
#include "widgets/WebViewStack.hpp"

#include "widgets/EdgeDecoration.hpp"

QString default_html_layout = R"HTML(
  {{body}}
  <style>
    :where(html, body) {
      margin: 0;
      padding: 0;
      background: #333;
      color: white;
      overflow: hidden;
    }
  </style>
)HTML";

EdgeDecoration::EdgeDecoration(bool vertical, QWebEngineProfile *profile, QWidget *parent)
    : QWidget(parent), vertical(vertical), profile(profile) {
  auto *vbox = new QVBoxLayout();
  vbox->setContentsMargins(0, 0, 0, 0);
  vbox->setSpacing(0);
  setLayout(vbox);
}

void EdgeDecoration::set_enabled(bool enabled_value) {
  enabled = enabled_value;
  setup_webview();
}

void EdgeDecoration::set_size(u_int16_t size_value) {
  size = size_value;
  setup_webview();
}

void EdgeDecoration::set_html(const QString &content) {
  html_content = content;
  setup_webview();
}

void EdgeDecoration::setup_webview() {
  if (enabled) {
    if (!webview.has_value()) {
      webview = new WebView(WebViewStack::next_webview_id++, profile, this);
      layout()->addWidget(webview.value());
    }

    webview.value()->setHtml(QString(default_html_layout).replace("{{body}}", html_content));
    if (vertical)
      webview.value()->setFixedWidth(size);
    else
      webview.value()->setFixedHeight(size);
  } else {
    if (webview.has_value()) {
      layout()->removeWidget(webview.value());
      webview.value()->deleteLater();
      webview = std::nullopt;
    }
  }
}

void EdgeDecoration::set_url(const QUrl &url) {
  if (!webview.has_value())
    return;
  webview.value()->setUrl(url);
}

std::optional<WebViewId> EdgeDecoration::get_view_id() {
  return webview.has_value() ? std::make_optional(webview.value()->get_id()) : std::nullopt;
}
