#!/bin/sh

set -e

ninja -C out/Debug
./out/Debug/zi "$@"
