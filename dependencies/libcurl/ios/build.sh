#!/bin/bash

cd ..
cd curl-7*
make distclean
../ios/build-libcurl-ios.sh
make distclean
touch include/curl/curlbuild.h
cp -r include/curl ~/Desktop



