#pragma once

#include "WebViewData.hpp"
#include "widgets/WebView.hpp"

class IWebViewMediator {
public:
  IWebViewMediator() = default;

  virtual bool has_webview(WebViewId webview_id) = 0;
  virtual void open_url(const QUrl &url, OpenType open_type, WebViewId webview_id) = 0;
  virtual void set_html(const QString &html, WebViewId webview_id) = 0;
  virtual void expose_rpc_function(const QString &name, const RpcFunc &action,
                                   WebViewId webview_id) = 0;
  virtual void run_javascript(const QString &js_code, WebViewId webview_id) = 0;
  virtual void reload(WebViewId webview_id) = 0;
};
