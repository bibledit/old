#!/bin/bash

# Sterilize the Bibledit library configuration.
pushd ../lib
./configure --enable-demo
echo Clean source.
make clean > /dev/null
popd


