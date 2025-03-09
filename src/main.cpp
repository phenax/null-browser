#include <QApplication>
#include <QMainWindow>

#include "widgets/MainWindow.hpp"

int main(int argc, char *argv[]) {
  // QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
  QApplication app(argc, argv);

  MainWindow mainWindow;
  mainWindow.setWindowTitle("web-browser");
  mainWindow.show();

  return app.exec();
}

// auto L = luaL_newstate();
//
// luaL_openlibs(L);
//
// auto status = luaL_dostring(L, "print(500 + 10 * 3)");
// if (status) {
//   fprintf(stderr, "Couldn't load file: %s\n", lua_tostring(L, -1));
//   exit(1);
// }
// lua_close(L);
// exit(0);
