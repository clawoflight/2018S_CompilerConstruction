#!/bin/bash

rm -rf builddir
mkdir builddir
cd builddir
meson -Db_coverage=true ..
ninja test
ninja -v coverage-html
firefox meson-logs/coveragereport/index.html