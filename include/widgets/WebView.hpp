#pragma once

#include <QKeyEvent>
#include <QWebEngineView>
#include <QWidget>
#include <QtCore>

class WebView : public QWebEngineView {
  Q_OBJECT

public:
  WebView(QWebEngineProfile *profile);
};
