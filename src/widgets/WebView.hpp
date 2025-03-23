#pragma once

#include <QWebEngineView>
#include <QtCore>

class WebView : public QWebEngineView {
  Q_OBJECT

public:
  WebView(QWebEngineProfile *profile, QWidget *parent_node = nullptr);
};
