#!/bin/bash

git clone https://github.com/modelica-tools/FMUComplianceChecker FMUComplianceChecker

git clone  https://github.com/modelon-community/fmi-library.git FMUComplianceChecker/FMI
mkdir build_FMUComplianceChecker
cd build_FMUComplianceChecker
cmake ../FMUComplianceChecker
# warning install in ../install
make install || exit 1
cd ..
rm -rf build_FMUComplianceChecker
