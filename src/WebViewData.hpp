#pragma once

#include <QtCore>

using WebViewId = qsizetype;

enum OpenType : uint8_t {
  OpenUrl,
  OpenUrlInView,
  OpenUrlInBgView,
  OpenUrlInWindow,
};

struct WebViewData {
  WebViewId id;
  QString url;
  QString title;
};
