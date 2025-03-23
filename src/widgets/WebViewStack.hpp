#pragma once

#include <QStackedLayout>
#include <QWebEngineProfile>
#include <cstdint>
#include <vector>

#include "Configuration.hpp"
#include "widgets/WebView.hpp"

enum OpenType : uint8_t {
  OpenUrl,
  OpenUrlInTab,
  OpenUrlInBgTab,
  OpenUrlInWindow
};

struct Tab {
  QString url;
  QString title;
};

class WebViewStack : public QWidget {
  Q_OBJECT

public:
  WebViewStack(const Configuration *configuration,
               QWebEngineProfile *profile = new QWebEngineProfile,
               QWidget *parent = nullptr);

  void open_url(const QUrl &url, OpenType open_type = OpenType::OpenUrl);

  std::vector<QUrl> urls();
  QList<Tab> get_tab_list();
  uint32_t current_web_view_index();
  uint32_t count();
  QUrl current_url();

  void focus_web_view(qsizetype index);
  void next();
  void previous();

  void close(qsizetype index);
  void close_current();

private slots:
  void on_new_web_view_request(const QWebEngineNewWindowRequest &request);

protected:
  void set_current_url(const QUrl &url);
  WebView *create_new_web_view(const QUrl &url, bool focus = false);

private:
  const Configuration *configuration;
  QWebEngineProfile *profile;
  QStackedLayout *layout;
  QList<WebView *> web_view_list;
};
