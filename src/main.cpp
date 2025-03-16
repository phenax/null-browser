#include <QApplication>
#include <QMainWindow>

#include "widgets/MainWindow.hpp"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  MainWindow mainWindow;
  mainWindow.setWindowTitle("null-browser");
  mainWindow.show();

  return app.exec();
}
