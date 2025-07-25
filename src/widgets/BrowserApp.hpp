#pragma once

#include "Configuration.hpp"
#include "widgets/BrowserWindow.hpp"
#include <qlist.h>
#include <qwebengineprofile.h>

class BrowserApp : public QObject {
  Q_OBJECT

public:
  BrowserApp(Configuration &configuration);

  BrowserWindow *create_window(const QStringList &urls = {});

protected:
  bool eventFilter(QObject *target, QEvent *event) override;

private:
  Configuration &configuration;
  QWebEngineProfile default_profile{"default"};
  QList<QWebEngineProfile *> profiles{&default_profile};

  void setup_profile(QWebEngineProfile *profile);
};
