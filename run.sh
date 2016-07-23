#!/bin/sh

set -e

mkdir -p out
gcc --std=c++11 -Werror -Wall -Wextra zi.cc -o out/zi
./out/zi
