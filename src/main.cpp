#include <QApplication>
#include <QMainWindow>
#include <lua.hpp>

#include "MainWindow.hpp"

int main(int argc, char *argv[]) {
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
