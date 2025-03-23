#!/usr/bin/env sh

set -e -o pipefail

class_name="$1"
[ -z "$class_name" ] && exit 1

echo "#include <QtCore>
#include \"testUtils.h\"

#include \"$class_name.hpp\"

// NOLINTBEGIN
class ${class_name}Spec : public QObject {
  Q_OBJECT

private slots:
  void testThings() {
    context(\"when initialized\");
    it(\"does things\") {
      $class_name instance;

      QCOMPARE(1, 1);
    }
  }
};

QTEST_REGISTER(${class_name}Spec)
#include \"${class_name}Spec.moc\"
// NOLINTEND
" > "./spec/${class_name}Spec.cpp"

