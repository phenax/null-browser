#pragma once

#include <QtCore>

class Configuration : public QObject {
  Q_OBJECT

public:
  using QObject::QObject;

  QUrl new_tab_url = QUrl("https://lite.duckduckgo.com");
};
