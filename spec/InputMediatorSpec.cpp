#include "InputMediator.hpp"
#include "LuaRuntime.hpp"
#include "testUtils.h"
#include "widgets/WebViewStack.hpp"

class InputMediatorSpec : public QObject {
  Q_OBJECT

private slots:
};

QTEST_REGISTER(InputMediatorSpec)
#include "InputMediatorSpec.moc"
