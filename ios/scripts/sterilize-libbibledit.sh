#!/bin/bash

# Sterilize the Bibledit library configuration.
# Configure it in client mode,
# Run only only one parallel task so the interface is more responsive.
pushd ../../lib
./configure --enable-client --with-parallel-tasks=1
echo Clean source.
make clean > /dev/null
popd


