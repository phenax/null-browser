#pragma once

#include <QWebEngineView>
#include <QtCore>
#include <cstdint>
#include <qtypes.h>

#include "utils.hpp"

class WebView : public QWebEngineView {
  Q_OBJECT

public:
  WebView(uint32_t webview_id, QWebEngineProfile *profile, QWidget *parent_node = nullptr);
  DEFINE_GETTER(get_id, id)

private:
  uint32_t id;
};
