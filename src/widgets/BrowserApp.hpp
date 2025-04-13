#pragma once

#include "widgets/BrowserWindow.hpp"
#include <qlist.h>

class BrowserApp : public QObject {
  Q_OBJECT

public:
  BrowserApp();

  BrowserWindow *create_window(const QStringList &urls = {});

protected:
  bool eventFilter(QObject *target, QEvent *event) override;

private:
  Configuration configuration;
  QWebEngineProfile default_profile{"default"};
  QList<QWebEngineProfile *> profiles{&default_profile};
};
