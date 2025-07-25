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
      background: black;
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

void EdgeDecoration::set_html(const QString &content) {
  html_content = content;
  setup_webview();
}

void EdgeDecoration::set_size(u_int16_t size_value) {
  size = size_value;
  setup_webview();
}

void EdgeDecoration::set_enabled(bool enabled_value) {
  enabled = enabled_value;
  setup_webview();
}

void EdgeDecoration::setup_webview() {
  if (enabled) {
    if (webview == nullptr) {
      webview = new WebView(WebViewStack::next_webview_id++, profile, this);
      layout()->addWidget(webview);
    }

    webview->setHtml(default_html_layout.replace("{{body}}", html_content));
    if (vertical)
      setFixedWidth(size);
    else
      setFixedHeight(size);
  } else {
    if (webview != nullptr) {
      layout()->removeWidget(webview);
      webview->deleteLater();
      webview = nullptr;
    }
  }
}
