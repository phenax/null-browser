#pragma once

#include <QMainWindow>
#include <QWebEngineView>
#include <QtCore>
#include <cstdint>
#include <qtypes.h>

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

class WebView : public QWebEngineView {
  Q_OBJECT

public:
  WebView(uint32_t webview_id, QWebEngineProfile *profile, QWidget *parent_node = nullptr);
  DEFINE_GETTER(get_id, id)

  void open_devtools();

private:
  uint32_t id;

  DevtoolsWindow *devtools_window = nullptr;
};
