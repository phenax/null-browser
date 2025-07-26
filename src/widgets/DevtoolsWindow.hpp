#pragma once

#include <QMainWindow>
#include <QWebEngineProfile>
#include <QWebEngineView>
#include <QtCore>

#include "utils.hpp"

class DevtoolsWindow : public QMainWindow {
  Q_OBJECT

public:
  DevtoolsWindow(QWebEngineProfile *profile, QWidget *parent = nullptr,
                 Qt::WindowFlags flags = Qt::WindowFlags())
      : QMainWindow(parent, flags) {
    webengineview = new QWebEngineView(profile, this);
    this->setCentralWidget(webengineview);
  }

  DELEGATE(webengineview, page, page)

signals:
  void closed();

protected:
  void closeEvent(QCloseEvent * /* event */) override { emit closed(); }

private:
  QWebEngineView *webengineview;
};
