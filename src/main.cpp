#include <QApplication>
#include <QMainWindow>

#include "widgets/MainWindow.hpp"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  MainWindow mainWindow;
  mainWindow.setWindowTitle("web-browser");
  mainWindow.show();

  return app.exec();
}
