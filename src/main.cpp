#include <QApplication>

#include "App.hpp"

int main(int argc, char *argv[]) {
  const QApplication qt_app(argc, argv);

  App app;
  app.run();

  if (app.is_server())
    return QApplication::exec();
  return 0;
}
