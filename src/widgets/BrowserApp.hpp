#pragma once

#include "widgets/BrowserWindow.hpp"

class BrowserApp : public QObject {
  Q_OBJECT

public:
  BrowserApp();

  BrowserWindow *create_window(const QStringList &urls = {});

protected:
  bool eventFilter(QObject *target, QEvent *event) override;

private:
  Configuration configuration;
};
