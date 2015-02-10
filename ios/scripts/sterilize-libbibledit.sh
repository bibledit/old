#!/bin/bash

# Sterilize the Bibledit library configuration.
pushd ../../lib
./configure --enable-client
echo Clean source.
make clean > /dev/null
popd


