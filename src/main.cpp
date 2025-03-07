#include <QApplication>
#include <QVBoxLayout>
#include <QWebEngineView>
#include <QWidget>

int main(int argc, char *argv[]) {
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
