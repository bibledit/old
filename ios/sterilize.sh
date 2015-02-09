#!/bin/bash

# Sterilize the Bibledit library configuration.
pushd ../lib
./configure
echo Clean source.
make clean > /dev/null
popd


