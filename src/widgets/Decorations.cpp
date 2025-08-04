#include <QWidget>
#include <QtCore>
#include <optional>
#include <qboxlayout.h>
#include <qlabel.h>
#include <qwidget.h>

#include "Decorations.hpp"
#include "WebViewData.hpp"
#include "widgets/EdgeDecoration.hpp"

Decorations::Decorations(QWidget *content_widget, QWebEngineProfile *profile, QWidget *parent)
    : QWidget(parent) {
  decoration_top = new EdgeDecoration(false, profile, this);
  decoration_bottom = new EdgeDecoration(false, profile, this);
  decoration_left = new EdgeDecoration(true, profile, this);
  decoration_right = new EdgeDecoration(true, profile, this);

  auto *vbox = new QVBoxLayout();
  vbox->setContentsMargins(0, 0, 0, 0);
  vbox->setSpacing(0);
  setLayout(vbox);

  auto *container = new QWidget(this);
  auto *hbox = new QHBoxLayout();
  hbox->setContentsMargins(0, 0, 0, 0);
  hbox->setSpacing(0);
  container->setLayout(hbox);

  vbox->addWidget(decoration_top);
  vbox->addWidget(container);
  vbox->addWidget(decoration_bottom);

  hbox->addWidget(decoration_left);
  hbox->addWidget(content_widget);
  hbox->addWidget(decoration_right);
}

void Decorations::set_enabled(DecorationType type, bool enabled) {
  auto decoration = get_decoration_widget_type(type);
  if (decoration.has_value())
    decoration.value()->set_enabled(enabled);
}

bool Decorations::get_enabled(DecorationType type) {
  auto decoration = get_decoration_widget_type(type);
  return decoration.has_value() && decoration.value()->is_enabled();
}

std::optional<EdgeDecoration *> Decorations::get_decoration_widget_type(DecorationType type) {
  switch (type) {
  case DecorationType::DecorationTop:
    return decoration_top;
  case DecorationType::DecorationBottom:
    return decoration_bottom;
  case DecorationType::DecorationLeft:
    return decoration_left;
  case DecorationType::DecorationRight:
    return decoration_right;
  }
  return nullptr;
}

std::optional<EdgeDecoration *> Decorations::get_decoration_widget_by_view_id(WebViewId view_id) {
  for (auto *decoration : decorations())
    if (decoration->get_view_id() == view_id)
      return std::make_optional(decoration);
  return std::nullopt;
}

bool Decorations::has_webview(WebViewId view_id) {
  return get_decoration_widget_by_view_id(view_id).has_value();
}

void Decorations::open_url(const QUrl &url, OpenType /*unused*/, WebViewId view_id) {
  auto decoration = get_decoration_widget_by_view_id(view_id);
  if (decoration.has_value())
    decoration.value()->set_url(url);
}

void Decorations::set_html(const QString &html, WebViewId view_id) {
  auto decoration = get_decoration_widget_by_view_id(view_id);
  if (decoration.has_value())
    decoration.value()->set_html(html);
}

void Decorations::run_javascript(const QString &js_code, WebViewId view_id,
                                 const JsOnResultFunc &on_result) {
  auto decoration = get_decoration_widget_by_view_id(view_id);
  if (decoration.has_value())
    decoration.value()->run_javascript(js_code, on_result);
}

std::optional<WebViewId> Decorations::get_view_id(DecorationType type) {
  auto decoration = get_decoration_widget_type(type);
  return decoration.has_value() ? decoration.value()->get_view_id() : std::nullopt;
}

void Decorations::expose_rpc_function(const QString &name, const RpcFunc &action,
                                      WebViewId view_id) {
  auto decoration = get_decoration_widget_by_view_id(view_id);
  if (decoration.has_value())
    decoration.value()->expose_rpc_function(name, action);
}

void Decorations::reload(WebViewId /* unused */) { qDebug() << "TODO: Impl"; }
