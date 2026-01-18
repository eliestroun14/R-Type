#!/usr/bin/env bash
set -euo pipefail

cd build
rm -rf *
cmake .. -DENABLE_COVERAGE=ON
make
echo "---------------------------------------------------------------"
make coverage_clean
ctest
echo "---------------------------------------------------------------"
make coverage
