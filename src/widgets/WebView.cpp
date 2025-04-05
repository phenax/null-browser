#include <QWebEngineView>
#include <QtCore>

#include "widgets/WebView.hpp"

WebView::WebView(uint32_t webview_id, QWebEngineProfile *profile, QWidget *parent_node)
    : QWebEngineView(profile, parent_node), id(webview_id) {}
