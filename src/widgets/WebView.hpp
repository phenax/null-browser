#pragma once

#include <QMainWindow>
#include <QWebEngineUrlRequestInterceptor>
#include <QWebEngineUrlRequestJob>
#include <QWebEngineUrlSchemeHandler>
#include <QWebEngineView>
#include <QtCore>
#include <cstdint>
#include <functional>
#include <qtypes.h>
#include <qurlquery.h>
#include <unordered_map>

#include "utils.hpp"
#include "widgets/DevtoolsWindow.hpp"

using RpcArgs = std::unordered_map<QString, QVariant>;
using RpcFunc = std::function<void(RpcArgs)>;

class WebView : public QWebEngineView {
  Q_OBJECT

public:
  WebView(uint32_t webview_id, QWebEngineProfile *profile, QWidget *parent_node = nullptr);
  void open_devtools();
  void scroll_increment(int deltax, int deltay);
  void scroll_to_top();
  void scroll_to_bottom();
  void enable_rpc_api();
  void expose_rpc_function(const QString &name, const RpcFunc &action);

  DEFINE_GETTER(get_id, id)

private:
  uint32_t id;
  DevtoolsWindow *devtools_window = nullptr;
  std::unordered_map<QString, RpcFunc> exposed_functions;

  void on_rpc_message(const QString &action, const QUrlQuery &params);
};
