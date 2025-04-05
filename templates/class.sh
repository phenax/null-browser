#!/usr/bin/env sh

set -e -o pipefail

class_name="$1"
path="$2"

[ -z "$class_name" ] && exit 1

if [ -z "$path" ]; then
  path="$path/"
else
  mkdir -p "./src/$path"
fi

# Header
echo "#pragma once

class $class_name {
  public:
  $class_name();
};
" > "./src/$path$class_name.hpp"

# Implementation
echo "

#include \"$path$class_name.hpp\"

$class_name::$class_name() {}
" > "./src/$path$class_name.cpp"
