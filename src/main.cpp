#include <QApplication>

#include "LuaRuntime.hpp"
#include "widgets/MainWindow.hpp"

int main(int argc, char *argv[]) {
  const QApplication app(argc, argv);

  auto *lua = LuaRuntime::instance();
  lua->start_event_loop();

  MainWindow main_window;
  main_window.setWindowTitle("null-browser");
  main_window.show();

  return QApplication::exec();
}
