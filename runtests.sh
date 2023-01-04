#!/usr/bin/sh

ctest -V --test-dir ./build/ --timeout 1
ctest --test-dir ./build/ --rerun-failed --output-on-failure --timeout 1
