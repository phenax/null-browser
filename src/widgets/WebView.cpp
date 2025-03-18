#include <QWidget>
#include <QtCore>

#include "widgets/WebView.hpp"

WebView::WebView(QWebEngineProfile *profile) : QWebEngineView(profile) {}
