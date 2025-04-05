#include <QApplication>

#include "InstanceManager.hpp"
#include "LuaRuntime.hpp"
#include "WindowActionRouter.hpp"
#include "widgets/BrowserApp.hpp"

QCommandLineParser *create_cli_parser() {
  auto *parser = new QCommandLineParser();
  parser->setApplicationDescription("null-browser");
  parser->addHelpOption();
  parser->addVersionOption();
  parser->addPositionalArgument("url", "URL(s) to open", "[url]");
  parser->addOptions({
      {{"e", "expr"}, "Lua expression to execute", "lua"},
  });

  return parser;
}

int main(int argc, char *argv[]) {
  const QApplication app(argc, argv);
  QApplication::setApplicationName("null-browser");
  QApplication::setApplicationVersion("0.0.0");

  auto *parser = create_cli_parser();
  parser->process(app);

  InstanceManager instance_manager;
  if (instance_manager.is_server()) {
    auto *browser = new BrowserApp;
    browser->create_window();

    auto &lua = LuaRuntime::instance();
    QObject::connect(&instance_manager, &InstanceManager::lua_eval_requested,
                     &lua, &LuaRuntime::evaluate);
    QObject::connect(&instance_manager, &InstanceManager::urls_open_requested,
                     &lua, [browser](const QStringList &urls) {
                       qDebug() << urls;
                       browser->create_window(urls);
                     });
  } else {
    qInfo() << "Using current instance";

    auto urls = parser->positionalArguments();
    auto lua_expr = parser->value("expr");

    if (!urls.isEmpty() || lua_expr.isEmpty())
      instance_manager.write_open_urls_to_socket(urls);

    if (!lua_expr.isEmpty())
      instance_manager.write_lua_expr_to_socket(lua_expr);

    instance_manager.send_message();
    return 0;
  }

  return QApplication::exec();
}
