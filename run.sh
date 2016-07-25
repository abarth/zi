#!/bin/sh

set -e

mkdir -p out
g++ -g --std=c++11 -Werror -Wall -Wextra -Wno-unused-const-variable \
  *.cc -o out/zi
./out/zi "$@"
