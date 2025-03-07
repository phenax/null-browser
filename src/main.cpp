#include <QApplication>
#include <QVBoxLayout>
#include <QWebEngineView>
#include <QWidget>
#include <lua.hpp>

int main(int argc, char *argv[]) {
  auto L = luaL_newstate();

  luaL_openlibs(L); /* Load Lua libraries */

  /* Load the file containing the script we are going to run */
  auto status = luaL_dostring(L, "print(500 + 10 * 3)");
  if (status) {
    /* If something went wrong, error message is at the top of */
    /* the stack */
    fprintf(stderr, "Couldn't load file: %s\n", lua_tostring(L, -1));
    exit(1);
  }
  lua_close(L);
  exit(0);

  QApplication app(argc, argv);
  QWidget mainWindow;
  mainWindow.setWindowTitle("web-browser");

  auto layout = new QVBoxLayout(&mainWindow);
  auto view = new QWebEngineView();
  view->setFixedSize(400, 400);
  view->setUrl(QUrl("https://ediblemonad.dev"));

  layout->addWidget(view);
  mainWindow.show();

  return app.exec();
}
