#pragma once

#include <QMainWindow>

#include "Configuration.hpp"
#include "WindowMediator.hpp"
#include "utils.hpp"

using WindowId = qsizetype;

class BrowserWindow : public QMainWindow {
  Q_OBJECT

public:
  BrowserWindow(const Configuration &configuration,
                const QString &start_url = "");

  DEFINE_GETTER(mediator, win_mediator)
  DEFINE_GETTER(get_id, win_id)
  DEFINE_SETTER(set_id, win_id)

  bool on_window_key_event(QKeyEvent *event);

  void closeEvent(QCloseEvent * /*event*/) override;

signals:
  void closed();
  // void new_window_requested(const QUrl &url);

private:
  WindowMediator *win_mediator;
  const Configuration &configuration;

  WindowId win_id = -1;
};

using WindowMap = std::unordered_map<uint64_t, BrowserWindow *>;
