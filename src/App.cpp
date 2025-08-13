#include "InstanceManager.hpp"
#include "LuaRuntime.hpp"
#include "widgets/BrowserApp.hpp"

#include "App.hpp"

App::App() {
  configure_cli_parser();
  QApplication::setApplicationName("null-browser");
  QApplication::setApplicationVersion("0.0.0");
}

void App::run() {
  cli_parser.process(*qApp);
  instance_manager.initialize();

  auto config_dir = get_cli_config_dir();
  if (!config_dir.isEmpty())
    configuration.set_user_config_dir(config_dir);

  if (instance_manager.is_server()) {
    init_server();
  } else {
    qInfo() << "Using current instance";
    init_client();
  }
}

void App::configure_cli_parser() {
  cli_parser.setApplicationDescription("null-browser");
  cli_parser.addHelpOption();
  cli_parser.addVersionOption();
  cli_parser.addPositionalArgument("url", "URL(s) to open", "[url]");
  cli_parser.addOptions({
      {{"e", "expr"}, "Lua expression to execute", "lua"},
      {{"C", "config-dir"}, "Config directory for null browser", "dir"},
  });
}

void App::init_server() {
  auto urls = get_cli_urls();
  auto lua_expr = get_cli_lua_expr();

  auto *browser = new BrowserApp(configuration);
  browser->create_window(urls);

  auto &lua = LuaRuntime::instance();
  QObject::connect(&instance_manager, &InstanceManager::lua_eval_requested, &lua,
                   &LuaRuntime::evaluate);
  QObject::connect(&instance_manager, &InstanceManager::urls_open_requested, browser,
                   &BrowserApp::create_window);

  if (!lua_expr.isEmpty())
    lua.evaluate(lua_expr);
}

void App::init_client() {
  auto urls = get_cli_urls();
  auto lua_expr = get_cli_lua_expr();

  if (!urls.isEmpty() || lua_expr.isEmpty()) {
    UrlServerMsg msg(urls);
    instance_manager.write_message(msg);
  }

  if (!lua_expr.isEmpty()) {
    LuaExprServerMsg msg(lua_expr);
    instance_manager.write_message(msg);
  }

  instance_manager.close();
}
