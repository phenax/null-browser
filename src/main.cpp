#include <QApplication>

#include "App.hpp"
#include "schemes/schemes.hpp"

int main(int argc, char *argv[]) {
  register_all_schemes();
  const QApplication qt_app(argc, argv);

  App app;
  app.run();

  if (app.is_server())
    return QApplication::exec();
  return 0;
}
