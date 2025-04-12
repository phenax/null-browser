#pragma once

#include <QtCore>

class Configuration : public QObject {
  Q_OBJECT

public:
  using QObject::QObject;

  QString new_view_url = "https://duckduckgo.com";

  bool close_window_when_no_views = true;
};
