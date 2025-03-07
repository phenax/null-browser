#include <QApplication>
#include <QVBoxLayout>
#include <QWebEngineView>
#include <QWidget>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  QWidget mainWindow;
  mainWindow.setWindowTitle("Qt6 WebEngine");

  QVBoxLayout *layout = new QVBoxLayout(&mainWindow);
  QWebEngineView *view = new QWebEngineView();
  view->setFixedSize(400, 400);
  view->setUrl(QUrl("https://www.qt.io"));

  layout->addWidget(view);
  mainWindow.show();

  return app.exec();
}
