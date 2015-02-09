#!/bin/bash

# Build libgit2 for the host operating system and the host architecture.

# Clean any previous output.
rm -rf include/*
rm -rf lib/*
mkdir -p include
mkdir -p lib

# Go to the source code directory where to work from.
cd libgit2*

# Clean / create build directory.
rm -rf build
mkdir build
cd build

# Configure builder.
cmake .. -DBUILD_SHARED_LIBS=OFF -DBUILD_CLAR=OFF -DTHREADSAFE=ON

# Build library.
cmake --build .

# Copy the output into its destination.
cp -r ../include/* ../../include
cp libgit2.a ../../lib
cd ..

# Clean build directory.
rm -rf build

echo If everything went well, then libgit2 has been built.
echo The output was copied to include and to lib.
