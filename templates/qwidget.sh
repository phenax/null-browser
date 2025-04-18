#!/usr/bin/env sh

set -e -o pipefail

class_name="$1"
path="$2"

[ -z "$class_name" ] && exit 1

if ! [ -z "$path" ]; then
  mkdir -p "./src/$path"
fi

# Header
echo "#pragma once

#include <QtCore>
#include <QWidget>

class $class_name : public QWidget {
  Q_OBJECT

  public:
  $class_name();
};
" > "./src/$path/$class_name.hpp"

# Implementation
echo "#include <QtCore>
#include <QWidget>

#include \"$path/$class_name.hpp\"

$class_name::$class_name(): QWidget() {}
" > "./src/$path/$class_name.cpp"
