#pragma once

#include <QApplication>
#include <qcommandlineparser.h>

#include "Configuration.hpp"
#include "InstanceManager.hpp"
#include "utils.hpp"

class App {
public:
  App();
  void run();
  DELEGATE((&instance_manager), is_server, is_server)

private:
  InstanceManager instance_manager;
  Configuration configuration;
  QCommandLineParser cli_parser;

  void configure_cli_parser();
  void init_server();
  void init_client();

  QString get_cli_config_dir() { return cli_parser.value("config-dir"); }
  QString get_cli_lua_expr() { return cli_parser.value("expr"); }
  QList<QString> get_cli_urls() { return cli_parser.positionalArguments(); }
};
