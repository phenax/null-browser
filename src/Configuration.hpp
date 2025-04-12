#pragma once

#include <QtCore>
#include <unordered_map>

class Configuration : public QObject {
  Q_OBJECT

private:
  std::unordered_map<QString, QVariant> config_map{
      {"new_view_url", "https://duckduckgo.com"},
      {"close_window_when_no_views", true},
  };

public:
  using QObject::QObject;

  void set_config(const QString &name, const QVariant &value) {
    qDebug() << "config update" << name << value;
    config_map[name] = value;
    emit config_updated(name, value);
  }

  QVariant get_config(const QString &name) const { return config_map.at(name); }

  QString new_view_url() const { return get_config("new_view_url").toString(); }
  bool close_window_when_no_views() const {
    return get_config("close_window_when_no_views").toBool();
  }

signals:
  void config_updated(const QString &name, const QVariant &value);
};
