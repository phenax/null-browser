#pragma once

#include <QtCore>
#include <cstdio>

class Configuration : public QObject {
  Q_OBJECT

public:
  using QObject::QObject;

  QUrl newTabUrl = QUrl("https://lite.duckduckgo.com");
};
