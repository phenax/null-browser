#pragma once

#include <QtCore>
#include <qwebengineprofile.h>
#include <unordered_map>

class Configuration : public QObject {
  Q_OBJECT

private:
  std::unordered_map<QString, QVariant> config_map{
      {"new_view_url", "https://duckduckgo.com"},
      {"close_window_when_no_views", true},
      {"user_agent", QWebEngineProfile::defaultProfile()->httpUserAgent()},
      {"downloads_dir", QWebEngineProfile::defaultProfile()->downloadPath()},
  };

public:
  using QObject::QObject;

  void set_config(const QString &name, const QVariant &value) {
    config_map[name] = value;
    on_update(name, value);
  }

  QVariant get_config(const QString &name, QVariant default_value = "") const {
    if (!config_map.contains(name))
      return default_value;

    return config_map.at(name);
  }

  bool close_window_when_no_views() const {
    return get_config("close_window_when_no_views").toBool();
  }
  QString new_view_url() const { return get_config("new_view_url").toString(); }
  QString user_agent() const { return get_config("user_agent").toString(); }
  QString downloads_dir() const { return get_config("downloads_dir").toString(); }

private:
  void on_update(const QString &name, const QVariant &value) {
    if (name == "user_agent")
      emit user_agent_updated(value.toString());
    if (name == "downloads_dir")
      emit downloads_dir_updated(value.toString());
  }

signals:
  void user_agent_updated(const QString &value);
  void downloads_dir_updated(const QString &value);
};
