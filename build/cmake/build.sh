#!/bin/bash

pushd dli-exporter
git clean -xdf .
cmake -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX .
make -j16
popd
