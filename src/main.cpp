#include <QApplication>
#include <QMainWindow>

#include "LuaRuntime.hpp"
#include "widgets/MainWindow.hpp"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  auto lua = LuaRuntime::instance();
  lua->startEventLoop();

  MainWindow mainWindow;
  mainWindow.setWindowTitle("null-browser");
  mainWindow.show();

  return app.exec();
}
