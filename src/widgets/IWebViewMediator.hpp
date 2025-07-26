#pragma once

#include "WebViewData.hpp"

class IWebViewMediator {
public:
  IWebViewMediator() = default;

  virtual bool has_webview(WebViewId webview_id) = 0;

  virtual void open_url(const QUrl &url, OpenType open_type, WebViewId webview_id) = 0;
  virtual void set_html(const QString &html, WebViewId webview_id) = 0;
};
