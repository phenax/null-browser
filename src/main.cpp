#include <QApplication>

#include "widgets/BrowserApp.hpp"

int main(int argc, char *argv[]) {
  const QApplication app(argc, argv);

  BrowserApp browser;
  browser.create_window();
  browser.create_window();

  return QApplication::exec();
}
