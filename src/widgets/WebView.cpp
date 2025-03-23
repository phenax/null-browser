#include <QWebEngineView>
#include <QtCore>

#include "widgets/WebView.hpp"

WebView::WebView(QWebEngineProfile *profile, QWidget *parent_node)
    : QWebEngineView(profile, parent_node) {}
