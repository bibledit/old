#!/bin/bash

# Build libbibledit on OS X for iOS.

# Sterilize the Bibledit library configuration.
pushd ../lib > /dev/null
make clean > /dev/null
popd > /dev/null



