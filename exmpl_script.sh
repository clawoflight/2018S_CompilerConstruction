#!/bin/bash

rm -rf builddir
mkdir builddir
cd builddir
meson -Db_coverage=true ..
ninja
./mC_to_dot
ninja -v coverage-html
firefox meson-logs/coveragereport/index.html