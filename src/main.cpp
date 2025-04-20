#include <QApplication>

#include "InstanceManager.hpp"
#include "LuaRuntime.hpp"
#include "widgets/BrowserApp.hpp"

QCommandLineParser *create_cli_parser() {
  auto *parser = new QCommandLineParser();
  parser->setApplicationDescription("null-browser");
  parser->addHelpOption();
  parser->addVersionOption();
  parser->addPositionalArgument("url", "URL(s) to open", "[url]");
  parser->addOptions({
      {{"e", "expr"}, "Lua expression to execute", "lua"},
      {{"c", "config-dir"}, "Config directory for null browser", "dir"},
  });

  return parser;
}

int main(int argc, char *argv[]) {
  const QApplication app(argc, argv);
  QApplication::setApplicationName("null-browser");
  QApplication::setApplicationVersion("0.0.0");

  auto *parser = create_cli_parser();
  parser->process(app);

  auto urls = parser->positionalArguments();
  auto lua_expr = parser->value("expr");
  auto config_dir = parser->value("config-dir");

  Configuration configuration;
  if (!config_dir.isEmpty())
    configuration.set_config_dir(config_dir);

  InstanceManager instance_manager;
  if (instance_manager.is_server()) {
    auto *browser = new BrowserApp(configuration);
    browser->create_window(urls);

    auto &lua = LuaRuntime::instance();
    QObject::connect(&instance_manager, &InstanceManager::lua_eval_requested, &lua,
                     &LuaRuntime::evaluate);
    QObject::connect(&instance_manager, &InstanceManager::urls_open_requested, browser,
                     &BrowserApp::create_window);

    if (!lua_expr.isEmpty())
      lua.evaluate(lua_expr);
  } else {
    qInfo() << "Using current instance";

    if (!urls.isEmpty() || lua_expr.isEmpty())
      instance_manager.write_open_urls_to_socket(urls);

    if (!lua_expr.isEmpty())
      instance_manager.write_lua_expr_to_socket(lua_expr);

    instance_manager.send_message();
    return 0;
  }

  return QApplication::exec();
}
