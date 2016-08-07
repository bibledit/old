#!/bin/bash

DEFS="-DHAVE_MSYS"
CFLAGS="-std=c++11 -Wall -Wextra -pedantic -I. -I/opt/local/include -I/usr/include -g -c"



g++ $CFLAGS $DEFS -o library.o library.cpp

rm -f liblibrary.a

ar cru liblibrary.a library.o 

ranlib liblibrary.a

g++ $CFLAGS $DEFS -o bibledit.o bibledit.cpp

g++ -std=c++11 -Wall -Wextra -pedantic -g -o bibledit bibledit.o liblibrary.a -L. -L/opt/local/lib -lpthread -lcurl -lssl -lcrypto -lws2_32

