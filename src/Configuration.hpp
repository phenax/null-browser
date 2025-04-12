#pragma once

#include <QtCore>

class Configuration : public QObject {
  Q_OBJECT

public:
  using QObject::QObject;

  QUrl new_view_url = QUrl("https://lite.duckduckgo.com");

  bool close_window_when_no_views = true;
};
