#include "testUtils.h"

// NOLINTBEGIN
class InputMediatorSpec : public QObject {
  Q_OBJECT

private slots:
};

QTEST_REGISTER(InputMediatorSpec)
#include "InputMediatorSpec.moc"
// NOLINTEND
