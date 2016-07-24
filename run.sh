#!/bin/sh

set -e

mkdir -p out
g++ -g --std=c++11 -Werror -Wall -Wextra zi.cc -o out/zi
./out/zi "$@"
