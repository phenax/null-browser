#pragma once

#include "utils.hpp"
#include <QtCore>
#include <qwebengineprofile.h>
#include <unordered_map>

class Configuration : public QObject {
  Q_OBJECT

private:
  QDir config_dir = QDir(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
  std::unordered_map<QString, QVariant> config_map{
      {"new_view_url", "https://duckduckgo.com"},
      {"close_window_when_no_views", true},
      {"user_agent", QWebEngineProfile::defaultProfile()->httpUserAgent()},
      {"downloads_dir", QWebEngineProfile::defaultProfile()->downloadPath()},
      {"permissions_persistance", "always"},
  };
  std::unordered_map<QString, QVariant> readonly_config_map{
      {"app_data_dir", QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)},
      {"config_dir", config_dir.path()},
      {"null_assets_dir", PROJECT_ASSETS_PATH},
      {"null_docs_dir", PROJECT_DOCS_PATH},
  };

public:
  using QObject::QObject;

  void set_config(const QString &name, const QVariant &value) {
    config_map[name] = value;
    on_update(name, value);
  }

  QVariant get_config(const QString &name, QVariant default_value = "") const {
    if (readonly_config_map.contains(name))
      return readonly_config_map.at(name);

    if (config_map.contains(name))
      return config_map.at(name);

    return default_value;
  }

  bool close_window_when_no_views() const {
    return get_config("close_window_when_no_views").toBool();
  }
  QString new_view_url() const { return get_config("new_view_url").toString(); }
  QString user_agent() const { return get_config("user_agent").toString(); }
  QString downloads_dir() const { return get_config("downloads_dir").toString(); }
  QString permissions_persistance() const {
    return get_config("permissions_persistance").toString();
  }

  QWebEngineProfile::PersistentPermissionsPolicy permission_persistance_policy() const {
    return to_permission_persistance_policy(permissions_persistance());
  }

  static QWebEngineProfile::PersistentPermissionsPolicy
  to_permission_persistance_policy(const QString &persistance) {
    if (persistance == "session")
      return QWebEngineProfile::PersistentPermissionsPolicy::StoreInMemory;
    if (persistance == "never")
      return QWebEngineProfile::PersistentPermissionsPolicy::AskEveryTime;
    return QWebEngineProfile::PersistentPermissionsPolicy::StoreOnDisk;
  }

  DEFINE_GETTER(get_user_config_dir, config_dir)
  DEFINE_SETTER(set_user_config_dir, config_dir)

  QString get_user_config_lua_init_file() const { return config_dir.filePath("init.lua"); }
  QString get_user_config_lua_dir() const { return config_dir.filePath("lua"); }

private:
  void on_update(const QString &name, const QVariant &value) {
    if (name == "user_agent")
      emit user_agent_updated(value.toString());
    else if (name == "downloads_dir")
      emit downloads_dir_updated(value.toString());
    else if (name == "permissions_persistance")
      emit permissions_persistance_updated(value.toString());
  }

signals:
  void user_agent_updated(const QString &value);
  void downloads_dir_updated(const QString &value);
  void permissions_persistance_updated(const QString &value);
};
